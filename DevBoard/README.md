# Development Board

The work in this directory is geared towards a completely custom firwmare for 
 the main LPC11U37 chip that will act a development environment for interfacing
 with the peripherals available on the Steam Controller hardware. 

The goal of this subproject is to use the Steam Controller as a development board, 
 rather than to create a replacement for the firmware that causes the controller 
 to work like a controller that interfaces with Steam.

# Resources, Utilities and Influences

## [lpc_chip_11uxx_lib](./lpc_chip_11uxx_lib)

NXP provided library for easy access to LPC11U37 processor peripherals, etc.

## [Reverse Engineering](../ReverseEngineering/)

The custom firwmare in this subproject is based on the Steam Controller 
 reverse engineering subproject. See this project for details on how the
 firmware supported by Valve works, as well as details on the hardware. 

## Development Environment

The custom firmware for the LPC11U37 has been developed in the LPCXpresso IDE 
 (v8.2.2 Build 650 2016-09-09). The IDE can is available from www.nxp.com.
 A Free Edition license that should be adequate for further development is 
 available through NXP.

## [How to Manually Load Firmware](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)

* Hold right trigger while connecting via USB
    * Mount "CRP DISABLD" will appear
    * cat new_firmware.bin > /mount/CRP\ DISABLD/firmware.bin
    * eject CRP DISABLD

# Building

Open LPCXpresso and import projects [SteamControllerDevKit](./SteamControllerDevKit) 
 and [lpc_chip_11uxx_lib](./lpc_chip_11uxx_lib).

A project can be imported into LPCXpresso by selecting: 
 File -> Import -> Existing Project into Workspace -> Select root directory

Run Project -> Build All to compile.
