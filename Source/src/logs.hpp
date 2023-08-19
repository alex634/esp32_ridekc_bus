#ifndef LOGS_HPP
#define LOGS_HPP

#include <Arduino.h>
#include <esp_spiffs.h>
#include <FS.h>
#include <ESP32Time.h>
#include "./fs_vars.hpp"

static bool logs_rtc_Set = false;
static ESP32Time logs_rtc;

void logs_enable_Rtc(ESP32Time e_Time) {
	logs_rtc = e_Time;
	logs_rtc_Set = true;
}

void logs_initialize_Logs() {
	Serial.begin(9600);
}

void logs_print(const char * string) {
	if (logs_rtc_Set) {
		String log_String = String(logs_rtc.getEpoch()) + ": " + string;
		Serial.print(log_String);

	} else {
		Serial.print(string);
	}
}

void logs_print(const String string) {
	if (logs_rtc_Set) {
		String log_String = String(logs_rtc.getEpoch()) + ": " + string;
		
		Serial.print(log_String);
	} else {
		Serial.print(string);
	}
}

void logs_println(const String string) {
	logs_print(string + "\n");
}

void logs_println(const char * string) {
	logs_print(String(string) + "\n");
}

#endif
