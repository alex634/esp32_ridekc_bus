# ESP32_RIDEKC_BUS

## Preface

The RIDEKC bus system does not provide people with a way to track the busses, or at least a working way. This would not be an issue if the busses were consistent, but unfortunately they are not. 

## Solution

To track the busses I came up with the solution of using an ESP32 to detect when the RIDEKC free bus WiFi is nearby, and then sending a PUT request to a server to create a file containing information about board status and arrival time.

The ESP32 will be enclosed in some sort of case with a source of power. This case will be designed specifically for where we place it and will be included here

To compile this code, you will need to use platformio. I used conda and pip to install the cli version, but you can use the VS Code plugin as well.
