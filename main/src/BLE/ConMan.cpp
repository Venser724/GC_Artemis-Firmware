#include "ConMan.h"
#include <cstring>

ConManager ConMan;

void ConManager::confDone(bool success){
	conConf.confDone(success);
}

void ConManager::connect(const esp_bd_addr_t addr){
	connected = true;
	memcpy(current, addr, 6);
	setCon();
}

void ConManager::disconnect(){
	connected = false;
	conConf.reset();
	setAdv();
}

void ConManager::goLowPow(){
	lowPow = true;
	if(connected){
		setCon();
	}else{
		setAdv();
	}
}

void ConManager::goHiPow(){
	lowPow = false;
	if(connected){
		setCon();
	}else{
		setAdv();
	}
}

void ConManager::setAdv(){
	// Sleep/wake (goLowPow/goHiPow) calls this whenever !connected, which includes right after
	// a disconnect already restarted advertising - stop first so we don't ask the controller to
	// start advertising while a previous session (possibly with different params) is still active.
	esp_ble_gap_stop_advertising();
	esp_ble_gap_start_advertising((esp_ble_adv_params_t*) (lowPow ? &AdvLowPow : &AdvHiPow));
}

void ConManager::setCon(){
	/* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
	esp_ble_conn_update_params_t params = {};
	memcpy(&params, lowPow ? &ConLowPow : &ConHiPow, sizeof(esp_ble_conn_update_params_t));
	memcpy(params.bda, current, 6);
	conConf.conf(params);
}
