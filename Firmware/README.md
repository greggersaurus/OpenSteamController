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

# TODO

This is a running list of items I would like to prioritize and not lose track
 of.

1. Get trackpad functionality working
    1. Valve's firmware seems to be using the Trackpads in AnyMeas mode
        1. See https://github.com/cirque-corp/Cirque_Pinnacle_1CA027/blob/master/Additional_Examples/AnyMeas_Example/AnyMeas_Example.ino for additional details
    1. In this way the controller gets the raw ADC data and can do what it wants with it
    1. What do we want to do...?
        1. Let's see what AnyMeas data looks like and if we can understand it
        1. Maybe this is necessary, given how poorly the default performance seemed to work... (or maybe there was something else going on related to bad performance in default mode?)
        1. Easiest approach may be to replicate what Valve does as accurately as possible (now that we understand AnyMeas mode a little more) and (try to) understand and use that data
        1. Biggest risk/disconnect, is not understanding what each "ADC" relates to and how to use those values...
            1. Hopefully DEV_KIT build will help with understanding this...
1. Revisist console/CDC uart
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
1. Dig into inaccuracy in haptic output
    1. Use violin tuner app and notice how the higher the frequencey the larger the gap in frequency output and what app measures...
1. Dig into infinite pulse on haptic bug
    1. There was a bug where Note was pointing to bogus data that resulted in a interrupt that never seemed to end
        1. Need bounds check on how interrupt variables are setup to stop this from happening???
1. Make sure all functions have usage 
1. Get controller to act as PowerA wired
    1. Clean up (i.e. naming changes...)
1. Add details on different firmware build types (i.e. intentions and commands/control layouts)
1. Add details for each specific firwmare build type to this README
1. Check clean build works (i.e. from fresh clone)
1. Revisit and clean init()
    1. What GPIOs, etc. do we still not understand?
1. Make template or something for communicating with Radio chip
    1. via UART?
    1. Mostly just a reference to RevEng efforts to get more data?
1. Update led command to allow LED to blink?
    1. Use another counter?
1. Look into having USB UART CDC still be active for controller build... (This might not be possible. Need to learn more about USB in general I think...)
    1. This could be an awesome debug option to be able plug controller into PC to get stats, etc. via UART after usage...
1. READMEs for lpcexpresso projects
    1. Details on where lpc project came from
    1. Intention and requirements (i.e. lpc project) for OpenSteamController project
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
