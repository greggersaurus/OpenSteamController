# Firmware Binaries

This directory contains prebuilt binaries for the custom firmware related to 
 this project. I have decided to add this in order to lower the barrier to 
 entry for those who are curious about what the project can accomplish. 

Please note that these are here as examples and the hope is that this is a
 jumping off point to encourage you to (modify and) rebuild the firmware 
 yourself. If you have any issue rebuilding the firmware please make an issue 
 so that actions can be taken to alleviate any roadblocks or barriers. 

## [OpenSteamControllerNinSwitch.bin](./OpenSteamControllerNinSwitch.bin)

This is a build of the [Open Steam Controller Firmware](../OpenSteamController)
 where FIRMWARE_BEHAVIOR in [fw_cfg.h](../OpenSteamController/inc/fw_cfg.h) is 
 set to SWITCH_WIRED_POWERA_FW. After this is loaded onto the Steam Controller
 if the controller is connected to a Nintendo Switch via USB the controller
 will act as a wired Switch Controller. With this build the Left Trackpad
 acts as a DPAD and the Right Trackpad acts the Right Analog Stick. If you
 would like this to be different, modify function updateReports() in 
 [usb.c](../OpenSteamController/src/usb.c) and rebuild. See the 
 [Firmware README](../README.md) for further details.
 

## [OpenSteamControllerDevBoard.bin](./OpenSteamControllerDevBoard.bin)

This is a build of the [Open Steam Controller Firmware](../OpenSteamController)
 where FIRMWARE_BEHAVIOR in [fw_cfg.h](../OpenSteamController/inc/fw_cfg.h) is 
 set to DEV_BOARD_FW. After this is loaded onto the Steam Controller the 
 controller will function as a development board-like system. Connecting the
 controller to a computer via USB will produce a serial port interface for
 interrogating the controller via a command line interface. See the
 [Firmware README](../README.md) for further details.
