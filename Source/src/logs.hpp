#ifndef LOGS_HPP
#define LOGS_HPP

#include <Arduino.h>
#include <esp_spiffs.h>
#include <FS.h>
#include <ESP32Time.h>
#include "./fs_vars.hpp"

static bool logs_rtc_Set = false;
static ESP32Time logs_rtc;
static File logs_file_Handle;
static bool logs_serial = false;

void logs_enable_Rtc(ESP32Time e_Time) {
	logs_rtc = e_Time;
	logs_rtc_Set = true;
}

void logs_initialize_Logs(bool debug) {
	Serial.begin(9600);
	if (SPIFFS.exists("/LOGS/PREVIOUS")) SPIFFS.remove("/LOGS/PREVIOUS");
	if (SPIFFS.exists("/LOGS/CURRENT")) SPIFFS.rename("/LOGS/CURRENT", "/LOGS/PREVIOUS");
	logs_file_Handle = SPIFFS.open("/LOGS/CURRENT", FILE_WRITE);

	if (debug) {
		logs_serial = true;
		Serial.begin(9600);
		while (!Serial) {}
	} else {
		logs_serial = false;
	}
}

void logs_print(const char * string) {
	if (logs_rtc_Set) {
		String log_String = String(logs_rtc.getEpoch()) + ": " + string;

		if (logs_serial) {Serial.print(log_String);return;}

		logs_file_Handle.print(log_String);
		logs_file_Handle.flush();
	} else {
		if (logs_serial) {Serial.print(string); return;}
		logs_file_Handle.print(string);
		logs_file_Handle.flush();
	}
}

void logs_print(const String string) {
	if (logs_rtc_Set) {
		String log_String = String(logs_rtc.getEpoch()) + ": " + string;
		
		if (logs_serial) {Serial.print(log_String);return;}
		
		logs_file_Handle.print(log_String);
		logs_file_Handle.flush();
	} else {
		if (logs_serial) {Serial.print(string); return;}
		
		logs_file_Handle.print(string);
		logs_file_Handle.flush();
	}
}

void logs_println(const String string) {
	logs_print(string + "\n");
}

void logs_println(const char * string) {
	logs_print(String(string) + "\n");
}

void logs_deinitialize_Logs() {
	logs_file_Handle.flush();
	logs_file_Handle.close();
}

#endif
