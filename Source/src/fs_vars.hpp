#ifndef FS_VARS_HPP
#define FS_VARS_HPP

#include <SPIFFS.h>
#include <FS.h>
#include "./logs.hpp"

struct fs_vars_misc_Vars {
	int GMT_OFFSET_SEC;
	int LAST_BUS_HOUR;

	char * COMMUN_SERVER;
	int COMMUN_SERVER_PORT;
	
	char * TIME_REST_SERVER;
	char * TIME_REST_QUERY;
	
	int DEEP_SLEEP_HOURS;
	int MODEM_SLEEP_SEC;
};

struct fs_vars_misc_Vars fs_vars_m_Vars;

void fs_vars_mount_Filesystem() {
	SPIFFS.begin();
}

void fs_vars_unmount_Filesystem() {
}

String fs_vars_Filename(const char * var_Domain, const char * var_Name) {
	return String("/") + String(var_Domain) + String("/") + String(var_Name);
}

bool fs_vars_get_Enabled(const char * var_Domain, const char * var_Name) {
	String filename = fs_vars_Filename(var_Domain, var_Name);
	logs_println("fs_vars_get_Enabled(): Checking enabled: " + filename);
	return SPIFFS.exists(filename);
}

void fs_vars_set_Enabled(const char * var_Domain, const char * var_Name) {
	String filename = fs_vars_Filename(var_Domain, var_Name);
	logs_println("fs_vars_set_Enabled(): Setting enabled: " + filename);
	File file = SPIFFS.open(filename);

	file.close();
}

void fs_vars_set_Disabled(const char * var_Domain, const char * var_Name) {
	String filename = fs_vars_Filename(var_Domain, var_Name);
	logs_println("fs_vars_set_Disabled(): Setting disabled: " + filename);
	SPIFFS.remove(filename);
}


//Please free this memory when you are done
//This should never be used to open logs, this may consume a lot of heap memory
char * fs_vars_get_Contents(const char * var_Domain, const char * var_Name, bool add_Null_Terminator) {
	String filename = fs_vars_Filename(var_Domain, var_Name);

	File file = SPIFFS.open(filename);
	logs_println("fs_vars_get_Contents(): Opened file: " + filename);

	size_t file_Size = file.size();
	logs_println("fs_vars_get_Contents(): Allocating memory");

	uint8_t * buffer; 

	buffer = (add_Null_Terminator) ? new uint8_t[file_Size + 1]: new uint8_t[file_Size];

	logs_println("fs_vars_get_Contents(): Reading file into buffer");
	file.read(buffer, file_Size);

	if (add_Null_Terminator) buffer[file_Size] = (char) 0;
	
	file.close();
	return (char *) buffer;
}

void fs_vars_set_Contents(const char * var_Domain, const char * var_Name, char * var_Contents, size_t length) {
	String filename = fs_vars_Filename(var_Domain, var_Name);
	SPIFFS.remove(filename);
	File file = SPIFFS.open(filename);
	logs_println("fs_vars_set_Contents(): Opened file: " + filename);
	
	file.write((uint8_t *) var_Contents, length);
	logs_println("fs_vars_set_Contents(): Wrote contents");
	file.close();
	
	return;
}

void fs_vars_load_Misc_Vars() {
	logs_println("fs_vars_load_Misc_Vars(): Loading struct fs_vars_misc_Vars");
	
	char * gmt_Offset_Sec = fs_vars_get_Contents("MISC", "GMT_OFFSET_SEC", true);
	fs_vars_m_Vars.GMT_OFFSET_SEC = String(gmt_Offset_Sec).toInt();
	delete gmt_Offset_Sec;

	char * last_Bus_Hour = fs_vars_get_Contents("MISC", "LAST_BUS_HOUR", true);
	fs_vars_m_Vars.LAST_BUS_HOUR = String(last_Bus_Hour).toInt();
	delete last_Bus_Hour;

	fs_vars_m_Vars.COMMUN_SERVER = fs_vars_get_Contents("MISC", "COMMUN_SERVER", true);

	char * commun_Server_Port = fs_vars_get_Contents("MISC", "COMMUN_SERVER_PORT", true);
	fs_vars_m_Vars.COMMUN_SERVER_PORT = String(commun_Server_Port).toInt();
	delete commun_Server_Port;

	fs_vars_m_Vars.TIME_REST_SERVER = fs_vars_get_Contents("MISC", "TIME_REST_SERVER", true);
	fs_vars_m_Vars.TIME_REST_QUERY = fs_vars_get_Contents("MISC", "TIME_REST_QUERY", true);
	
	char * deep_Sleep_Hours = fs_vars_get_Contents("MISC", "DEEP_SLEEP_HOURS", true);
	fs_vars_m_Vars.DEEP_SLEEP_HOURS = String(deep_Sleep_Hours).toInt();
	delete deep_Sleep_Hours;

	char * modem_Sleep_Sec = fs_vars_get_Contents("MISC", "MODEM_SLEEP_SEC", true);
	fs_vars_m_Vars.MODEM_SLEEP_SEC = String(modem_Sleep_Sec).toInt();
	delete modem_Sleep_Sec;

	logs_println("fs_vars_load_Misc_Vars(): Finished loading struct");
}

void fs_vars_free_Misc_Vars() {
	logs_println("fs_vars_free_Misc_Vars(): Freeing struct fs_vars_misc_Vars");

	delete fs_vars_m_Vars.COMMUN_SERVER;
	delete fs_vars_m_Vars.TIME_REST_SERVER;
	delete fs_vars_m_Vars.TIME_REST_QUERY;

	logs_println("fs_vars_free_Misc_Vars(): Finished freeing struct");
}

#endif
