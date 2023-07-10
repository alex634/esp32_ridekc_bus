There are two types of options. Ones that are enabled and disabled by the
presence of a file. These are empty. The ones that aren't are just a value
represented by the filename (key). SPIFFS does not support directories, only
files. So, we prepend the folder name before the filename for organizational
purposes. All files in folders will be flattened into one directory.

Presence Enabled:
WIFI_COMMUN_WPA2 - Communication network uses WPA2
WIFI_COMMUN_WPA2_PSK - Communication network uses WPA2 with password
WIFI_COMMUN_WPA2_ENTERPRS - Communication network uses WPA2 Enterprise

WIFI_DBG_COMMUN_WPA2 - Testing device acting as the communication
network uses WPA2
WIFI_DBG_COMMUN_WPA2_PSK - Testing device acting as communication network
uses WPA2 with password
WIFI_DBG_COMMUN_WPA2_ENTERPRS - Testing communication network uses WPA2 Enterprise

Contains Data:
WIFI_COMMUN_SSID - Contains the network SSID of the communication
network
WIFI_COMMUN_PASSWORD - Wireless password for commnunication network
WIFI_COMMUN_USERNAME - Username for Wireless if using WPA2 Enterprise

WIFI_DBG_COMMUN_SSID - Contains the network SSID of the testing device acting
as the communication network
WIFI_DBG_COMMUN_PASSWORD - Contains the password for the testing device acting
as the communication network
WIFI_DBG_COMMUN_USERNAME - Contains the username for wireless if using WPA2
Enterprise

WIFI_DBG_BUS_SSID - Contains the network SSID of the test bus network
WIFI_BUS_SSID - Contains the network SSID of the bus
