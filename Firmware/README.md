# Firmware

The work in this directory is geared towards a completely custom firwmare for 
 the main LPC11U37 chip that will act a development environment for interfacing
 with the peripherals available on the Steam Controller hardware. This firmware
 also acts as a jumping off point for using the Steam Controller hardware for
 other purposes (i.e. to act as a wired controller for the Nintendo Switch).


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

## [FauxControllers](./FauxControllers)

The purpose of this project is a workspace to learn about how USB HIDs work, 
 specifically in the context of attempting to use the LPCXpresso11U37H 
 Evaluation board OM13074 to act as a known commercial controllers (i.e.
 Nintendo Switch Pro Controller).

The idea is to merge what is learned here into the OpenSteamController Project
 and close down this project when it is no longer useful.

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

Open the LPCXpresso IDE and import projects [OpenSteamController](./OpenSteamController) 
 and [lpc_chip_11uxx_lib](./lpc_chip_11uxx_lib).

A project can be imported into LPCXpresso by selecting: 
 File -> Import -> General -> Existing Project into Workspace -> Select root directory

Run Project -> Build All to compile.

## Firmware Configuration

There are two primary modes this firmware can be compiled with: DEV_BOARD_FW
 and SWITCH_WIRED_POWERA_FW. See OpenSteamController/inc/fw_cfg.h for further 
 details and to configure which behavior the firmware builds with. 


# TODO

This is a running list of items I would like to prioritize and not lose track
 of.

1. Faux Switch Controller
    1. Clean up (i.e. naming changes...)
    1. Add startup jingle...?



1. Building
    1. Test Release build and move away from working with Debug
        1. Debug isn't buying us anything on this platform anyways, right?
    1. Better build solution?
        1. Docker?
        1. Try with latest tools from NXP?
1. Documentation
    1. READMEs for lpcexpresso projects
        1. Details on where lpc project came from
        1. Intention and requirements (i.e. lpc project) for OpenSteamController project
    1. Make sure all functions have usage 
1. jingle_data.c
    1. Needs more support functions for adding new Jingles, notes, etc.
1. haptic.c
    1. Check range on input arguments
    1. playHaptic() should copy data?
    1. Dig into inaccuracy in haptic output
        1. Use violin tuner app and notice how the higher the frequencey the larger the gap in frequency output and what app measures...
    1. Dig into infinite pulse on haptic bug
        1. There was a bug where Note was pointing to bogus data that resulted in a interrupt that never seemed to end
            1. Need bounds check on how interrupt variables are setup to stop this from happening???
1. init.c
    1. Verify values for OscRateIn and ExtRateIn
    1. Evaluate if busy wait loop in stage1Init can/should be changed
    1. Use #defines for GPIOs?
1. led_ctrl.c
    1. Command to allow LED to blink
        1. Use another counter to make interrupt driven?
    1. Revisit error function...
1. usb.c
    1. Make regression tests (i.e. testPrint)
        1. Buffer overflow related
        1. __disable_irq() related (i.e. ADC IRQs causing double prints or data loss...)
    1. Try to fix CDC UART overflow issues (can we without rewriting USB ROM code though...?)
        1. The issues stems from the fact that USB ROM code ACKs data packets before calling callbacks
            1. This means that in callback if we don't have room, we cannot tell the host to wait
            1. Other work arounds have not worked yet (i.e. disabling EP when buffer is full and reenabling later)
            1. Maybe we can hijack IRQ handler??
            1. Or maybe we look into not using ROM code and writing out own (see polling example fix for this issue: http://www.eevblog.com/forum/microcontrollers/usb-cdc-_flow-control_/)
        1. Reproduce issues by pasting entire jingle into console no longer locks it up, but it looks like some command get mangled...
1. test.c
    1. Clean up and add more tests
    1. Create (at least manual) test procedure?
1. motion.c
    1. Add support for interfacing with MPU-6500 Six-Axis (Gyro + Accelerometer)
1. radio.c
    1. Add support for interfacing with nRF51822 Radio Chip
1. trackpad.c
    1. Add ability to sample ADCs in low power mode
1. eeprom_access.c
    1. Implement writing function
1. mem_access.c
    1. Implement write command
1. command.c
    1. Replace linear search with something more efficient?
