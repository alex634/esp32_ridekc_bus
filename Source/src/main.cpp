#include <Arduino.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <WiFiType.h>
#include <WiFi.h>
#include <ESP32Time.h>
#include <ArduinoHttpClient.h>
#include "./fs_vars.hpp"
#include "./logs.hpp"
#include "./wifi.hpp"

ESP32Time esp_Time;
bool sync_Time_Necessary = true;

void disable_Bluetooth() {
	esp_bluedroid_disable();
	esp_bluedroid_deinit();
	
	logs_println("disable_Bluetooth(): Disabled bluetooth stack");

	esp_bt_controller_disable();
	esp_bt_controller_deinit();	

	logs_println("disable_Bluetooth(): Disabled bluetooth");

}

void setup() {
	//This is so we don't miss boot messages
	delay(5000);
	fs_vars_mount_Filesystem();
	
	logs_initialize_Logs(fs_vars_debug_Mode_Enabled());
	logs_println("Initialized Logging");
	fs_vars_load_Misc_Vars();

	disable_Bluetooth();
	wifi_enable_Disable_Wifi(true);

	esp_Time = ESP32Time(fs_vars_m_Vars.GMT_OFFSET_SEC);
}


void enter_Deep_Sleep_If_Ready() {
	if (esp_Time.getHour(true) > 20) {
		esp_sleep_enable_timer_wakeup(fs_vars_m_Vars.DEEP_SLEEP_HOURS * 3600ULL * 1000000ULL);
		
		logs_println("enter_Deep_Sleep_If_Ready(): Entering deep sleep now...");
		logs_deinitialize_Logs();
		fs_vars_free_Misc_Vars();
		
		esp_deep_sleep_start();
		
		
	}
	
	logs_println("enter_Deep_Sleep_If_Ready(): Deep sleep was not necessary");
	
}

//This changes the data in the buffer
int32_t get_Current_Unix_Time(char * buffer) {
	char * token;
	
	token = strtok(buffer, "\r\n");
	
	do {
		if (!strncmp("unixtime:", token, 9)) {
			
			logs_println("get_Current_Unix_Time(): Found current Unix Time in buffer");
				
			char * number = 10 + token;
			return String(number).toInt();
		}
	} while ((token = strtok(NULL, "\r\n")) != NULL);

	
	logs_println("get_Current_Unix_Time(): Couldn't find current time in buffer");

	
	return -1;
}

char * create_Mutable_String(const char * immutable_String) {
	const int length = strlen(immutable_String) + 1;	 
	char * return_String = new char[length];
	

	memcpy((void *) return_String, (void *) immutable_String, length);
	return return_String;
}

bool sync_Time() {
	
	logs_println("sync_Time(): Entering sync_Time() function");
	

	WiFiClient w_Client;
	HttpClient http = HttpClient(w_Client, fs_vars_m_Vars.TIME_REST_SERVER);

	int try_Count = 0;

	while (try_Count <= 10 && http.get(fs_vars_m_Vars.TIME_REST_QUERY)) {
		
		logs_println("sync_Time(): Attempting request...");
		
		delay(1000);
		try_Count += 1;
	}

	if (try_Count > 10) {
		
		logs_println("sync_Time(): Time syncing failed");
		http.stop();
		return false;
		
	}

	int status_Code = http.responseStatusCode();

	
	logs_print("sync_Time(): HTTP status code: ");
	logs_println(String(status_Code));
	

	if (status_Code >= 400 && status_Code < 500) {
		
		logs_println("sync_Time(): HTTP error code");	
		http.stop();
		return false;
	}
	
	String response = http.responseBody();
	
	
	logs_print("sync_Time(): Server content: ");
	logs_println(response);
	
	
	//reponse_Immutable is invalid when the response string is destroyed
	//So, basically, no freeing is necessary
	//Unix time function should work on strings directly, but changes in implementations caused
	//such a situation where it did not.
	const char * response_Immutable = response.c_str();
	char * response_Mutable = create_Mutable_String(response_Immutable);
	
	int32_t u_Time = get_Current_Unix_Time(response_Mutable);
	
	logs_print("sync_Time(): Found current Unix Time: ");
	logs_println(String(u_Time));

	if (u_Time == -1) {
		logs_println("sync_Time(): Failed to sync, couldn't find time string");
		delete response_Mutable;
		http.stop();
		return false;
	}

	logs_println("sync_Time(): Setting esp32 RTC");
	esp_Time.setTime(u_Time);
	logs_enable_Rtc(esp_Time);

	delete response_Mutable;
	
	http.stop();
	return true;
}

void modem_Sleep(int seconds) {
	logs_println("modem_Sleep(): Beginning to enter modem sleep");

	wifi_enable_Disable_Wifi(false);
	
	logs_println("modem_Sleep(): Lowering clock speed to 80 mhz");

	setCpuFrequencyMhz(80);

	logs_println("modem_Sleep(): Entering modem sleep");
	
	delay(seconds * 1000);

	logs_println("modem_Sleep(): Beginning exit out of modem sleep");
	logs_println("modem_Sleep(): Raising Clock speed to 240 mhz");

	setCpuFrequencyMhz(240);
	
	wifi_enable_Disable_Wifi(true);

	logs_println("modem_Sleep(): Exiting modem sleep");
}

const char * get_Status_String() {
	return "BUS ARRIVED...";	
}

void announce_Bus_Arrival() {
	wifi_connect_To_Communication_Wifi();

	WiFiClient w_Client;
	HttpClient http = HttpClient(w_Client, fs_vars_m_Vars.COMMUN_SERVER, fs_vars_m_Vars.COMMUN_SERVER_PORT);
	
	unsigned long arrival_Time = esp_Time.getEpoch();

	logs_println("announce_Bus_Arrival(): Getting bus arrival time from rtc ");
	
	String time_String = String(arrival_Time);
	String filename = "/" + time_String + ".txt";
	
	http.put(filename, "text/plain", get_Status_String());
	
	int status_Code = http.responseStatusCode();

	logs_print("announce_Bus_Arrival(): HTTP PUT status code: ");
	logs_println(String(status_Code));

	logs_println("announce_Bus_Arrival(): Posted to server");
	http.stop();
	return;
}

void loop() {
	if (sync_Time_Necessary) {
		wifi_connect_To_Communication_Wifi();
		sync_Time();
		sync_Time_Necessary = false;
	}

	modem_Sleep(fs_vars_m_Vars.MODEM_SLEEP_SEC);

	if (wifi_bus_Wifi_Present()) announce_Bus_Arrival();
	
	enter_Deep_Sleep_If_Ready();
}
