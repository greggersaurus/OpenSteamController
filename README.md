# Introduction

The purpose of this project is to explore, deconstruct and, hopefully, expand 
 upon configuration options for the Steam Controller. This is both a personal 
 project to learn more about USB and gain experience with Python as well as 
 something that can hopefully be shared with the community to provide the 
 ability to safely customize a Steam Controller in ways not currently supported
 through Steam.

# Goals and Requirements

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

## Customize Song/Tune/Jingle

Assumption is that jingle data is coded into firmware or stored in EEPROM on NXP chip and I can come up with a way to modify this data into user customizable jingles.

### Firmware Assumption 

Goal is to find section of firmware where jingle data is, or prove jingle data is not stored in firmware.bin.

#### Status

* Using pinkySim I am currently able to simulate until firmware makes call to Wait For Interrupt (WFI) instruction
    * Details (to be used as basis for [custom firmware](https://github.com/greggersaurus/SteamControllerCustomFirmware)?):
        * TODO
    * Unclear if this is enough setup for jingle to be played or not.
        * Adding breakpoint instruction to isolate key code has had mixed results thus far.
    * Unclear if this is particular path of code that for some reason bypasses playing jingle (i.e some error path of the code due to some bad assumption on my part)

#### TODO

* Understand and add details to status on what code does as simulated so far.
    * Make sure to add details of latest sim with moving past 16-bit counter and reaching WFI.
        * Pay special attention to NVIC related chanages that are being made here...
        * Make mention of how controller maybe actually powers up on battery insert and low level sleeps until button is pushed?
            * Or was it something else insightful I meant to say here?
        * Dig into simulation thinking what pushing Steam Button or plugging in USB really does to power up the unit?
            * A counter to this theory is that the USB plugging in powers the dev board, which definitely is not in a low powered state...
            * Can we use a multimeter to probe the state of the chip with batteries in and out of the system?
    * WIP note/thoughts (be clear if anything has been confirmed with quantified hardware behavior changes)
        * Boilerplate init code added to lpcxpresso project targeting llu37 processor
            * More details to be added here...
        * Read from EEPROM Using IAP Command code 62
            * See 20.14.12 in UM10462 datasheet for command details
            * Excerpts from runLogFile_00000000001490067251:
                * 54144, alignedMemWrite: 8 kB SRAM0 (0x10001bc0 = 0x0000003e) -> Command Code 62 (decimal) Read EEPROM
                * 54145, alignedMemWrite: 8 kB SRAM0 (0x10001bc4 = 0x00000000) -> EEPROM Address
                * 54146, alignedMemWrite: 8 kB SRAM0 (0x10001bc8 = 0x10000254) -> RAM Address
                * 54147, alignedMemWrite: 8 kB SRAM0 (0x10001bcc = 0x00000008) -> Number of bytes to be read
                * 54141, alignedMemWrite: 8 kB SRAM0 (0x10001bd0 = 0x0000b71b) -> System Clock Frequency (CCLK) in kHz. (46.875 MHz)
        * Check 8 bytes of EEPROM data written to 0x10000254 by IAP
             * Check if word 0 is 0xa55a
             * Exceprts from runLogFile_00000000001490067251:
                * 56007, alignedMemRead: 8 kB SRAM0 (0x10000254 --> 0x00000000)
                * 56008, ldrhImmediate
                * 56009, cmpRegisterT1: Subtract Reg 2 (0x00000000) minus Reg 1 (0x0000a55a)
                * 56010, conditionalBranch: Not branching to 0x00000d26
        * Write to EEPROM Using IAP Command code 61 (only if magic word 0xa55a was not read from EEPROM)
            * See 20.14.11 in UM10462 datasheet for command details
                * 56054, alignedMemWrite: 8 kB SRAM0 (0x10001bb8 = 0x0000003d) -> Command Code 61 (decimal) Write EEPROM
                * 56055, alignedMemWrite: 8 kB SRAM0 (0x10001bbc = 0x00000000) -> EEPROM Address
                * 56056, alignedMemWrite: 8 kB SRAM0 (0x10001bc0 = 0x10000254) -> RAM Address
                * 56057, alignedMemWrite: 8 kB SRAM0 (0x10001bc4 = 0x00000008) -> Number of bytes to be written
                * 56051, alignedMemWrite: 8 kB SRAM0 (0x10001bc8 = 0x0000b71b) -> System Clock Frequency (CCLK) in kHz. (46.875 MHz)
        * TODO: More to be added here...
        * PMU
            * 58065,   alignedMemWrite: PMU (*0x40038008 = 0x00000000), 44577 -> Clear GPREG1
            * 60091,  alignedMemRead: PMU (*0x40038004 --> 0x00000000), 46391 -> Read GPREG0 (to later check if it is set to 0xecaabac0)
        * USB
            * In LCP11U37 datasheet look at Fig 24 in 11.7.3 and other charts to understand how USB transmission works.
                * Understanding how USB is setup may be key to understanding how jingle is transmitted.
            *  58103, alignedMemWrite: IOCON (0x40044018 = 0x00000001) -> PIO0_6 Set pin function to USB CONNECT
            *  58122, alignedMemWrite: IOCON (0x400440a4 = 0x00000002) -> PIO1_17 Set pin function to RXD
            *  58141, alignedMemWrite: IOCON (0x400440a8 = 0x00000002) -> PIO1_18 Set pin function to TXD
        * IOCON
            *  58085, alignedMemWrite: IOCON (0x4004400c = 0x00000008) -> PIO0_3 Pull-down resistor enabled
                *  what is this for? USB?
        * 16-bit timer 1 seems to be used as PWM to keep Steam Controller Button LED blinking
            * 266733, alignedMemWrite: IOCON (0x40044054 = 0x00000081) -> PIO0_21 Set pin function to CT16B1_MAT0
            * 266771, alignedMemWrite: 16-bit counter/timer 1 (0x4001000c = 0x00000000) -> Set Prescale Counter to 0
            * 266777, alignedMemWrite: 16-bit counter/timer 1 (0x40010074 = 0x00000001) -> PWM mode is enabled for CT16B1_MAT0
                * Why are changes made to MR0 and not MR3?
                * Check which pin PIO0_21 is physically and if this matches to PCB traces connecting to Steam Controller Button LED (from what can be seen on teardown image)
            * 266784, alignedMemWrite: 16-bit counter/timer 1 (0x40010024 = 0x00000fff) -> Timer counter match value MR3 is set to 0x0fff
            * 266787, alignedMemWrite: 16-bit counter/timer 1 (0x40010018 = 0x00001000) -> Timer counter match value MR0 is set to 0x1000
            * 266793, alignedMemWrite: 16-bit counter/timer 1 (0x40010014 = 0x00000400) -> Reset on MR3: the TC will be reset if MR3 matches it
            * 266800, alignedMemWrite: 16-bit counter/timer 1 (0x40010004 = 0x00000000) -> The counters are disabled
            * 266803, alignedMemWrite: 16-bit counter/timer 1 (0x40010008 = 0x00000001) -> Timer counter value set to 1
            * 266806, alignedMemWrite: 16-bit counter/timer 1 (0x40010004 = 0x00000002) -> The Timer Counter and the Prescale Counter are synchronously reset on the next positive edge of PCLK. The counters remain reset until TCR[1] is returned to zero.
            * 719056, alignedMemWrite: 16-bit counter/timer 1 (0x40010004 = 0x00000000) -> The counters are disabled
            * 719062, alignedMemWrite: 16-bit counter/timer 1 (0x40010004 = 0x00000001) -> The Timer Counter and Prescale Counter are enabled for counting
            * 719082, alignedMemWrite: 16-bit counter/timer 1 (0x40010018 = 0x0000ffff) -> Timer counter match value MR0 is set to 0xffff
        * 32-bit timer 1 seems to be used for system to timeout and shutdown if no connection is established
            * 719840, alignedMemWrite: 32-bit counter/timer 1 (0x4001800c = 0x0000bb7f) -> Set Prescale Counter to 0xbb7f
            * 719848, alignedMemWrite: 32-bit counter/timer 1 (0x40018014 = 0x00000002) -> Reset on MR0: the TC will be reset if MR0 matches it.
            * 719853, alignedMemWrite: 32-bit counter/timer 1 (0x40018014 = 0x00000003) -> Also, Interrupt on MR0: an interrupt is generated when MR0 matches the value in the TC.
            * 719856, alignedMemWrite: 32-bit counter/timer 1 (0x40018018 = 0x0000000b) -> Timer counter match value MR0 is set to 0xb
            * 719960, alignedMemWrite: 32-bit counter/timer 1 (0x40018004 = 0x00000001) -> The Timer Counter and Prescale Counter are enabled for counting.
        * private peripheral bus
            *  65079,   alignedMemRead: private peripheral bus (0xe000e414 --> 0x00000000),  50194 -> Read Interrupt Priority Register 5
            *  65083,    alignedMemWrite: private peripheral bus (0xe000e414 = 0x00000000),  50197 -> Make sure IP_USB_IRQ is set to highest priority (see previous instructions for details)
            *  65090,    alignedMemWrite: private peripheral bus (0xe000e100 = 0x00400000),  50202 -> Enable interrupt 22 (0-based) -> USB_IRQ -> USB IRQ Interrupt
            *  65109,   alignedMemRead: private peripheral bus (0xe000e414 --> 0x00000000),  50219 -> Read Interrupt Priority Register 5
            *  65113,    alignedMemWrite: private peripheral bus (0xe000e414 = 0x00400000),  50222 -> Set IP_USB_IRQ to one below highest priority
            *  65231,    alignedMemWrite: private peripheral bus (0xe000e100 = 0x00200000),  50298 -> Enable itnerrupt 21 (0-based) -> USART -> USART interrupt (and disable USB IRQ interrupt?)
                * Is the expectation that the USB IRQ fires before this and changes some state?
                * TODO: Look for branches or some countdown between these two instructions?
            *  65256,   alignedMemRead: private peripheral bus (0xe000e414 --> 0x00400000),  50319,
            *  65260,    alignedMemWrite: private peripheral bus (0xe000e414 = 0x00400000),  50322,
            *  65283,   alignedMemRead: private peripheral bus (0xe000ed20 --> 0x00000000),  50343,
            *  65287,    alignedMemWrite: private peripheral bus (0xe000ed20 = 0x00400000),  50346,
            * 307241,    alignedMemWrite: private peripheral bus (0xe000e280 = 0x00080000), 281832,
            * 307245,    alignedMemWrite: private peripheral bus (0xe000e100 = 0x00080000), 281834 -> Enable interrupt 19 (0-based) -> CT32B1 -> CT32B1 interrupt
               * TODO: This is interrupt handler for when the 32-bit counter counts down and the controller powers off because nothing connected?

* Make mods to firmware that are controlled and can be observed concretely via controller behavior changing
    * Mods to 16-bit and 32-bit counter setup does not seem to change anything
        * TODO: Try to simulate modified code to make sure firmware changes simulate as expected
        * Are discrepencies with what 16-bit and 32-bit timer seem to be doing versus what changes to firmware affects due to changes I am making being on a different code path than what controller is executing?
        * Everything else seems to add up in the sim being legit, so why aren't changes showing expeced reaction in hardware?
    * should focus be to simulate different startup paths and try to mod behavior there?
        * Complete: Simulate from beginning but change value read from EEPROM 
            * Have it read back magic word that seems to cause subsequent write and see if simulation is different
                * gbd command to step to instruction where it is safe to change memory that EEPROM controller was setup to write to
                    * stepi 42750 
                * gdb command to change memory EEPROM controller was setup to write to
                    * set {int}0x10000254 = 0xa55a
            * Diff of simulation runs show this just skips over writing magic word 0xa55a back to EEPROM.
                * Could not see change to memory that would effect other code paths (i.e. interrupt handlers)
        * Complete: Simulate from beginning but change values of 0x50000003 GPIOs PIO0_3
            * One byte is read from 0x50000003 and checked against 0x00000000
                * stepi 44525, 44650, 50353
                * set {int}0x50000003 = 1
            * Removes addtional check of 0x40048030 (System reset status register) if set
                * Some sort of reset or power supply indicator?
            * Changes what value is written to 0x50000028 
                * If PIO0_3 is 0 then PIO1_8 is set to 0
                * If PIO0_3 is 1 then PIO1_8 is set to 1
            * Changes whether long manual countdown occurs or not
                * Instructions 0x12c8 - 0x12c3
            * TODO: Check where these GPIOs are routed via ohming out the trace
                * PIO0_3 as input 
                * PIO1_8 as output
                    * Is this setup to be an output?
                * Actually, I think looking through documentation shows this is monitoring presence of USB bus power...
        * Complete: Simulate from beginning but change value of 0x40038004 (PMU GPREG0)
            * Is looking for register to be set to specific value 0xecaabac0
                * set {int}0x40038004 = 0xecaabac0
            * If this is already set some instructions are not executed and PMU GPREG0 is cleared
                * Some RAM addresses are read, but nothing else really seems to change...
        * TODO: look into possible branches after long delay before Steam Controller Button LED is setup to start flashing
            * What if connection happens? How is this handled?
            * Is this what happens if PIO0_3 is set to 1 and instructions 0x12c8 - 0x12c3 do not execute?
            * Could this be a connectivity line for USB?
                * PIO0_3 can function as USB_VBUS which monitors the presence of USB bus power.
        * TODO: Verify that there aren't other possible branches I am overlooking
            * Double check all hardware peripheral reads
            * Look deeply into how values read are used (maybe even later down the line by saving in RAM or being pushed on the stack?)
        * Insert breakpoint instruction (0xbebe) at various points in firmware to verfiry we are disassembling correctly.
            * Need to be careful that simulation paths for some reason do not match controller paths due to EEPROM config read...
            * Could also be affected by interrupt handlers triggering...
        * TODO: Need to put results of completed tests somewhere for future reference and remove from here?
            * Maybe in history/outline of simulation above?
            * Take a fresh look at this, clean up and clear out

* Start trying to simulate interrupt handlers
    * TODO: This should be next top priority
        * Simulation thus far could be somewhat inaccurate if interrupt handler code changes states and such... which might explain why certain mods don't seem to have any effect...
    * Assumption is that jingle is played when some interrupt handler is called
    * Need instructions for loading .bin files saved after sim dump
        * Need better organization of .bin files? 
            * Will need different .bin files for different scenarios?
            * Better way to do this in general?
    * Need to set status register properly depending on which 
    * Figure out which interrupt handler happens when 32-bit counter causes shutdown and simualte
        * Jingle is also played on shutdown

* Full disassembly of firmware.bin to assembly.
    * Use idea in FirmwareParser, but extend pinkySim's ability to decode and execute instructions.

* Hook up UART to Steam Controller?
    * Simulation makes it look like USART is being setup
        * Need to verify it is being setup as UART
    * Requires solder on to Steam Controller...
        * Are correct pins obvious?

* Hook up debugger to Steam Controller?
    * Use debugger on LPCXpresso11U37H Evaluation board OM13074
    * Create pogo pin adapter to debug spot on Steam Controller
        * Need to understand how to connect 10 pin header to 6 ping DEBUG (if possible...)

### EEPROM Assumption

Goal is to find section fo EEPROM where jingle data is, or prove jingle data is not stored in EEPROM.

#### Status

* Working on custom firmware based on nxp_lpcxpresso_11u37_usbd_rom_cdc_uart example. 
    * Idea is to have have USB port act as UART.
    * Will be able to issue commands to read memory or request EEPROM data.
    * Created code and loaded on controller, but did not work (not sure if it even powered up... Pinmux setting? Dig into 11U37h board.c?).

#### TODO

* Incorporate data from longer simulation run for more complete setup so that custom firmware works.
    * Maybe we are missing something to actually keep the system powered up?
* Once custom firmware is working.
    * Look at what is in EEPROM. Feed back into simulation to show path actually being take through firmware?
    * Compare how EEPROM changes between configuration changes via Steam.

# Resources, Utilities and Influences

## [How to Manually Load Firmware](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)

* Hold right trigger while connecting via USB
    * Mount "CRP DISABLD" will appear
    * cat new_firmware.bin > /mount/CRP\ DISABLD/firmware.bin
    * eject CRP DISABLD
* Used to load old firmware on controller to monitor update process via Steam
* Will be used to load custom firmware on to read/write EEPROM if necessary?

## [Pilatomic Steam Controller Singer](https://gitlab.com/Pilatomic/SteamControllerSinger)

* Example of using the haptics to play music
* Example of code written in C using libusb
* Downside is that it takes over controller when playing custom music

## [Standalone Steam Controller Driver](https://github.com/ynsta/steamcontroller)

* OK source for breaking down some USB commands
* Goal of this project is to work outside Steam platform. I want to add configuration ability to controller, not subvert or reproduce (though some reproduction will be necessary) what is already there

## [Teardown of Steam Controller](https://www.ifixit.com/Teardown/Steam+Controller+Teardown/52578)

* Picture shows processor is LPC11U37F/501

## [NXP LPC11U37FBD64/501 Specifics](http://www.nxp.com/products/microcontrollers-and-processors/arm-processors/lpc-cortex-m-mcus/lpc-cortex-m0-plus-m0/lpc1100-cortex-m0-plus-m0/128kb-flash-12kb-sram-lqfp64-package:LPC11U37FBD64?fpsp=1&tab=Documentation_Tab)

Below are some accumulated details on the main processor on the Steam Controller. 
Understanding the processor may be key for meeting some requirements.

### [Datasheet](http://www.nxp.com/documents/data_sheet/LPC11U3X.pdf?fasp=1&WT_TYPE=Data%20Sheets&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=pdf&WT_ASSET=Documentation&fileExt=.pdf)

### [User Manual](http://www.nxp.com/documents/user_manual/UM10462.pdf)

* Firmware is 128 kb.

#### Chapter 20: LPC11U3x/2x/1x Flash programming firmware

* UM10462 for details on loading firmware via bootloader

#### Chapter 24: LPC11U3x/2x/1x Appendix ARM Cortex-M0

* 32-bit processor
* ARMv6-M architecture (16-bit Thumb ISA and includes Thumb-2 technology?)
    * [ISA](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.subset.architecture.reference/index.html)

### [Secondary Bootloader App Note](http://www.nxp.com/documents/application_note/AN11732.zip?fasp=1&WT_TYPE=Application%20Notes&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=zip&WT_ASSET=Documentation&fileExt=.zip)

### LPCXpresso IDE

#### [Generating .bin file](https://community.nxp.com/thread/389005)

* AXF file (ELF/DWARF) to binary
    * arm-none-eabi-objcopy -v -O binary "FirstProject.axf" "FirstProject.bin"
* From objcopy [man page](https://sourceware.org/binutils/docs/binutils/objcopy.html)
    * "When objcopy generates a raw binary file, it will essentially produce a memory dump of the contents of the input object file. All symbols and relocation information will be discarded. The memory dump will start at the load address of the lowest section copied into the output file."

#### .bin File Layout
* Starts with vector table
    * From 20.7 of UM10462: The reserved ARM Cortex-M0 exception vector location 7 (offset 0x0000 001C in the vector table) should contain the 2’s complement of the check-sum of table entries 0 through 6. This causes the checksum of the first 8 table entries to be 0. The bootloader code checksums the first 8 locations in sector 0 of the flash. If the result is 0, then execution control is transferred to the user code.

* See Section 24.3.3.4 (Fig 24-80) of UM10462 for layout of vector table
    * 0x0000 00004 - Entry point to code in ARM Cortex-M0 vector table. See 20.15.1 of UM10462
    * 0x0000 0001C - Vector location 7. Must contain 2's complement of vector table entries 0-6. See 20.7 or UM10462

* Endianness
    * Certain sections default to little endian
    * Other sections are configurable
    * TODO: Where is this stated in UM10462?

#### [LPC Image Checksums](https://community.nxp.com/thread/389046)

* Checksum is stored at 0x1C for Cortex-M based parts
    * checksum -p ${TargetChip} -d "${BuildArtifactFileBaseName}.bin"
* This checksum is calculated from the contents of the vector table, not the whole image (probably why I got away with chaning USB info strings)
    * For more details please see the user manual for the MCU that you are using.
* [Creating checksum manually](https://community.nxp.com/thread/388993)

## [Steam Controller Update News](http://store.steampowered.com/news/?appids=353370)

Use to get an idea of what changed from firmware to firwmare release

## Steam Communities

* [Official Group](http://steamcommunity.com/games/353370#announcements/detail/901091250587237164)
* [Custom Sounds Discussion](https://steamcommunity.com/app/353370/discussions/0/458607699626517823/)

## Disassembly 

Various approaches, tools and information on disassembling compiled software. 
It may be necessary to disassemble the Steam Controller firmware to meet goals
 such as customizing jingles.

### [Reverse Engineering for Beginners](https://github.com/dennis714/RE-for-beginners)

Free book on how to reverse engineer code.

### [pinkySim](https://github.com/greggersaurus/pinkySim)

* ARMv6-M Thumb instruction simulator.
* Forked and working on logging function.
    * Helpful to have (slightly more) human readable log of what code is doing.
    * Can possibly be used to recreate symbol table for firmware binary.
* Incentive of gaining more experience with gdb
    * Using gdb installed by LPCXpresso IDE (On OSX: /Applications/lpcxpresso_8.2.2_650/lpcxpresso/tools/bin/arm-none-eabi-gdb)

#### Setup:

##### Launch Simulator 

The following command launches the emulator with the proper memory map of the LPC11U37F501:

* ./pinkySim --breakOnStart --flash 0 131072 --ram 268435456 8192 --ram 536805376 16384 --ram 536870912 2048 --ram 536887296 2048 --ram 1073741824 16384 --ram 1073758208 16384 --ram 1073774592 16384 --ram 1073790976 16384 --ram 1073807360 16384 --ram 1073823744 16384 --ram 1073840128 16384 --ram 1073856512 16384 --ram 1073971200 16384 --ram 1073987584 16384 --ram 1074003968 16384 --ram 1074020352 16384 --ram 1074036736 16384 --ram 1074053120 16384 --ram 1074102272 16384 --ram 1074118656 16384 --ram 1074135040 16384 --ram 1074266112 16384 --ram 1342177280 16484 --ram 3758096384 1048576 firmware.bin
    * Note: --ram 536805376 16384 --flash, but since we need to fill this ROM with the boot ROM code via gdb, this needs to be writable

##### Set Memory Defaults 

The following command launches gbd, attaches it to the running eumulator and sets up register to values that allow system to proceed past initialization:

* ./gdb -ex "target remote localhost:3333" -ex "set {int}0x40008004 = 0" -ex "set {int}0x4000800c = 0" -ex "set {int}0x40008014 = 0x0060" -ex "set {int}0x40010004 = 0" -ex "set {int}0x40010008 = 0" -ex "set {int}0x40010014 = 0" -ex "set {int}0x40010074 = 0" -ex "set {int}0x40018004 = 0" -ex "set {int}0x40018014 = 0" -ex "set {int}0x40038004 = 0" -ex "set {int}0x4003c010 = 2" -ex "set {int}0x4003cfe0 = 0xFFFFFFFF" -ex "set {int}0x40048000 = 2" -ex "set {int}0x40048008 = 0" -ex "set {int}0x4004800c = 1" -ex "set {int}0x40048014 = 1" -ex "set {int}0x40048030 = 3" -ex "set {int}0x40048040 = 1" -ex "set {int}0x40048044 = 1" -ex "set {int}0x40048070 = 0" -ex "set {int}0x40048074 = 1" -ex "set {int}0x40048078 = 1" -ex "set {int}0x40048080 = 0x3F" -ex "set {int}0x40048170 = 0x10" -ex "set {int}0x4004819C = 1" -ex "set {int}0x40048230 = 0xFFFF" -ex "set {int}0x40048234 = 0xEDF0" -ex "set {int}0x40048238 = 0xEDD0" -ex "set {int}0x40080000 = 0x0800" -ex "set {int}0x50000003 = 0" -ex "set {int}0x50002004 = 0" -ex "set {int}0xe000e414 = 0" -ex "set {int}0xe000ed20 = 0"
    * Once connected use command "restore LPC11U3x16kBbootROM.bin binary 0x1fff0000" to fill boot ROM with binary downloaded from LPCXpresso11U37H dev board (i.e. LPC Expresso V2 board for 11U37H) 
    * Execute "continue" to start simulation.
    * Will need to break (ctrl-c) and execute command "set {int}0x40010008 = 0" to get simulation past waiting for 16-bit counter/timer 1.
        * User can tell this needs to happen when instructions 0x61a, 0x61c, 0x61e repeat non-stop.
    * Simulation will end with 0xa6e as last valid instruction.
        * Next instruction is Wait for Interrupt (WFI), which pinkySim simulator reports as Unsupported Instruction.
    * The following commands can be used to save the state of memory if say the user wants to reload state for attempting simulation of interrupts without having to re-run entire simulation.
        * 8 kB SRAM
             * dump binary memory sram.bin 0x10000000 0x10002000
        * 2 kB USB SRAM
             * dump binary memory usbsram.bin 0x20004000 0x20004800
        * APB peripherals
             * dump binary memory apb1.bin 0x40000000 0x40020000
             * dump binary memory apb2.bin 0x40038000 0x40050000
             * dump binary memory apb3.bin 0x40058000 0x40064000
        * USB registers
             * dump binary memory usb.bin 0x40080000 0x40084000
        * GPIO registers
             * dump binary memory gpio.bin 0x50000000 0x50004000
        * private peripheral bus
             * dump binary memory privperiph.bin 0xe0000000 0xe0100000 

###### Breakdown of Input Arguments

The following outlines details on the input arguments of the previous section and why they are set the way they are:

* Connect to remote simulator being run on port 3333 of local machine
    * target remote localhost:3333
* USART/SMART CARD Register Settings (base offset 0x40008000)
    * Set USART Divisor Latch Register (when DLAB = 0) 0x40008004 to 0 (reset value)
        * set {int}0x40008004 = 0
    * Set USART Line Control Register 0x4000800c to 0x00000000 (reset value)
        * set {int}0x4000800c = 0
    * Set Line Status Register (Read Only) 0x40008014 to 0x00000060 (reset value)
        * set {int}0x40008014 = 0x0060 
* 16-bit Counter/Timer 1 Register Settings (base offset 0x40010000)
    * Set Timer Control Register 0x40010004 to 0 (reset value)
        * set {int}0x40010004 = 0
    * Set Time Counter Register 0x40010008 to 0 (reset value)
        * set {int}0x40010008 = 0
    * Set Match Control Register 0x40010014 to 0 (reset value)
        * set {int}0x40010014 = 0
    * Set PWM Control Register 0x40010074 to 0 (reset value)
        * set {int}0x40010074 = 0
* 32-bit Counter/Timer 1 Register Settings (base offset 0x40018000)
    * Set Timer Control Register 0x40018004 to 0 (reset value)
        * set {int}0x40018004 = 0
    * Set Match Control Register 0x40018014 to 0 (reset value)
        * set {int}0x40018014 = 0
* PMU Register Settings (base offset 0x40038000)
    * Set General purpose register 0 0x40038004 to 0x00000000 (reset value)
        * set {int}0x40038004 = 0
* Flash/EEPROM Controller Register Settings (base offset 0x4003C000)
    * Set Flash configuration register 0x4003c010 to 0x00000002 (reset value)
        * set {int}0x4003c010 = 2
    * Set Flash module status register 0x4003cfe0 to 0xFFFFFFFF to indicate that EEPROM write finished
        * set {int}0x4003cfe0 = 0xFFFFFFFF
* System Control Register Settings (base offset 0x40048000)
    * Set System memory remap register 0x40048000 to 0x00000002 (reset value)
        * set {int}0x40048000 = 2
    * Set System PLL contro register 0x40048008 to 0x00000000 (reset value)
        * set {int}0x40048008 = 0
    * Set System PLL status register 0x4004800c to 0x00000001 (indicates System PLL is locked)
        * set {int}0x4004800c = 1 
    * Set USB PLL status register 0x40048014 to 0x00000001 (indicates USB PLL is locked)
        * set {int}0x40048014 = 1
    * Set System reset status register 0x40048030 to 0x00000003 (reset value)
        * set {int}0x40048030 = 3
    * Set System PLL clock source register 0x40048040 to 0x00000001 (reset value)
        * set {int}0x40048040 = 1
    * Set System PLL clock source update register 0x40048044 to 0x00000001 (reset value)
        * set {int}0x40048044 = 1
    * Set Main clock source select register 0x40048070 to 0x00000000 (reset value)
        * set {int}0x40048070 = 0
    * Set Main clock source update enable register 0x40048074 to 0x00000001 (reset value)
        * set {int}0x40048074 = 1
    * Set System clock divider register 0x40048078 to 0x00000001 (reset value)
        * set {int}0x40048078 = 1
    * Set Sytem clock control register 0x40048080 to 0x0000003F (reset value)
        * set {int}0x40048080 = 0x3F
    * Set IRQ Latency register 0x40048170 to 0x00000010 (reset value)
        * set {int}0x40048170 = 0x10
    * Set USB block status register 0x4004819C to 0x00000001 (reset value)
        * set {int}0x4004819C = 1
    * Set Deep-sleep mode configuration register 0x40048230 to 0x0000FFFF (reset value)
        * set {int}0x40048230 = 0xFFFF
    * Set Wake-up configuration register 0x40048234 to 0x0000EDFO (reset value)
        * set {int}0x40048234 = 0xEDF0
    * Set Power configuration register 0x40048238 to 0x0000EDDO (reset value)
        * set {int}0x40048238 = 0xEDD0
* USB Register Settings (base offset 0x40080000)
    * Set USB Device Command/Status register 0x40080000 to 0x00000800 (reset value)
        * set {int}0x40080000 = 0x0800
* GPIO Register Settings (base offset 0x50000000)
    * Set GPIO port byte pin register 0x50000003 to 0x00000000 (assume external values of P0_3, P0_4, P0_5 and P0_6 are all 0)
        * set {int}0x50000003 = 0
        * Note that we do not yet know 100% what these GPIOs should read be reading or how the software reacts to them
    * Set GPIO direction port 1 register 0x50002004 to 0x00000000 (reset value)
        * set {int}0x50002004 = 0
* Private Peripheral Bus Register Settings (base offset 0xE0000000)
    * Set Input Priority Register 5 0xe000e414 to 0x00000000 (reset value)
        * set {int}0xe000e414 = 0
    * Set System Handler Priority Register 3 0xe000ed20 to 0x00000000 (reset value)
        * set {int}0xe000ed20 = 0

###### Simulation Oddities

This section outlines oddities observed in simulation with possible explanations.

* 0x4003c000 is read from and written to, but UM10462 datasheet makes no mention of this register.
    * This is done by boot ROM code. So maybe it knows some secrets not mentioned in datasheet.
* 0x4003c08c is read from and written to, but UM10462 datasheet makes no mention of this register.
    * This is done by boot ROM code. So maybe it knows some secrets not mentioned in datasheet.
* 0x4003c094 is read from and written to, but UM10462 datasheet makes no mention of this register.
    * This is done by boot ROM code. So maybe it knows some secrets not mentioned in datasheet.
* 0x40048224 is read from and written to, but UM10462 datasheet makes no mention of this register.
    * This is done by boot ROM code. So maybe it knows some secrets not mentioned in datasheet.

* According to UM10462 datahsset, flash module status register 0x4003cfe0 only has bit 2 as non-reserved, but boot ROM code is checking other bits for status.
    * Assumptionis that this must be some weird hardware issue with how reserved bits function. Upper bits should be non-use, but setting bit 2 does not add up to check being performed on register (lsl immediate).

### [Radare](http://www.radare.org/r/)

* [Steep learning curve](https://www.gitbook.com/book/radare/radare2book/details)
* Has option to disassemble ARM, but how exactly does it handle ARMv6-M?
    * Does not automatically work with firmware.bin as input
    * What about #if 0  section iwth armv6 options?
* With have firmware binary format (i.e. no code/data sections information) this only helps so much. 
    * Simulation with pinkySim can be utilized to possibly rebuild this section information and then revisit this tool?

### [Fracture](https://github.com/draperlaboratory/fracture)

* Decompiles to LLVM intermediate representation
    * It looks like this does not work with binary file format with no code/data sections information.
    * Simulation with pinkySim can be utilized to possibly rebuild this section information and then revisit this tool?

### LPCXpresso

* Disassemble axf https://community.nxp.com/thread/388997.
    * It looks like this does not work with binary file format with no code/data sections information.

### [Online Disassembler](https://onlinedisassembler.com/odaweb/)

* Good source to punch individual instruction in quickly
* Make minor mods to instructions to verify understanding of how they are being decoded and behaving (i.e. LDR)
* Finicky interface
    * Might work better if file is uploaded instead of copy paste
* Without code/data section information tool tries to decode everything
    * Simulation with pinkySim can be utilized to possibly rebuild this section information and then revisit this tool?

### FirmwareParser.py

Original idea was to create disassembler that can recreate assembly file, 
distinguishing data versus instructions by evaulating code and all possible
branches. 

In the end this was a larger undertaking than expected. It would make more 
sense to leverage pinkySim's ability to decode instructions and their 
behavior to do this, as opposed to starting from scratch.

### [objdump](https://sourceware.org/binutils/docs/binutils/objdump.html)

* ./arm-none-eabi-objdump -b binary -D vcf_wired_controller_d0g_57bf5c10.bin -m arm attempts to disassemble binary file
* With have firmware binary format (i.e. no code/data sections information) this only helps so much. 
    * Simulation with pinkySim can be utilized to possibly rebuild this section information and then revisit this tool?

### [ARMu](http://pel.hu/armu/)

* For ARMv5TE, but might be worth understanding approach if this works from binary

### [Capstone](http://www.capstone-engine.org/)

* Worth looking into?

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
