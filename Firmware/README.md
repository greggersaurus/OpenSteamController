# Firmware

The work in this directory is geared towards a completely custom firmware for 
 the Steam Controller. The primary purpose is for this firmware to act as a 
 development environment for interfacing with the peripherals available on the 
 Steam Controller hardware. This firmware also acts as a jumping off point for 
 using the Steam Controller hardware for other purposes (i.e. to act as a wired
 controller for the Nintendo Switch).

See the [OpenSteamController](./OpenSteamController) Project for the source and
 additional details.


# Resources, Utilities and Influences

This section outlines tools, efforts, etc. that make the [OpenSteamController](./OpenSteamController)
 Firmware possible. 

## [lpc_chip_11uxx_lib](./lpc_chip_11uxx_lib)

This is an NXP provided library for easy access to LPC11U37 processor 
 peripherals, etc. This was obtained from NXP's website under the 
 lpcopen_v2_03_lpcxpresso_nxp_lpcxpresso_11u37h package. It also seems Valve is 
 leveraging this library in the Steam Controller's official firmware. 

## [Reverse Engineering](../ReverseEngineering/)

The custom firwmare in this Subproject is based on the Steam Controller 
 Reverse Engineering Subproject. See this project for details on how the
 firmware supported by Valve works, as well as details on the hardware. 

## [FauxControllers](./WipExplorations/FauxControllers)

The purpose of this project is a workspace to learn about how USB HIDs work, 
 specifically in the context of attempting to use the LPCXpresso11U37H 
 Evaluation board OM13074 to act as a known commercial controllers (i.e.
 Nintendo Switch Pro Controller).

The idea is to merge what is learned here into the OpenSteamController Project
 and close down this project when it no longer contains unique information.

## Development Environment

The custom firmware for the LPC11U37 has been developed in the LPCXpresso IDE 
 (v8.2.2 Build 650 2016-09-09). The IDE can is available from www.nxp.com.
 A Free Edition license that should be adequate for further development is 
 available through NXP.

## Loading Firmware

See the [Loading Firmware](../LoadingFirmware.md) document for details on how
 to backup the current firmware and load new firmware onto the controller.


# Building

Open the LPCXpresso IDE and import projects [OpenSteamController](./OpenSteamController) 
 and [lpc_chip_11uxx_lib](./lpc_chip_11uxx_lib).

A project can be imported into LPCXpresso by selecting: 
 File -> Import -> General -> Existing Project into Workspace -> Select root directory

Run Project -> Build All to compile.

## Firmware Configuration

There are two primary modes this firmware can be compiled with: DEV_BOARD_FW
 and SWITCH_WIRED_POWERA_FW. See OpenSteamController/inc/fw_cfg.h in order to 
 configure which behavior the firmware is built with. 

### DEV_BOARD_FW 

When FIRMWARE_BEHAVIOR is set to DEV_BOARD_FW the firmware that is built will
 turn the Steam Controller into a development board-like system. The USB
 interfaces acts as a serial device, which presents a command line interface
 for interrogating different peripherals on the Steam Controller. 

If you connect the Steam Controller via USB to a PC once this firmware is loaded
 you should see a serial device appear (i.e. in OS X you will see something
 like /dev/tty.usbmodem1781, in Windows you will see something like "USB Serial
 Device (COM3)"). Using any number of programs with
 serial communication capabilities (i.e. screen, TeraTerm, putty) should allow
 you to access the command line interface (i.e. screen /dev/tty.usbmodem1781).
 Once you are connected the 'help' command is a great place to start to see
 what you can check out on the controller. 

The intention of this build configuration is to learn more about the 
 peripherals and interfaces on the Steam Controller. This also provides a 
 platform to monitor controller states to verify or feed data back into
 Reverse Engineering simulations to observe different behaviors. 

### SWITCH_WIRED_POWERA_FW 

When FIRMWARE_BEHAVIOR is set to SWITCH_WIRED_POWERA_FW the firmware that is
 built will cause the Steam Controller to behave as a wired Nintendo Switch
 controller. Specifically the controller is pretending to be a Wired Nintendo
 Switch controller by PowerA. 

If you load this firmware onto a Steam Controller and then connect the controller
 to the Nintendo Switch via a USB cable you can use the controller with
 your Switch. Have fun, but note this was created for educational purposes
 and probably will not perform as well as an officially licensed Nintendo
 product. 


# TODO

See [TODO](./TODO.md) for details.
