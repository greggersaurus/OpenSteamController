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
 Reverse Engineering subproject. See this project for details on how the
 firmware supported by Valve works, as well as details on the hardware. 

## Development Environment

The custom firmware for the LPC11U37 has been developed in the LPCXpresso IDE 
 (v8.2.2 Build 650 2016-09-09). The IDE can is available from www.nxp.com.
 A Free Edition license that should be adequate for further development is 
 available through NXP.

## [How to Manually Load Firmware](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)

* From powered down state, hold right trigger while connecting via USB
    * Mount "CRP DISABLD" will appear
    * Load new firmware binary with command "cat new_firmware.bin > /mount/CRP\ DISABLD/firmware.bin"
    * eject "CRP DISABLD"

# Building

Open the LPCXpresso IDE and import projects [SteamControllerDevKit](./SteamControllerDevKit) 
 and [lpc_chip_11uxx_lib](./lpc_chip_11uxx_lib).

A project can be imported into LPCXpresso by selecting: 
 File -> Import -> General -> Existing Project into Workspace -> Select root directory

Run Project -> Build All to compile.

# TODO

This is a running list of items I would like to prioritize and not lose track
 of.

1. Check clean build works
1. (finish) ADC command
    1. Have it use interrupt like Valve's firmware does
    1. Add safety checks, timeouts, etc.
    1. Read other channels (analog triggers? analog joystick?)
        1. See how sim results configure PIOs
    1. Polling mode where adc results are refreshed automatically until 'q' is pressed
1. Help command
1. READMEs for lpcexpresso projects
    1. Details on where lpc project came from
    1. Intention and requirements (i.e. lpc project) for SteamControllerDevKit project
1. Command for communication with haptics
    1. It seems SPI may be the interface
    1. [Sample Interface Code](https://github.com/cirque-corp/Cirque_Pinnacle_1CA027) to use as starting point?
1. Command for communication with gyro sensor
1. Command for monitoring controller state
    1. Button states
    1. Gyro info?
    1. Haptic sensor info (i.e. x,y touch location)?
1. Command for communications with radio chip?
1. Reorganize/rename project (i.e. CustomFirwmare) so #define can be used to build for various needs
    1. DEV_KIT where USB acts as UART console for controlling peripherals
    1. NIN_SWITCH where USB acts as Nintendo Switch wired controller
    1. PS4 where USB acts as Playstation 4 wired controller (if there is such a thing... Might need to be PS3?)
