# ESP32_RIDEKC_BUS

## What does this do

This code allows for someone to determine when a bus is in close proximity to a ESP32. The ESP32 waits until it sees a bus Wifi hotspot and then connects to another Wifi nearby (in my case, a university network). It then issues a PUT request to a server to upload a file with the current UNIX time as the name of the file and "BUS ARRIVED..." as the content. This PUT request serves to indicate when a bus arrives.

## How to Use it

### Flashing Code / Editing Source

The documentation for the following is located here: [Documentation](FLASHING_CODE_README.md)

### Filesystem Variables

The ESP32 reads files on its onboard flash to determine which networks to connect to amongst other things. There will be more documentation released soon on how to set these variables. For now, you will have to use the README files in "Source/data/WIFI" and "Source/data/MISC" to determine how to set these correctly. Keep in mind that most text editors add newlines to the end. You will need to make sure to remove these with `truncate -s -1 <filename>` if your text editor does this.
