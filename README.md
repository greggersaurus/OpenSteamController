# Introduction

The purpose of this project is to explore, deconstruct and, hopefully, expand 
 upon configuration options for the Steam Controller. This is both a personal 
 project to learn more about USB and gain experience with Python as well as 
 something that can hopefully be shared with the community to provide the 
 ability to safely customize a Steam Controller in ways not currently supported
 through Steam.

# Resources and Influences

## [How to manually load firmware on NXP chip](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)
* Hold right trigger while connecting via USB
 * Mount "CRP DISABLD" will appear
 * cat new_firmware.bin > /mount/CRP\ DISABLD/firmware.bin
 * eject CRP DISABLD
* Used to load old firmware on controller to monitor update process via Steam
* Will be used to load custom firmware on to read/write EEPROM if necessary?

## [Pilatomic Steam Controller Singer](https://gitlab.com/Pilatomic/SteamControllerSinger)
* Great example of using the haptics to play music
* Example of code written in C using libusb
* Downside is that it takes over controller when playing custom music

## [Standalone Steam Controller Driver](https://github.com/ynsta/steamcontroller)
* OK source for breaking down some USB commands
* Goal of this project is to work outside Steam platform. I want to add configuration ability to controller, not subvert or reproduce (though some reproduction will be necessary) what is already there

## [Teardown of Steam Controller](https://www.ifixit.com/Teardown/Steam+Controller+Teardown/52578)
* Picture shows processor is LPC11U37F/501

## [NXP LPC11U37FBD64](http://www.nxp.com/products/microcontrollers-and-processors/arm-processors/lpc-cortex-m-mcus/lpc-cortex-m0-plus-m0/lpc1100-cortex-m0-plus-m0/128kb-flash-12kb-sram-lqfp64-package:LPC11U37FBD64?fpsp=1&tab=Documentation_Tab)

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
 * TODO: How does this affect disassembly? How we do rebuild symbols, etc.?

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
* Use to get an idea of what changed from firmware to firwmare release

## Steam Communities
* [Official Group](http://steamcommunity.com/games/353370#announcements/detail/901091250587237164)
* [Custom Sounds Discussion](https://steamcommunity.com/app/353370/discussions/0/458607699626517823/)

## Disassembly 

* Going to dig into and work with a number of tools and approaches
 * Use different tools to verify each other

### [Reverse Engineering for Beginners](https://github.com/dennis714/RE-for-beginners)

### [pinkySim](https://github.com/greggersaurus/pinkySim)

* ARMv6-M Thumb instruction simulator.
* Forked and working on logging function.
 * Helpful to have (slightly more) human readable log of what code is doing.
 * Can possibly be used to recreate symbol table for firmware binary.
* Incentive of gaining more experience with gdb
 * Using gdb installed by LPCXpresso IDE (On OSX: /Applications/lpcxpresso_8.2.2_650/lpcxpresso/tools/bin/arm-none-eabi-gdb)

#### Setup:

##### Launch Simulator (with proper memory map)
* ./pinkySim --breakOnStart --flash 0 131072 --ram 268435456 8192 --ram 536805376 16384 --ram 536870912 2048 --ram 536887296 2048 --ram 1073741824 16384 --ram 1073758208 16384 --ram 1073774592 16384 --ram 1073790976 16384 --ram 1073807360 16384 --ram 1073823744 16384 --ram 1073840128 16384 --ram 1073856512 16384 --ram 1073971200 16384 --ram 1073987584 16384 --ram 1074003968 16384 --ram 1074020352 16384 --ram 1074036736 16384 --ram 1074053120 16384 --ram 1074102272 16384 --ram 1074118656 16384 --ram 1074135040 16384 --ram 1074266112 16384 --ram 1342177280 16484 --ram 3758096384 1048576 firmware.bin
 * Note: --ram 536805376 16384 --flash, but since we need to fill this ROM with the boot ROM code via gdb, this needs to be writable

##### Set Memory Defaults (to simulate peripherals and get through initialization)

* ./gdb -ex "target remote localhost:3333" -ex "set {int}0x40048000 = 2" -ex "set {int}0x4004800c = 1" -ex "set {int}0x40048014 = 1" -ex "set {int}0x40048028 = 0x080" -ex "set {int}0x40048030 = 3" -ex "set {int}0x40048040 = 1" -ex "set {int}0x40048044 = 1" -ex "set {int}0x40048074 = 1" -ex "set {int}0x40048078 = 1" -ex "set {int}0x40048080 = 0x3F" -ex "set {int}0x40048170 = 0x10" -ex "set {int}0x4004819C = 1" -ex "set {int}0x40048230 = 0xFFFF" -ex "set {int}0x40048234 = 0xEDF0" -ex "set {int}0x40048238 = 0xEDD0"
 * Once connected use command "restore LPC11U3x16kBbootROM.bin binary 0x1fff0000" to fill boot ROM with binary downloaded from LPCXpresso11U37H dev board (i.e. LPC Expresso V2 board for 11U37U) 

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

##### TODO: Application is currently falling apart after blx to 0x1fff1ff0
* This seems to be a call into IAP commands (i.e. for accessing EEPROM). 
* Either simulator cannot handle this or data being returned from "EEPROM read" is sending asm code off in the weeds
* To make sure we can simulate an IAP EEPROM read by taking lpcopen_v2_03_lpcxpresso_nxp_lpcxpresso_11u37f501 project and having it attempt to access EEPROM via IAP (Use command 62 for EEPROM read and give it a unique RAM address to write to, so we know what Steam Controller firmware IAP command is trying to do)
 * If this passes then it is what is getting "read" from EEPROM that is causing simulator to run off in the weeds

### [Radare](http://www.radare.org/r/)

* [Steep learning curve](https://www.gitbook.com/book/radare/radare2book/details)
 * TODO: Compile test code using LPCXpresso so we have .bin and known source
* Has option to disassemble ARM, but how exactly does it handle ARMv6-M?
 * Does not automatically work with firmware.bin as input
 * What about #if 0  section iwth armv6 options?

### [Fracture](https://github.com/draperlaboratory/fracture)

* Decompiles to LLVM intermediate representation
 * TODO: It looks like this does not work with binary file format. Dig deeper to confirm and cross off list if so.

### LPCXpresso

* Disassemble axf https://community.nxp.com/thread/388997.
 * TODO: how to access disassembler outside of GUI?
 * TODO: Can use this to disassemble .bin?
 * TODO: Is this just using objdump?

### [Online Disassembler](https://onlinedisassembler.com/odaweb/)

* Good source to punch individual instruction in quickly
* Make minor mods to instructions to verify understanding of how they are being decoded and behaving (i.e. LDR)
* Finicky 
 * Might work better if file is uploaded instead of copy paste

### FirmwareParser.py

* I am writing this, even though there are a host of other tools that cover this functionality
 * Act of creating disassembler drives in knowledge of how instructions function
 * One more scenario to get better with Python
* Starts with known info (vector table) and attempts to decompile by stepping through known paths and accesses
 * Vector table points to known instructions
 * After each known instruction decode can next instructions or possible data
 * Attempt to decompile remaining instructions?
 * From instructions such as load/store can infer potential data sections?
* Will only progress this as far as is useful

#### TODO:
* test decoding of LDR address
* ability to enter classification of bin file
 * Move this forward by being able to decode more instructions?
 * Also consider this should allow full classification of firmware.bin (unless jumps are based on reg vals...), but may only take us so far as certain instructions are based on register values... 
 * 0x2f0 data comment (Mark 32-bit word starting at offset 0x2f0 as data)
 * 0x4f0 instruction comment (Mark treat word starting at offset 0x4f0 as instruction)
 * App will accept text file and read this in
 * When instructions are decoded they make their own array of these to automatically identify more of fw (non-branch instruction always marks next instruction)
* Read up on and understand compilers better (i.e. Relocation)

### [objdump](https://sourceware.org/binutils/docs/binutils/objdump.html)

* ./arm-none-eabi-objdump -b binary -D vcf_wired_controller_d0g_57bf5c10.bin -m arm attempts to disassemble binary file
 * TODO: note that -d instead of -D produces nothing, while -D is all wrong
* TODO: read up more on objdump as it seems to just try and decode every line. What other options might be useful?
 * For VAX, you can specify function entry addresses with -M entry:0xf00ba.  You can use this multiple times to properly disassemble VAX binary files that don't contain symbol tables (like ROM dumps).  In these cases, the function entry mask would otherwise be decoded as VAX instructions, which would probably lead the rest of the function being wrongly disassembled."
 * Are compiler options from LPCXpresso helpful at all? (arm-none-eabi-c++ -D__NEWLIB__ -DNDEBUG -D__CODE_RED -DCORE_M0 -D__USE_ROMDIVIDE_ -DCPP_USE_HEAP -D__LPC11UXX__ -Os -fno-common -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -mcpu=cortex-m0 -mthumb -MMD -MP -MF"src/FirstProject.d" -MT"src/FirstProject.o" -MT"src/FirstProject.d" -o "src/FirstProject.o" "../src/FirstProject.cpp"
* ndiasm was given as an example when looking up "disassemble binary file" but this is for x86. See what we can learn from what was done here and what objdump tries to do. 
* TODO: read up on idea of [sync points](http://www.nasm.us/doc/nasmdoca.html) (is this common nomenclature?) 

### [ARMu](http://pel.hu/armu/)

* For ARMv5TE, but might be worth understanding approach if this works from binary

### [Capstone](http://www.capstone-engine.org/)

* Worth looking into?

# Goals/Requirements

## Written in Python
* While I could produce cross platform code written in C using libusb, producing Python (that using pyusb) seems to be a simpler platform independent solution
* I do not know much Python, so this will be a good learning experience

##  Get controller info

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

## Play song (using haptics)
* Instruct controller to play a particular song to demo what it sounds like
* Reproduction of what Steam does when you select a song for startup or shutdown in config
* Status: Implemented, but does not work on firmware before 5653a68a
* TODO: add firmware check and report error if firmware if before 5653a68a

## Config startup and shutdown song 
* Reproduction of what Steam does when you click confirm after viewing/changing controller config
* Status: Captured data and analyzed
* TODO: Implement in code

## Configure Steam Button Brightness
* Status: Captured commands broke down represent brightness value
* TODO: check if this works with firmware previous to 5653a68a

## Load firmware
* Reproduction of what Steam does automatically when it detects out of date firmware, but this will allow for loading custom firmware or specifying which previous firmware specified in vdf should be installed
* Status: Captured USB data 
* TODO: Analyze USB data and implement function. Look for failure retry in captured data. 

## Custom song 
* Assumption is that song files are coded into firmware or stored in EEPROM on NXP chip

### Firmware assumption 
* Start here and definitively determine if it is in code or not
* TODO: Reverse compile firmware (how is firmware laid out (mapping), look at ARM assembly instructions?)   
 * See Resources related to LPCXpresso and NXP LPC11U37FBD64 and radare
* TODO: Create example project for chip using LPCXpresso and dig into compiler, etc.
 * See Resources related to LPCXpresso and NXP LPC11U37FBD64 and radare
* TODO: Look at code changes from 5643849f to 5653a68a. Loading firmware older than 5653a68a results in system not playing song, but could just be different EEPROM setup?
* TODO: How does Steam react to pre-5653a68a firmware? Saw something in Steam Controller updates about change to firmware and Steam handling this update. Does this mean there are USB commands to access EEPROM?
* TODO: Look into mention in updates of storing custom configs/mappings and how they travel with controller. Will this allow us access to what we want somehow?

### EEPROM assumption
* Can we dump EEPROM somehow?
 * TODO: see mention above above config/mappings traveling with controller. Does this allow us access?
 * TODO: write custom firmware that dump EEPROM over USB (is this realistic? how to set pins to not damage controller?)

* Working on custom firmware based on nxp_lpcxpresso_11u37_usbd_rom_cdc_uart example. 
 * Idea is to have have USB port act as UART
 * Will be able to issue commands to read memory or request EEPROM data
 * Created code and loaded on controller, but did not work (not sure if it even powered up... Pinmux setting? Dig into 11U37h board.c?)

# USB Data Capture

* Use lsusb to get bus and devnum
* Use [usbmon](https://www.kernel.org/doc/Documentation/usb/usbmon.txt)
 * cat /sys/kernel/debug/usb/usbmon/u0 
 * Pipe to grep to filter on data we care about (i.e. bus and devnum)
* Capture actions
 * ssh into Steam Box and cat usbmon file
 * Perform distinct actions and capture data
 * Isolate variables with slight variations on actions
