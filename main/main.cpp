#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>
#include <nvs_flash.h>
#include <bootloader_random.h>
#include <esp_random.h>
#include <esp_system.h>
#include <esp_heap_caps.h>
#include <Devices/BatteryV2.h>
#include <Devices/BatteryV3.h>

#include "Settings/Settings.h"
#include "Pins.hpp"
#include "Periph/I2C.h"
#include "Periph/PinOut.h"
#include "Periph/Bluetooth.h"
#include "Devices/Battery.h"
#include "Devices/Display.h"
#include "Devices/Input.h"
#include "Devices/IMU.h"
#include "BLE/GAP.h"
#include "BLE/Client.h"
#include "BLE/Server.h"
#include "Notifs/Phone.h"
#include "LV_Interface/LVGL.h"
#include "LV_Interface/FSLVGL.h"
#include "LV_Interface/InputLVGL.h"
#include <lvgl/lvgl.h>
#include "Util/EfuseMeta.h"
#include "Theme/theme.h"
#include "Util/Services.h"
#include "Services/BacklightBrightness.h"
#include "Services/ChirpSystem.h"
#include "Services/Time.h"
#include "Services/StatusCenter.h"
#include "Services/SleepMan.h"
#include "Services/Weather.h"
#include "Screens/ShutdownScreen.h"
#include "Screens/Lock/LockScreen.h"
#include "JigHWTest/JigHWTest.h"
#include "Util/Notes.h"
#include "Screens/Intro/IntroScreen.h"
#include "Filepaths.hpp"

LVGL* lvgl;
BacklightBrightness* bl;
SleepMan* sleepMan;

// --- crash / memory diagnostics (persistent, kept on to catch any regression) ---
static const char* resetReasonStr(esp_reset_reason_t r){
	switch(r){
		case ESP_RST_POWERON:   return "POWERON";
		case ESP_RST_EXT:       return "EXT";
		case ESP_RST_SW:        return "SW";
		case ESP_RST_PANIC:     return "PANIC";
		case ESP_RST_INT_WDT:   return "INT_WDT";
		case ESP_RST_TASK_WDT:  return "TASK_WDT";
		case ESP_RST_WDT:       return "OTHER_WDT";
		case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
		case ESP_RST_BROWNOUT:  return "BROWNOUT";
		case ESP_RST_SDIO:      return "SDIO";
		case ESP_RST_USB:       return "USB";
		default:                return "UNKNOWN";
	}
}

[[noreturn]] static void heapDiagTask(void*){
	while(true){
		// total = internal+PSRAM (PSRAM dominates ~2 MB and would mask a leak);
		// internal RAM (~300 KB) is the scarce pool where a notification leak bites.
		// retention pool included to verify the sleep fix: with light-sleep PM configured once,
		// the CPU-retention buffer is allocated a single time, so these should stay constant across
		// sleep/wake cycles instead of the largest block shrinking (the fragmentation that crashed us).
		// dma pool added to hunt a btController "Mem alloc fail ... caps 0x1808" crash (DMA|INTERNAL|
		// DEFAULT) seen once in a coredump - suspected fragmentation of this pool from BLE traffic,
		// same shape as the retention-pool bug, different pool.
		printf("[DIAG] total free=%u min=%u largest=%u | internal free=%u min=%u largest=%u | retention free=%u largest=%u | dma free=%u largest=%u\n",
			   (unsigned) esp_get_free_heap_size(),
			   (unsigned) esp_get_minimum_free_heap_size(),
			   (unsigned) heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
			   (unsigned) heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
			   (unsigned) heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL),
			   (unsigned) heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL),
			   (unsigned) heap_caps_get_free_size(MALLOC_CAP_RETENTION),
			   (unsigned) heap_caps_get_largest_free_block(MALLOC_CAP_RETENTION),
			   (unsigned) heap_caps_get_free_size(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL),
			   (unsigned) heap_caps_get_largest_free_block(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));

		// Bangle's task has only a 4 KB stack and getProperty() is regex-heavy (4x
		// std::regex_replace per property) - watching headroom here to check whether a burst of
		// musicinfo/musicstate messages is what's driving it toward overflow.
		TaskHandle_t bangleTask = xTaskGetHandle("Bangle");
		if(bangleTask != nullptr){
			printf("[DIAG] Bangle task stack min-free=%u words (%u bytes)\n",
				   (unsigned) uxTaskGetStackHighWaterMark(bangleTask),
				   (unsigned) (uxTaskGetStackHighWaterMark(bangleTask) * sizeof(StackType_t)));
		}

		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

void shutdown(){
	lvgl->startScreen([](){ return std::make_unique<ShutdownScreen>(); });

	if(!lvgl->running()){
		lvgl->start();
	}

	lv_timer_handler();
	sleepMan->wake(true);
	if(!bl->isOn()){
		bl->fadeIn();
	}
	vTaskDelay(SleepMan::ShutdownTime-1000);
	sleepMan->shutdown();
}

//This works since it's same across all revisions.
//Change if necessary (since setLEDs() call could mess up another hardware revision)
static const gpio_num_t LEDs[] = { GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_43, GPIO_NUM_44, GPIO_NUM_45, GPIO_NUM_46 };

void setLEDs(){
	for(int i = 0; i < sizeof(LEDs) / sizeof(LEDs[0]); i++){
		const gpio_num_t pin = LEDs[i];
		gpio_config_t config = {
				.pin_bit_mask = 1ULL << pin,
				.mode = GPIO_MODE_OUTPUT,
				.pull_up_en = GPIO_PULLUP_DISABLE,
				.pull_down_en = GPIO_PULLDOWN_DISABLE,
				.intr_type = GPIO_INTR_DISABLE
		};

		gpio_config(&config);
		gpio_set_level(pin, 0);
	}
}

void init(){
	esp_reset_reason_t resetReason = esp_reset_reason();
	printf("[DIAG] boot reset_reason=%d (%s) free_heap=%u\n",
		   (int) resetReason, resetReasonStr(resetReason), (unsigned) esp_get_free_heap_size());

	setLEDs();
	gpio_install_isr_service(ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM);

	auto ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	auto settings = new Settings();
	Services.set(Service::Settings, settings);

	if(JigHWTest::checkJig()){
		printf("Jig\n");
		Pins::setLatest();
		auto test = new JigHWTest();
		test->start();
		vTaskDelete(nullptr);
	}else{
		printf("Hello\n");
	}

	if(!EfuseMeta::check()){
		while(true){
			vTaskDelay(1000);
			EfuseMeta::log();
		}
	}

	uint8_t rev = 0;
	if(!EfuseMeta::readRev(rev)){
		while(true){
			vTaskDelay(1000);
			printf("Failed to read hardware revision.");
		}
	}

	auto adc1 = new ADC(ADC_UNIT_1);

	auto blPwm = new PWM(Pins::get(Pin::LedBl), LEDC_CHANNEL_1, true);
	blPwm->detach();
	bl = new BacklightBrightness(blPwm);
	Services.set(Service::Backlight, bl);

	auto buzzPwm = new PWM(Pins::get(Pin::Buzz), LEDC_CHANNEL_0);
	auto audio = new ChirpSystem(*buzzPwm);
	Services.set(Service::Audio, audio);

	auto i2c = new I2C(I2C_NUM_0, (gpio_num_t) Pins::get(Pin::I2cSda), (gpio_num_t) Pins::get(Pin::I2cScl));
	auto imu = new IMU(*i2c);
	Services.set(Service::IMU, imu);

	auto disp = new Display(rev);
	auto input = new Input();
	Services.set(Service::Input, input);

	lvgl = new LVGL(*disp);
	auto theme = theme_init(lvgl->disp());
	lv_disp_set_theme(lvgl->disp(), theme);

	auto lvglInput = new InputLVGL();
	auto fs = new FSLVGL('S');

	sleepMan = new SleepMan(*lvgl);
	Services.set(Service::Sleep, sleepMan);

	auto status = new StatusCenter();
	Services.set(Service::Status, status);

	Battery* battery = nullptr;

	if(rev == 0 || rev == 1){
		battery = new BatteryV2(*adc1);
	}else{
		battery = new BatteryV3(*adc1);
	}

	if(battery->isShutdown()) return; // Stop initialization if battery is critical
	Services.set(Service::Battery, battery);

	auto rtc = new RTC(*i2c);
	auto time = new Time(*rtc);
	Services.set(Service::Time, time); // Time service is required as soon as Phone is up

	auto weather = new Weather();
	Services.set(Service::Weather, weather);

	auto bt = new Bluetooth();
	auto gap = new BLE::GAP();
	auto client = new BLE::Client(gap);
	auto server = new BLE::Server(gap);
	auto phone = new Phone(server, client);
	server->start();
	Services.set(Service::Phone, phone);

	srand(esp_random());

	FSLVGL::loadCache(settings->get().themeData.theme);

	// Load start screen here
	lvgl->startScreen([](){ return std::make_unique<IntroScreen>(); });

	// Start UI thread after initialization
	lvgl->start();

	bl->fadeIn();

	if(settings->get().notificationSounds){
		audio->play({
							Chirp{ .startFreq = 250, .endFreq = 250, .duration = 100 },
							Chirp{ .startFreq = 0, .endFreq = 0, .duration = 100 },
							Chirp{ .startFreq = 250, .endFreq = 250, .duration = 100 },
							Chirp{ .startFreq = 0, .endFreq = 0, .duration = 250 },
							Chirp{ .startFreq = 500, .endFreq = 500, .duration = 200 }
					});

		vTaskDelay(2500);
	}

	// Start Battery scanning after everything else, otherwise Critical
	// Battery event might come while initialization is still in progress
	battery->begin();

	// periodic heap/fragmentation logger (own task; NOT the 2 KB Time task,
	// whose stack would overflow if a printing log ran there)
	xTaskCreate(heapDiagTask, "heapDiag", 3072, nullptr, 1, nullptr);
}

extern "C" void app_main(void){
	init();

	vTaskDelete(nullptr);
}
