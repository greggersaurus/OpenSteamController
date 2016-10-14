# Introduction

The purpose of this project is to explore, deconstruct and, hopefully, expand 
 upon configuration options for the Steam Controller. This is both a personal 
 project to learn more about USB and gain experience with Python as well as 
 something that can hopefully be shared with the community to provide the 
 ability to safely customize a Steam Controller in ways not currently supported
 through Steam.

# Resources and Influences

* [How to manually load firmware on NXP chip](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)
 * Used to load old firmware on controller to monitor update process via Steam
 * Will be used to load custom firmware on to read/write EEPROM if necessary?
* [Pilatomic Stream Controller Singer](https://gitlab.com/Pilatomic/SteamControllerSinger)
 * Great example of using the haptics to play music
 * Example of code written in C using libusb
 * Downside is that it takes over controller when playing custom music
* [Standalone Stream Controller Driver](https://github.com/ynsta/steamcontroller)
 * OK source for breaking down some USB commands
 * Goal of this project is to subvert Steam platform. I have no issues with Valve or Steam and I want to add configuration ability to controller, not subvert or reproduce (though some reproduction will be necessary) what is already there
* [Teardown of Steam Controller](https://www.ifixit.com/Teardown/Steam+Controller+Teardown/52578)
 * Picture shows processor is LPC11U37F/501
* [NXP LPC11U37FBD64 Documentation](http://www.nxp.com/products/microcontrollers-and-processors/arm-processors/lpc-cortex-m-mcus/lpc-cortex-m0-plus-m0/lpc1100-cortex-m0-plus-m0/128kb-flash-12kb-sram-lqfp64-package:LPC11U37FBD64?fpsp=1&tab=Documentation_Tab)
 * [Datasheet](http://www.nxp.com/documents/data_sheet/LPC11U3X.pdf?fasp=1&WT_TYPE=Data%20Sheets&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=pdf&WT_ASSET=Documentation&fileExt=.pdf)
 * [Secondary Bootloader App Note](http://www.nxp.com/documents/application_note/AN11732.zip?fasp=1&WT_TYPE=Application%20Notes&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=zip&WT_ASSET=Documentation&fileExt=.zip)
* [Steam Controller Update News](http://store.steampowered.com/news/?appids=353370)
 * Use to get an idea of what changed from firmware to firwmare release

# Goals/Requirements

* Written in Python
 * While I could produce cross platform code written in C using libusb, producing Python (that using pyusb) seems to be a simpler platform independent solution
 * I do not know much Python, so this will be a good learning experience

* Get controller info
 * Serial Number
  * Printed on back of controller 
  * Steam reports same number, but do not see it in traffic...
   * Possible it is now shown by usbmon as it only prints 32 of 64 bytes on control messages
   * TODO: capture 0x83 response and look at all data to see if its in there
 * Board Revision
  * In 0x83 repsonse
 * Firmware Revision
  * In 0x83 repsonse
 * Radio Firmware Revision
  * TODO: always zeros for Steam Controller. Not used?

* Play song (using haptics)
 * Instruct controller to play a particular song to demo what it sounds like
 * Reproduction of what Steam does when you select a song for startup or shutdown in config
 * Status: Implemented, but does not work on firmware before 5653a68a
 * TODO: add firmware check and report error if firmware if before 5653a68a

* Config startup and shutdown song 
 * Reproduction of what Steam does when you click confirm after viewing/changing controller config
 * Status: Captured data and analyzed
 * TODO: Implement in code

* Configure Steam Button Brightness
 * Status: Captured commands broke down represent brightness value
 * TODO: check if this works with firmware previous to 5653a68a

* Load firmware
 * Reproduction of what Stream does automatically when it detects out of date firmware, but this will allow for loading custom firmware or specifying which previous firmware specified in vdf should be installed
 * Status: Captured USB data 
 * TODO: Analyze USB data and implement function. Look for failure retry in captured data. 

* Custom song 
 * Assumption is that song files are coded into firmware or stored in EEPROM on NXP chip
  * Firmware assumption 
   * Start here and definitively determine if it is in code or not
   * TODO: Look at code changes from 5643849f to 5653a68a. Loading firmware older than 5653a68a results in system not playing song, but could just be different EEPROM setup?
   * TODO: How does Steam react to pre-5653a68a firmware? Saw something in Steam Controller updates about change to firmware and Steam handling this update. Does this mean there are USB commands to access EEPROM?
   * TODO: Reverse compile firmware (how is firmware laid out (mapping), look at ARM assembly instructions?)   
   * TODO: Create example project for chip using LPCXpresso and dig into compiler, etc.
   * TODO: Look into mention in updates of storing custom configs/mappings and how they travel with controller. Will this allow us access to what we want somehow?
  * EEPROM assumption
   * Can we dump EEPROM somehow?
    * TODO: see mention above above config/mappings traveling with controller. Does this allow us access?
    * TODO: write custom firmware that dump EEPROM over USB (is this realistic? how to set pins to not damage controller?)

# USB Data Capture

* Use lsusb to get bus and devnum
* Use [usbmon](https://www.kernel.org/doc/Documentation/usb/usbmon.txt)
 * cat /sys/kernel/debug/usb/usbmon/u0 
 * Pipe to grep to filter on data we care about (i.e. bus and devnum)
* Capture actions
 * ssh into Steam Box and cat usbmon file
 * Perform distinct actions and capture data
 * Isolate variables with slight variations on actions
