#include <Arduino.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <WiFiType.h>
#include <WiFi.h>
#include <ESP32Time.h>
#include <ArduinoHttpClient.h>

#define GMT_OFFSET_IN_SECONDS -18000

#define DEBUG
#define VERBOSE_DEBUG
#define DEBUG_COMMUNICATION_SSID "example"
#define DEBUG_COMMUNICATION_PSK "example"
#define DEBUG_BUS_SSID "example"

#define COMMUNICATION_SSID "KUGUEST"
#define BUS_SSID "ridekc"

#define COMMUNICATION_SERVER "timoiv.com"
#define COMMUNICATION_SERVER_PORT 10000

#define TIME_REST_API_URL "worldtimeapi.org"
#define TIME_REST_API_URL_QUERY_STRING "/api/timezone/America/Chicago.txt"

#define DEFAULT_USER_AGENT_STRING "esp32"

#define LAST_BUS_TIME_HOUR 20 //Last bus is around 10 o'clock
#define LAST_BUS_DEEP_SLEEP_HOURS 7ULL
#define LAST_BUS_DEEP_SLEEP_MICROSECONDS (LAST_BUS_DEEP_SLEEP_HOURS * 3600ULL * 1000000ULL) //7 Hours In Deep Sleep

#define MODEM_SLEEP_SECONDS 120

ESP32Time esp_Time;
bool sync_Time_Necessary = true;

void disable_Bluetooth() {
	esp_bluedroid_disable();
	esp_bluedroid_deinit();
	#ifdef DEBUG
	Serial.println("disable_Bluetooth(): Disabled bluetooth stack");
	#endif
	esp_bt_controller_disable();
	esp_bt_controller_deinit();
	
	#ifdef DEBUG
	Serial.println("disable_Bluetooth(): Disabled bluetooth");
	#endif
}

void enable_Disable_Wifi(bool enable) {
	if (enable) {
		#ifdef DEBUG
		Serial.println("enable_Wifi(): WiFi entering station mode");
		#endif
		WiFi.mode(WIFI_AP_STA);
	} else {
		#ifdef DEBUG
		Serial.println("enable_Wifi(): WiFi entering off mode");
		#endif
		WiFi.mode(WIFI_OFF);
	}
}

void setup() {
	#ifdef DEBUG
	//This is so we don't miss boot messages
	delay(5000);
	Serial.begin(9600);
	while (!Serial) {}
	Serial.println("Booting...");
	#endif

	disable_Bluetooth();
	enable_Disable_Wifi(true);

	esp_Time = ESP32Time(GMT_OFFSET_IN_SECONDS);
}


void enter_Deep_Sleep_If_Ready() {
	if (esp_Time.getHour(true) > 20) {
		esp_sleep_enable_timer_wakeup(LAST_BUS_DEEP_SLEEP_MICROSECONDS);
		#ifdef DEBUG
		Serial.println("enter_Deep_Sleep_If_Ready(): Entering deep sleep now...");
		#endif
		esp_deep_sleep_start();
		#ifdef DEBUG
		Serial.println("enter_Deep_Sleep_If_Ready(): Waking up from deep sleep now...");
		#endif
		sync_Time_Necessary = true;
		return;
	}
	#ifdef DEBUG
	Serial.println("enter_Deep_Sleep_If_Ready(): Deep sleep was not necessary");
	#endif
}

//This changes the data in the buffer
int32_t get_Current_Unix_Time(char * buffer) {
	char * token;
	
	token = strtok(buffer, "\r\n");
	
	do {
		if (!strncmp("unixtime:", token, 9)) {
			#ifdef DEBUG
			Serial.println("get_Current_Unix_Time(): Found current Unix Time in buffer");
			#endif
			char * number = 10 + token;
			return String(number).toInt();
		}
	} while ((token = strtok(NULL, "\r\n")) != NULL);

	#ifdef DEBUG
	Serial.println("get_Current_Unix_Time(): Couldn't find current time in buffer");
	#endif
	
	return -1;
}

char * create_Mutable_String(const char * immutable_String) {
	const int length = strlen(immutable_String) + 1;	 
	char * return_String = new char[length];
	

	memcpy((void *) return_String, (void *) immutable_String, length);
	return return_String;
}

bool sync_Time() {
	#ifdef DEBUG
	Serial.println("sync_Time(): Entering sync_Time() function");
	#endif

	WiFiClient w_Client;
	HttpClient http = HttpClient(w_Client, TIME_REST_API_URL);

	int try_Count = 0;

	while (try_Count <= 10 && http.get(TIME_REST_API_URL_QUERY_STRING)) {
		#ifdef DEBUG
		Serial.println("sync_Time(): Attempting request...");
		#endif
		delay(1000);
		try_Count += 1;
	}

	if (try_Count > 10) {
		#ifdef DEBUG
		Serial.println("sync_Time(): Time syncing failed");
		return false;
		#endif
	}

	int status_Code = http.responseStatusCode();

	#ifdef VERBOSE_DEBUG
	Serial.print("sync_Time(): HTTP status code: ");
	Serial.println(status_Code);
	#endif

	if (status_Code >= 400 && status_Code < 500) {
		#ifdef DEBUG
		Serial.println("sync_Time(): HTTP error code");	
		#endif
		return false;
	}
	
	String response = http.responseBody();
	
	#ifdef VERBOSE_DEBUG
	Serial.print("sync_Time(): Server content: ");
	Serial.println(response);
	#endif
	
	//reponse_Immutable is invalid when the response string is destroyed
	//So, basically, no freeing is necessary
	//Unix time function should work on strings directly, but changes in implementations caused
	//such a situation where it did not.
	const char * response_Immutable = response.c_str();
	char * response_Mutable = create_Mutable_String(response_Immutable);
	
	int32_t u_Time = get_Current_Unix_Time(response_Mutable);
	
	#ifdef VERBOSE_DEBUG
	Serial.print("sync_Time(): Found current Unix Time: ");
	Serial.println(u_Time);
	#endif

	if (u_Time == -1) {
		#ifdef DEBUG
		Serial.println("sync_Time(): Failed to sync, couldn't find time string");
		#endif
		delete response_Mutable;
		return false;
	}

	#ifdef DEBUG
	Serial.println("sync_Time(): Setting esp32 RTC");
	#endif
	esp_Time.setTime(u_Time);

	delete response_Mutable;

	return true;
}

//This only exists for logging
void disconnect_From_SSID() {
	#ifdef DEBUG
	WiFi.disconnect();
	Serial.println("disconnect_From_SSID(): Disconnected from SSID");
	#endif
}

void connect_To_SSID(char *ssid, char * password) {
	disconnect_From_SSID();
	while (WiFi.status() != WL_CONNECTED) {
		if (password == (char *)0) {
		} else {
			WiFi.begin(ssid, password);
		}
		
		#ifdef DEBUG
		Serial.print("connect_To_SSID(): attempting connection to SSID:");
		Serial.println(ssid);
		#endif
		
		delay(10000);
	}
	
	#ifdef DEBUG
	Serial.print("connect_To_SSID(): created a connection to SSID:");
	Serial.println(ssid);
	#endif
	
	return;
}


bool is_SSID_Present(const String ssid) {
	const int count = WiFi.scanNetworks();
	
	#ifdef DEBUG
	if (count == -1) {
		Serial.println("is_SSID_Present(): Scanning error");
	}
	#endif

	if (count == -1) return false;
	
	for (int i = 0; i < count; i++) {
		String current_SSID = WiFi.SSID(i);
		if (ssid.equals(current_SSID)) {
			#ifdef DEBUG
			Serial.print("is_SSID_Present(): Found SSID: ");
			Serial.println(current_SSID);
			#endif
			return true;
		} 

		#ifdef VERBOSE_DEBUG 
		else {
			Serial.print("is_SSID_Present(): Checking SSID:  ");
			Serial.println(current_SSID);
		}
		#endif
	}
	
	#ifdef DEBUG
	Serial.println("is_SSID_Present(): Could not find SSID");
	#endif
	
	return false;
}

void perform_KU_Portal_Sign_In() {
	return;
}

void modem_Sleep(int seconds) {
	#ifdef DEBUG
	Serial.println("modem_Sleep(): Beginning to enter modem sleep");
	#endif

	enable_Disable_Wifi(false);
	
	#ifdef DEBUG
	Serial.println("modem_Sleep(): Lowering clock speed to 80 mhz");
	#endif

	setCpuFrequencyMhz(80);

	#ifdef DEBUG
	Serial.println("modem_Sleep(): Entering modem sleep");
	#endif
	
	delay(seconds * 1000);

	#ifdef DEBUG
	Serial.println("modem_Sleep(): Beginning exit out of modem sleep");
	Serial.println("modem_Sleep(): Raising Clock speed to 240 mhz");
	#endif

	setCpuFrequencyMhz(240);
	
	enable_Disable_Wifi(true);

	#ifdef DEBUG
	Serial.println("modem_Sleep(): Exiting modem sleep");
	#endif
}

const char * get_Status_String() {
	return "BUS ARRIVED...";	
}

void announce_Bus_Arrival() {
	WiFiClient w_Client;
	HttpClient http = HttpClient(w_Client, COMMUNICATION_SERVER, COMMUNICATION_SERVER_PORT);
	
	unsigned long arrival_Time = esp_Time.getEpoch();

	#ifdef DEBUG
	Serial.print("announce_Bus_Arrival(): Bus arrival time in Unix Time: ");
	Serial.println(arrival_Time);
	#endif
	
	String time_String = String(arrival_Time);
	String filename = "/" + time_String + ".txt";
	
	http.put(filename, "text/plain", get_Status_String());
	
	int status_Code = http.responseStatusCode();

	#ifdef VERBOSE_DEBUG
	Serial.print("announce_Bus_Arrival(): HTTP PUT status code: ");
	Serial.println(status_Code);
	#endif

	#ifdef DEBUG
	Serial.println("announce_Bus_Arrival(): Posted to server");
	#endif
	return;
}

void loop() {
	if (sync_Time_Necessary) {
		#ifdef DEBUG
			connect_To_SSID(DEBUG_COMMUNICATION_SSID, DEBUG_COMMUNICATION_PSK);
		#else
			connect_To_SSID(COMMUNICATION_SSID, (char *) 0);		
		#endif
		sync_Time();
		sync_Time_Necessary = false;
	}

	modem_Sleep(MODEM_SLEEP_SECONDS);

	#ifdef DEBUG
	if (is_SSID_Present(DEBUG_BUS_SSID)) {
		announce_Bus_Arrival();
	}
	#else
	if (is_SSID_Present(BUS_SSID)) {
		announce_Bus_Arrival();
		perform_KU_Portal_Sign_In();
	}
	#endif
	
	enter_Deep_Sleep_If_Ready();
}
