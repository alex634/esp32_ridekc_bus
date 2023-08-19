# Creating a Development Environment on Ubuntu 20.04

## Preface

I personally use a Gentoo machine but figured I should make instructions that would apply to most. I downloaded an Ubuntu 22.04 image and created a clean install of Ubuntu. I forwarded the ESP32's USB port to the VM. These instructions should work fine, but keep this in mind if any issues do arise.

## Prerequisite Programs

### Anaconda

Go to [https://www.anaconda.com](https://www.anaconda.com) and click the "Download" button. Make sure that you aren't spoofing your useragent. It downloads the correct version for your system based on the useragent.

Next, open a terminal. Change your current directory to your Downloads folder `cd ~/Downloads`. Then give permissions to allow for your user to execute the shell script that was downloaded `chmod u+x ./Ananconda*`. Finally, execute it `./Anaconda*`.

When you execute the script, it will make you read a license. To scroll through it, press enter, but be weary when you get towards the end of the license. If you hold enter to scroll though it, you may accidentally say no to the license terms because the default response when you press enter to the license prompt is no. It will then prompt where to install Anaconda. Most can probably install it to the default location. When it asks to initialize conda, say yes.

Once finished, close your terminal and open a new one.

### PlatformIO

First, create a new conda environment `conda create -n pio pip`. Then, activate that new conda environment `conda activate pio`. Then, install platformio for within the new conda environment `pip install platformio`. 

Now, everything should be installed correctly. Any time you need to use platformio, make sure that you activate the environment `conda activate pio`.

### Git

This is necessary to get the source. To install Git is easy `sudo apt-get install git`.

### Lsusb (Optional)

In some cases, you may want to determine if your device has been detected correctly. Most if not all ESP32 devices should be detected as USB to serial devices. On Ubuntu, you can use `lsusb` to list all connected devices. In my case, my ESP32 shows up as "Bus 001: Device 002: ID 10c4:ea60 Silicon Labs CP210x UART Bridge". If your device is not shown when you plug it in, it is likely not a driver related issue. If you do see the device, and you cannot find the device file associated with the serial connection (ex. /dev/ttyUSB0), then you probably do have a driver issue.


# Getting and Using the Source

### Getting Sources

Go to a directory where you want to store the source and run: `git clone https://github.com/alex634/esp32_ridekc_bus.git`. Once it is cloned, go into the directory: `cd esp32_ridekc_bus`. Then enter the source directory: `cd Source`. If you list all files within the current directory `ls`, then you should see a file named platformio.ini. Open it and make sure the board is selected correctly for your development board.

### Flashing the Code

Make sure that the platformio conda environment is enabled: `conda activate pio`. Then, build the filesystem: `pio run -t buildfs`. This may install a few programs and libraries if this is the first time running. Then, next, build the program itself: `pio run`. To upload the filesystem, run: `pio run -t uploadfs`. If this fails, you may need to add yourself to the 'dialout' group: `sudo usermod -a -G dialout <your_username>`. If you have to add yourself to the dialout group, you will need to login again, reactivate the environment, and cd back into the "Source" directory. Once you have uploaded the filesystem, you can now upload the code too: `pio run -t upload`.

### Editing Source Code

Inside of the "Source" directory there is a directory named "src". This contains all code for the ESP32. The "data" directory is more likely to be of importance to someone wanting to just use the existing code. There are settings and README files that contain information on variables that can be set within this directory. You need to make sure that you run `truncate -s -1 <file_variable>` on those that aren't empty because by default many text editors add a newline at the end. If there is a newline at the end of a password (for example) it will be interpreted to be a part of the password. More documentation on the variables themselves will be released later.
