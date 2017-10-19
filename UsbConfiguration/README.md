# USB Configuration 

Steam offers the ability to change certain configuration options for a Steam
 Controller (i.e. change LED brightness, change jingle to one of a few 
 pre-defined options). This directory contains work for understanding, 
 reproducing and, maybe, expanding upon this functionality.

# Goals 

This section outlines specifics regarding what this project hopes to achieve.

##  Get Controller Information via USB

The following detail the various pieces of controller information that I want to retrieve via USB.

### Serial Number

* Printed on back of controller 
* Steam reports same number, but do not see it in traffic...
    * Possible it is now shown by usbmon as it only prints 32 of 64 bytes on control messages
    * TODO: capture 0x83 response and look at all data to see if its in there

### Board Revision

* In 0x83 repsonse

### Firmware Revision

* In 0x83 repsonse

### Radio Firmware Revision

* TODO: always zeros for Steam Controller. Not used?

## Play Song/Tune/Jingle Using Haptics via USB Request

* Instruct controller to play a particular song to demo what it sounds like
* Reproduction of what Steam does when you select a song for startup or shutdown in config
* Status: Implemented, but does not work on firmware before 5653a68a
* TODO: add firmware check and report error if firmware if before 5653a68a

## Config Startup and Shutdown Song via USB Request

* Reproduction of what Steam does when you click confirm after viewing/changing controller config
* Status: Captured data and analyzed
* TODO: Implement in code

## Configure Steam Button Brightness via USB Request

* Status: Captured commands broke down represent brightness value
* TODO: check if this works with firmware previous to 5653a68a

## Load Firmware via USB Request

* Reproduction of what Steam does automatically when it detects out of date firmware, but this will allow for loading custom firmware or specifying which previous firmware specified in vdf should be installed
* Status: Captured USB data 
* TODO: Analyze USB data and implement function. Look for failure retry in captured data. 

# USB Data Capture

This sections outlines how USB commands are captured for analysis.

* Use lsusb to get bus and devnum
* Use [usbmon](https://www.kernel.org/doc/Documentation/usb/usbmon.txt)
    * cat /sys/kernel/debug/usb/usbmon/u0 
    * Pipe to grep to filter on data we care about (i.e. bus and devnum)
* Capture actions
    * ssh into Steam Box and cat usbmon file
    * Perform distinct actions and capture data
    * Isolate variables with slight variations on actions
