#ifndef WIFI_HPP
#define WIFI_HPP

#include "./logs.hpp"
#include <esp_wpa2.h>
#include <WiFi.h>

enum wifi_Connection_Type {
	WPA2_PSK,
	WPA2_NO_PSK,
	WPA2_ENTERPRISE
};

struct wifi_WPA2_Enterprise_Secrets {
	char * ssid;
	char * username;
	char * password;
};

struct wifi_WPA2_PSK_Secrets {
	char * ssid;
	char * password;
};

struct wifi_WPA2_Secrets {
	char * ssid;
};

struct wifi_Connection_Request {
	enum wifi_Connection_Type c_Type;
	void * secrets;
};

void wifi_disconnect_From_SSID() {
        WiFi.disconnect();
       	logs_println("wifi_disconnect_From_SSID(): Disconnected from SSID");
}

void wifi_connect_To_SSID(struct wifi_Connection_Request request) {
	wifi_disconnect_From_SSID();

	while (WiFi.status() != WL_CONNECTED) {
		switch (request.c_Type) {
			case WPA2_NO_PSK:
				logs_print("wifi_connect_To_SSID(): Attempting WPA2 connection to: ");
				logs_println(((struct wifi_WPA2_Secrets *) request.secrets)->ssid);

				WiFi.begin(( (struct wifi_WPA2_Secrets *) request.secrets)->ssid);
			break;
			
			case WPA2_PSK:
				logs_print("wifi_connect_To_SSID(): Attempting WPA2 connection to: ");
				logs_println(((struct wifi_WPA2_PSK_Secrets *) request.secrets)->ssid);
				
				WiFi.begin( ( (struct wifi_WPA2_PSK_Secrets *) request.secrets)->ssid, ( (struct wifi_WPA2_PSK_Secrets *) request.secrets)->password );
			break;
			
			case WPA2_ENTERPRISE:
				esp_wifi_sta_wpa2_ent_set_identity( (unsigned char *) ( (struct wifi_WPA2_Enterprise_Secrets *) request.secrets)->username, strlen(( (struct wifi_WPA2_Enterprise_Secrets *) request.secrets)->username));
				esp_wifi_sta_wpa2_ent_set_username( (unsigned char *) ( (struct wifi_WPA2_Enterprise_Secrets *) request.secrets)->username, strlen(( (struct wifi_WPA2_Enterprise_Secrets *) request.secrets)->username));
				esp_wifi_sta_wpa2_ent_set_password( (unsigned char *) ( (struct wifi_WPA2_Enterprise_Secrets *) request.secrets)->password, strlen(( (struct wifi_WPA2_Enterprise_Secrets *) request.secrets)->password));
				esp_wifi_sta_wpa2_ent_enable();
				WiFi.begin(((struct wifi_WPA2_Enterprise_Secrets *) (request.secrets))->ssid );

				logs_print("wifi_connect_To_SSID(): Attempting WPA2 Enterprise connection to: ");
				logs_println(((struct wifi_WPA2_Enterprise_Secrets *) (request.secrets))->ssid);
			break;
		}
		delay(10000);
	}
	

	logs_println("wifi_connect_To_SSID(): Connection established");
}

void wifi_enable_Disable_Wifi(bool enable) {
        if (enable) {
                logs_println("wifi_enable_Disable_Wifi(): WiFi entering station mode");
                WiFi.mode(WIFI_AP_STA);
        } else {
                logs_println("wifi_enable_Disable_Wifi(): WiFi entering off mode");
                WiFi.mode(WIFI_OFF);
        }
}

bool wifi_is_SSID_Present(const String ssid) {
        const int count = WiFi.scanNetworks();

        if (count == -1) {
                logs_println("wifi_is_SSID_Present(): Scanning error");
        }

        if (count == -1) return false;

        for (int i = 0; i < count; i++) {
                String current_SSID = WiFi.SSID(i);
                if (ssid.equals(current_SSID)) {
                        logs_print("wifi_is_SSID_Present(): Found SSID: ");
                        logs_println(current_SSID);
                        return true;
                }

                else {
                        logs_print("wifi_is_SSID_Present(): Checking SSID:  ");
                        logs_println(current_SSID);
                }
        }

        logs_println("wifi_is_SSID_Present(): Could not find SSID");

        return false;
}

void wifi_connect_To_Debug_Wifi() {
	struct wifi_Connection_Request connection_Request;
	
	if (fs_vars_get_Enabled("WIFI", "DBG_COMMUN_WPA2")) {
		struct wifi_WPA2_Secrets secrets;

		logs_println("wifi_connect_To_Debug_Wifi(): Settings indicate WPA2 without PSK");
		connection_Request.c_Type = WPA2_NO_PSK;
		
		logs_println("wifi_connect_To_Debug_Wifi(): Retrieving ssid");
		secrets.ssid = fs_vars_get_Contents("WIFI", "DBG_COMMUN_SSID", true);

		connection_Request.secrets = (void *) &secrets;

		wifi_connect_To_SSID(connection_Request);
		delete secrets.ssid;

	} else if (fs_vars_get_Enabled("WIFI", "DBG_COMMUN_WPA2_PSK")) {
		struct wifi_WPA2_PSK_Secrets secrets;

		logs_println("wifi_connect_To_Debug_Wifi(): Settings indicate WPA2 with PSK");
		connection_Request.c_Type = WPA2_PSK;

		logs_println("wifi_connect_To_Debug_Wifi(): Retrieving password");
		secrets.ssid = fs_vars_get_Contents("WIFI", "DBG_COMMUN_SSID", true);

		logs_println("wifi_connect_To_Debug_Wifi(): Retrieving password");
		secrets.password = fs_vars_get_Contents("WIFI", "DBG_COMMUN_PASSWORD", true);

		connection_Request.secrets = (void *) &secrets;

		wifi_connect_To_SSID(connection_Request);
		delete secrets.ssid;
		delete secrets.password;


	} else if (fs_vars_get_Enabled("WIFI", "DBG_COMMUN_WPA2_ENTERPRS")) {
		struct wifi_WPA2_Enterprise_Secrets secrets;
		
		logs_println("wifi_connect_To_Debug_Wifi(): Settings indicate WPA2 Enterprise");
		connection_Request.c_Type = WPA2_ENTERPRISE;

		logs_println("wifi_connect_To_Debug_Wifi(): Retrieving ssid");
		secrets.ssid = fs_vars_get_Contents("WIFI", "DBG_COMMUN_SSID", true);
		
		logs_println("wifi_connect_To_Debug_Wifi(): Retrieving password");
		secrets.password = fs_vars_get_Contents("WIFI", "DBG_COMMUN_PASSWORD", true);

		logs_println("wifi_connect_To_Debug_Wifi(): Retrieving username");
		secrets.username = fs_vars_get_Contents("WIFI", "DBG_COMMUN_USERNAME", true);

		wifi_connect_To_SSID(connection_Request);
		delete secrets.ssid;
		delete secrets.password;
		delete secrets.username;
	}
}

void wifi_connect_To_Normal_Wifi() {
	struct wifi_Connection_Request connection_Request;
	
	if (fs_vars_get_Enabled("WIFI", "COMMUN_WPA2")) {
		struct wifi_WPA2_Secrets secrets;

		logs_println("wifi_connect_To_Normal_Wifi(): Settings indicate WPA2 without PSK");
		connection_Request.c_Type = WPA2_NO_PSK;
		
		logs_println("wifi_connect_To_Normal_Wifi(): Retrieving ssid");
		secrets.ssid = fs_vars_get_Contents("WIFI", "COMMUN_SSID", true);

		connection_Request.secrets = (void *) &secrets;

		wifi_connect_To_SSID(connection_Request);
		delete secrets.ssid;

	} else if (fs_vars_get_Enabled("WIFI", "COMMUN_WPA2_PSK")) {
		struct wifi_WPA2_PSK_Secrets secrets;

		logs_println("wifi_connect_To_Normal_Wifi(): Settings indicate WPA2 with PSK");
		connection_Request.c_Type = WPA2_PSK;

		logs_println("wifi_connect_To_Normal_Wifi(): Retrieving password");
		secrets.ssid = fs_vars_get_Contents("WIFI", "COMMUN_SSID", true);

		logs_println("wifi_connect_To_Normal_Wifi(): Retrieving password");
		secrets.password = fs_vars_get_Contents("WIFI", "COMMUN_PASSWORD", true);

		connection_Request.secrets = (void *) &secrets;

		wifi_connect_To_SSID(connection_Request);
		delete secrets.ssid;
		delete secrets.password;


	} else if (fs_vars_get_Enabled("WIFI", "COMMUN_WPA2_ENTERPRS")) {
		struct wifi_WPA2_Enterprise_Secrets secrets;
		
		logs_println("wifi_connect_To_Normal_Wifi(): Settings indicate WPA2 Enterprise");
		connection_Request.c_Type = WPA2_ENTERPRISE;

		logs_println("wifi_connect_To_Normal_Wifi(): Retrieving ssid");
		secrets.ssid = fs_vars_get_Contents("WIFI", "COMMUN_SSID", true);
		
		logs_println("wifi_connect_To_Normal_Wifi(): Retrieving password");
		secrets.password = fs_vars_get_Contents("WIFI", "COMMUN_PASSWORD", true);

		logs_println("wifi_connect_To_Normal_Wifi(): Retrieving username");
		secrets.username = fs_vars_get_Contents("WIFI", "COMMUN_USERNAME", true);

		wifi_connect_To_SSID(connection_Request);
		delete secrets.ssid;
		delete secrets.password;
		delete secrets.username;
	}
}

bool wifi_is_Debug_Bus_Present() {
	char * debug_bus_SSID = fs_vars_get_Contents("WIFI", "DBG_BUS_SSID", true);
	bool present = wifi_is_SSID_Present(debug_bus_SSID);
	delete debug_bus_SSID;
	return present;
}

bool wifi_is_Normal_Bus_Present() {
	char * bus_SSID = fs_vars_get_Contents("WIFI", "BUS_SSID", true);
	bool present = wifi_is_SSID_Present(bus_SSID);
	delete bus_SSID;
	return present;
}

bool wifi_bus_Wifi_Present() {
	if (fs_vars_debug_Mode_Enabled()) {
		logs_println("wifi_bus_Wifi_Present(): Attempting to find debug bus wifi");
		return wifi_is_Debug_Bus_Present();
	}

	logs_println("wifi_bus_Wifi_Present(): Attempting to find normal bus wifi");
	return wifi_is_Normal_Bus_Present();
}

void wifi_connect_To_Communication_Wifi() {
	if (fs_vars_debug_Mode_Enabled()) {
		logs_println("wifi_connect_To_Communication_Wifi(): Attempting to connect to debug communication wifi");
		return wifi_connect_To_Debug_Wifi();
	}

	logs_println("wifi_connect_To_Communication_Wifi(): Attempting to connect to communication wifi");
	return wifi_connect_To_Normal_Wifi();
}


#endif
