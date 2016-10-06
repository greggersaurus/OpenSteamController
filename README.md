# Introduction

The purpose of this project is to explore, deconstruct and, hopefully, expand 
 upon configuration options for the Steam Controller. This is both a personal 
 project to learn more about USB and gain experience with Python as well as 
 something that can hopefully be shared with the community to provide the 
 ability to safely customize a Steam Controller in ways not currenrtly supported
 through Steam.

# Resources/Influences

* [How to manually load firmware on NXP chip](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)
 * Used to load old firmware on controller to monitor update process via Steam
 * Will be used to load custom firmware on to read/write EEPROM?
* [Pilatomic Stream Controller Singer](https://gitlab.com/Pilatomic/SteamControllerSinger)
 * Great example of using the haptics to play music
 * Example of code written in C using libusb
 * Downside is that it must take over controller when playing custom music
* [Standalone Stream Controller Driver](https://github.com/ynsta/steamcontroller)
 * OK source for breaking down some USB commands
 * Goal of this project is to subvert Steam Platform. I have no issues with Valve or Steam and I want to add configuration ability to controller, not subvert or reproduce (though some reproduction will be necessary) what is already there
* [NXP LPC11U37FBD64 Documentation](http://www.nxp.com/products/microcontrollers-and-processors/arm-processors/lpc-cortex-m-mcus/lpc-cortex-m0-plus-m0/lpc1100-cortex-m0-plus-m0/128kb-flash-12kb-sram-lqfp64-package:LPC11U37FBD64?fpsp=1&tab=Documentation_Tab)
 * [Datasheet](http://www.nxp.com/documents/data_sheet/LPC11U3X.pdf?fasp=1&WT_TYPE=Data%20Sheets&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=pdf&WT_ASSET=Documentation&fileExt=.pdf)
 * [Secondary Bootloader App Note](http://www.nxp.com/documents/application_note/AN11732.zip?fasp=1&WT_TYPE=Application%20Notes&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=zip&WT_ASSET=Documentation&fileExt=.zip)

# Goals/Requirements

* Written in Python
 * While I could produce cross platform code written in C using libusb, producing Python (that using pyusb) seems to be a simpler platform independent solution
 * I do not know much Python, so this will be a good learning experience
* Play tune
 * Instruct controller to play a particular tune to demo what it sounds like
 * Reproduction of what Steam does when you select a tune for startup or shutdown in config
* Config startup and shutdown tune
 * Reproduction of what Stream does when you click confirm after viewing/changing controller config
* Load firmware
 * Reproduction of what Stream does automatically when it detects out of date firmware, but this will allow for loading custom firmware or specifying which previous firmware specified in vdf should be installed
* Custom tune
 * Assumption is that tune files are stored in EEPROM on NXP chip
  * Possible they have been baked into firmware. Look at different versions of firmware and see if tunes ever change? How else to eliminate this option?
 * How to access EEPROM?
  * USB command? Monitor firmware load to see how this is done. If this, or another command, allows direct memory access we may have a way in.
  * Complete custom firmware code
    * Allows for reading and writing EEPROM via USB commands.
    * This will be time consuming to develop, especially with limited debug capabilities
  * Inject new functionality into existing firmware.
    * Reverse compile firmware?
    * Add custom code into FFFF section of firmware and modify code to jump to these instructions?  (Maybe too unrealistic?)

# USB Sniffing

* Use usbmon
 * cat /sys//kernel/debug file
* compare files with only slightly different actions
