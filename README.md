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
    * Details:
        * TODO
    * Unclear if this is enough setup for jingle to be played or not.
        * Adding breakpoint instruction to isolate key code has had mixed results thus far.
    * Unclear if this is particular path of code that for some reason bypasses playing jingle (i.e some error path of the code due to some bad assumption on my part)

#### TODO

* Understand and add details to status on what code does as simulated so far.
    * Make sure to add details of latest sim with moving past 16-bit counter and reaching WFI.
        * Pay special attention to NVIC related chanages that are being made here...

* Check that defaults are set for all configuration registers being accessed and re-simulate
    * Now that we are getting past EEPROM access more registers are being accessed.

* In LCP11U37 datasheet look at Fig 24 in 11.7.3 and other charts to understand how USB transmission works.
    * Understanding how USB is setup may be key to understanding how jingle is transmitted.

* Consider what if code to play jingle occurs in interrupt handler.
    * Current simulation never reaches this code.
    * Which interrupt is for USB and how should interrupt status register be set?

* Insert breakpoint instruction (0xbebe) at various points in firmware to verfiry we are disassembling correctly.
    * Need to be careful that simulation paths for some reason do not match controller paths due to EEPROM config read...

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

* ./gdb -ex "target remote localhost:3333" -ex "set {int}0x40048000 = 2" -ex "set {int}0x4004800c = 1" -ex "set {int}0x40048014 = 1" -ex "set {int}0x40048028 = 0x080" -ex "set {int}0x40048030 = 3" -ex "set {int}0x40048040 = 1" -ex "set {int}0x40048044 = 1" -ex "set {int}0x40048074 = 1" -ex "set {int}0x40048078 = 1" -ex "set {int}0x40048080 = 0x3F" -ex "set {int}0x40048170 = 0x10" -ex "set {int}0x4004819C = 1" -ex "set {int}0x40048230 = 0xFFFF" -ex "set {int}0x40048234 = 0xEDF0" -ex "set {int}0x40048238 = 0xEDD0" -ex "set {int}0x4003cfe0 = 0xFFFFFFFF"
    * Once connected use command "restore LPC11U3x16kBbootROM.bin binary 0x1fff0000" to fill boot ROM with binary downloaded from LPCXpresso11U37H dev board (i.e. LPC Expresso V2 board for 11U37H) 

###### Breakdown of Input Arguments

The following outlines details on the input arguments of the previous section and why they are set the way they are:

* Connect to remote simulator being run on port 3333 of local machine
    * target remote localhost:3333
* Set 0x40048000 to 0x00000002 (reset value)
    * set {int}0x40048000 = 2
* Set 0x4004800c to 0x00000001 to indicate System PLL is locked                  
    * set {int}0x4004800c = 1 
* Set 0x40048014 to 0x00000001 to indicate USB PLL is locked
    * set {int}0x40048014 = 1
* Set 0x40048028 to 0x00000080 (reset value)
    * set {int}0x40048028 = 0x080
* Set 0x40048030 to 0x00000003 (reset value)
    * set {int}0x40048030 = 3
* Set 0x40048040 to 0x00000001 (reset value)
    * set {int}0x40048040 = 1
* Set 0x40048044 to 0x00000001 (reset value)
    * set {int}0x40048044 = 1
* Set 0x40048074 to 0x00000001 (reset value)
    * set {int}0x40048074 = 1
* Set 0x40048078 to 0x00000001 (reset value)
    * set {int}0x40048078 = 1
* Set 0x40048080 to 0x0000003F (reset value)
    * set {int}0x40048080 = 0x3F
* Set 0x40048170 to 0x00000010 (reset value)
    * set {int}0x40048170 = 0x10
* Set 0x4004819C to 0x00000001 (reset value)
    * set {int}0x4004819C = 1
* Set 0x40048230 to 0x0000FFFF (reset value)
    * set {int}0x40048230 = 0xFFFF
* Set 0x40048234 to 0x0000EDFO (reset value)
    * set {int}0x40048234 = 0xEDF0
* Set 0x40048238 to 0x0000EDDO (reset value)
    * set {int}0x40048238 = 0xEDD0
* Set 0x4003cfe0 to 0xFFFFFFFF to convince system that EEPROM write finished
    * NOTE: This must be some weird hardware issue relating to the conditional for checking write. Upper bits should be non-use, but setting bit 2 does not add up to check being performed on register (lsl immediate).
    * set {int}0x4003cfe0 = 0xFFFFFFFF

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
