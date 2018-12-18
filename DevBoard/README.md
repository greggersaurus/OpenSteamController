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
 This was obtained from NXP's website under the 
 lpcopen_v2_03_lpcxpresso_nxp_lpcxpresso_11u37h package. It also seems Valve is 
 leveraging this library in the Steam Controller's official firmware. 

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

* From powered down state, hold the right trigger while connecting the controller via USB to a PC
    * This will activate the USB In-System Programming for the LPC11U37F
* The LPC11U37F will act as a FAT12 file system labeled "CRP DISABLD"
    * The file system will have a single file called "firmware.bin"
* The update process may be slightly different depending on your operating system 
    * On OSX (Tested specifically on 10.12.6):
        * Mount "CRP DISABLD" will appear
        * Load new firmware binary with command "cat new_firmware.bin > /Volumes/CRP\ DISABLD/firmware.bin"
        * eject "CRP DISABLD"
    * On Linux systems (As reported by @rigidsh):
        * If you will use  "cat new_firmware.bin > /mount/CRP\ DISABLD/firmware.bin" on ubuntu(or similar linux) firmware will be corrupted and controller will not start.
        * To prevent it, use command: `dd conv=nocreat,notrunc oflag=direct bs=512 if=<path to your firmware> of=<path to sc flash>/firmware.bin`
    * On Windows (Untested. See remark in Section 3 of AN11305v1 from NXP for source):
        * "Any standard program or tool can be used to write new firmware to the LPC11U37." 
        * "In a Windows Explorer window, a user can delete firmware.bin and drag over a new file to program the flash."

# Building

Open the LPCXpresso IDE and import projects [SteamControllerDevKit](./SteamControllerDevKit) 
 and [lpc_chip_11uxx_lib](./lpc_chip_11uxx_lib).

A project can be imported into LPCXpresso by selecting: 
 File -> Import -> General -> Existing Project into Workspace -> Select root directory

Run Project -> Build All to compile.

# TODO

This is a running list of items I would like to prioritize and not lose track
 of.

1. Create sleep function
    1. Use CT16B0 like official Valve FW does?
1. Now that we know PIO1_1 is active low enable of analog triggers (L = AD0 and R = AD2), clean up and organize!
1. Get controller to act as PowerA wired
    1. Clean up naming in changes...
1. Add details on different firmware build types (i.e. intentions and commands/control layouts)
1. Add details for each specific firwmare build type to this README
1. Figure out interrupt related issues as to why Console output gets weird
    1. Is this related to WFI() in main loop?
    1. Is this related to ADC IRQ always running?
    1. Is this related to monitor command lock up?
1. Check clean build works (i.e. from fresh clone)
1. Look into having USB UART CDC still be active for controller build... (This might not be possible. Need to learn more about USB in general I think...)
    1. This could be an awesome debug option to be able plug controller into PC to get stats, etc. via UART after usage...
1. (finish) ADC command
    1. Have it use interrupt like Valve's firmware does
        1. Make sure IRQ is not starving anything running in normal execution context (i.e. not IRQ)
    1. Add safety checks, timeouts, etc.
    1. Read other channels (analog triggers? analog joystick?)
        1. See how sim results configure PIOs
    1. Polling mode where adc results are refreshed automatically until 'q' is pressed
    1. Better comments
1. Help command
1. READMEs for lpcexpresso projects
    1. Details on where lpc project came from
    1. Intention and requirements (i.e. lpc project) for SteamControllerDevKit project
1. Command for communication with haptics
    1. It seems SPI may be the interface
    1. [Sample Interface Code](https://github.com/cirque-corp/Cirque_Pinnacle_1CA027) to use as starting point?
1. Command for communication with gyro sensor
1. Command for monitoring controller state
    1. Gyro info?
    1. Haptic sensor info (i.e. x,y touch location)?
1. Command for communications with radio chip?
1. Reorganize/rename project (i.e. CustomFirwmare) so #define can be used to build for various needs
    1. DEV_KIT where USB acts as UART console for controlling peripherals
    1. NIN_SWITCH where USB acts as Nintendo Switch wired controller
    1. PS4 where USB acts as Playstation 4 wired controller (if there is such a thing... Might need to be PS3?)
1. All TODOs in code to have items in this README?
