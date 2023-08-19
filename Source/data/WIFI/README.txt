There are two types of options. Ones that are enabled and disabled by the
presence of a file. These are empty. The ones that aren't are just a value
represented by the filename (key). SPIFFS does not support directories, only
files. So, we prepend the folder name before the filename for organizational
purposes. All files in folders will be flattened into one directory.

Presence Enabled:
COMMUN_WPA2 - Communication network uses WPA2
COMMUN_WPA2_PSK - Communication network uses WPA2 with password
COMMUN_WPA2_ENTERPRS - Communication network uses WPA2 Enterprise

Contains Data:
COMMUN_SSID - Contains the network SSID of the communication
network
COMMUN_PASSWORD - Wireless password for commnunication network
COMMUN_USERNAME - Username for Wireless if using WPA2 Enterprise

BUS_SSID - Contains the network SSID of the bus
