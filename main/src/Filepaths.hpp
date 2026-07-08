#ifndef ARTEMIS_FIRMWARE_FILEPATHS_HPP
#define ARTEMIS_FIRMWARE_FILEPATHS_HPP

namespace File {
	[[maybe_unused]] constexpr const char* BackgroundBottom = "S:/bg_bot.bin";
	[[maybe_unused]] constexpr const char* Background = "S:/bg.bin";
	[[maybe_unused]] constexpr const char* ModalBackground = "S:/ModalBg.bin";

	namespace Level {
		[[maybe_unused]] constexpr const char* Background = "S:/level/bg.bin";
		[[maybe_unused]] constexpr const char* Bubble = "S:/level/bubble.bin";
		[[maybe_unused]] constexpr const char* MarkingsCenter = "S:/level/markingsCenter.bin";
		[[maybe_unused]] constexpr const char* MarkingsHorizontal = "S:/level/markingsHorizontal.bin";
		[[maybe_unused]] constexpr const char* MarkingsVertical = "S:/level/markingsVertical.bin";
	}

	namespace Intro {
		[[maybe_unused]] constexpr const char* Artemis = "S:/intro/artemis.bin";
		[[maybe_unused]] constexpr const char* BlackBackground = "S:/intro/blackBg.bin";
		[[maybe_unused]] constexpr const char* CircuitMess = "S:/intro/cm.bin";
		[[maybe_unused]] constexpr const char* GeekClub = "S:/intro/geek.bin";
		[[maybe_unused]] constexpr const char* OrangeBackground = "S:/intro/orangeBg.bin";
		[[maybe_unused]] constexpr const char* Space = "S:/intro/space.bin";
	}

	namespace LockScreen {
		namespace Theme1 {
			[[maybe_unused]] constexpr const char* Background = "S:/theme1/lock_screen/bg.bin";
			[[maybe_unused]] constexpr const char* Phone = "";
			[[maybe_unused]] constexpr const char* PhoneDisconnected = "";
			[[maybe_unused]] constexpr const char* BatteryLevel = "";
			[[maybe_unused]] constexpr const char* Clock = "";
			[[maybe_unused]] constexpr const char* Battery1 = "";
			[[maybe_unused]] constexpr const char* Battery2 = "";
			[[maybe_unused]] constexpr const char* Battery3 = "";
			[[maybe_unused]] constexpr const char* Battery4 = "";
			[[maybe_unused]] constexpr const char* Charging = "";
		}
	}

	namespace Clock {
		namespace Theme1 {
			[[maybe_unused]] constexpr const char* Num0 = "S:/theme1/clock/0.bin";
			[[maybe_unused]] constexpr const char* Num1 = "S:/theme1/clock/1.bin";
			[[maybe_unused]] constexpr const char* Num2 = "S:/theme1/clock/2.bin";
			[[maybe_unused]] constexpr const char* Num3 = "S:/theme1/clock/3.bin";
			[[maybe_unused]] constexpr const char* Num4 = "S:/theme1/clock/4.bin";
			[[maybe_unused]] constexpr const char* Num5 = "S:/theme1/clock/5.bin";
			[[maybe_unused]] constexpr const char* Num6 = "S:/theme1/clock/6.bin";
			[[maybe_unused]] constexpr const char* Num7 = "S:/theme1/clock/7.bin";
			[[maybe_unused]] constexpr const char* Num8 = "S:/theme1/clock/8.bin";
			[[maybe_unused]] constexpr const char* Num9 = "S:/theme1/clock/9.bin";
			[[maybe_unused]] constexpr const char* Colon = "S:/theme1/clock/colon.bin";
			[[maybe_unused]] constexpr const char* Space = "S:/theme1/clock/space.bin";
		}
	}

	namespace Icons {
		namespace Theme1 {
			[[maybe_unused]] constexpr const char* AppInst = "S:/theme1/icon/app_inst.bin";
			[[maybe_unused]] constexpr const char* AppMess = "S:/theme1/icon/app_mess.bin";
			[[maybe_unused]] constexpr const char* AppSms = "S:/theme1/icon/app_sms.bin";
			[[maybe_unused]] constexpr const char* AppSnap = "S:/theme1/icon/app_snap.bin";
			[[maybe_unused]] constexpr const char* AppTiktok = "S:/theme1/icon/app_tiktok.bin";
			[[maybe_unused]] constexpr const char* AppWapp = "S:/theme1/icon/app_wapp.bin";
			[[maybe_unused]] constexpr const char* Back = "S:/theme1/icon/back.bin";
			[[maybe_unused]] constexpr const char* BackSel = "S:/theme1/icon/back_sel.bin";
			[[maybe_unused]] constexpr const char* CallIn = "S:/theme1/icon/call_in.bin";
			[[maybe_unused]] constexpr const char* CallMiss = "S:/theme1/icon/call_miss.bin";
			[[maybe_unused]] constexpr const char* CallOut = "S:/theme1/icon/call_out.bin";
			[[maybe_unused]] constexpr const char* CatEmail = "S:/theme1/icon/cat_email.bin";
			[[maybe_unused]] constexpr const char* CatEntert = "S:/theme1/icon/cat_entert.bin";
			[[maybe_unused]] constexpr const char* CatFin = "S:/theme1/icon/cat_fin.bin";
			[[maybe_unused]] constexpr const char* CatHealth = "S:/theme1/icon/cat_health.bin";
			[[maybe_unused]] constexpr const char* CatLoc = "S:/theme1/icon/cat_loc.bin";
			[[maybe_unused]] constexpr const char* CatNews = "S:/theme1/icon/cat_news.bin";
			[[maybe_unused]] constexpr const char* CatOther = "S:/theme1/icon/cat_other.bin";
			[[maybe_unused]] constexpr const char* CatSched = "S:/theme1/icon/cat_sched.bin";
			[[maybe_unused]] constexpr const char* CatSoc = "S:/theme1/icon/cat_soc.bin";
			[[maybe_unused]] constexpr const char* LockClosed = "S:/theme1/icon/lock_closed.bin";
			[[maybe_unused]] constexpr const char* LockOpen = "S:/theme1/icon/lock_open.bin";
			[[maybe_unused]] constexpr const char* Trash = "S:/theme1/icon/trash.bin";
			[[maybe_unused]] constexpr const char* TrashSel = "S:/theme1/icon/trash_sel.bin";
		}
	}

	namespace SmallIcons {
		namespace Theme1 = Icons::Theme1;
	}

	namespace Weather {
		namespace Theme1 {
			[[maybe_unused]] constexpr const char* Sun = "S:/theme1/weather/sun.bin";
			[[maybe_unused]] constexpr const char* Cloud = "S:/theme1/weather/cloud.bin";
			[[maybe_unused]] constexpr const char* Rain = "S:/theme1/weather/rain.bin";
		}
	}

	namespace Menu {
		namespace Theme1 {
			[[maybe_unused]] constexpr const char* BatteryEmpty = "S:/theme1/menu/batteryEmpty.bin";
			[[maybe_unused]] constexpr const char* BatteryFull = "S:/theme1/menu/batteryFull.bin";
			[[maybe_unused]] constexpr const char* BatteryLow = "S:/theme1/menu/batteryLow.bin";
			[[maybe_unused]] constexpr const char* BatteryMid = "S:/theme1/menu/batteryMid.bin";
			[[maybe_unused]] constexpr const char* BigLowBattery = "S:/theme1/menu/bigLowBattery.bin";
			[[maybe_unused]] constexpr const char* Phone = "S:/theme1/menu/phone.bin";
			[[maybe_unused]] constexpr const char* PhoneDisconnected = "S:/theme1/menu/phoneDc.bin";
			[[maybe_unused]] constexpr const char* Background = "S:/theme1/menu/bg.bin";
			[[maybe_unused]] constexpr const char* SettingsBackground = "S:/theme1/menu/bg2.bin";
			[[maybe_unused]] constexpr const char* Connection = "S:/theme1/menu/connection.bin";
			[[maybe_unused]] constexpr const char* Find = "S:/theme1/menu/find.bin";
			[[maybe_unused]] constexpr const char* Level = "S:/theme1/menu/level.bin";
			[[maybe_unused]] constexpr const char* Settings = "S:/theme1/menu/settings.bin";
			[[maybe_unused]] constexpr const char* Notification = "S:/theme1/menu/notif.bin";
		}
	}
}

#define THEMED_FILE(Category, Filename, theme) File::Category::Theme1::Filename

#endif //ARTEMIS_FIRMWARE_FILEPATHS_HPP
