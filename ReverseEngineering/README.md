# Reverse Engineering

The work in this directory focuses on deconstructing the firmware and hardware
 of the Steam Controller. The understanding gained from this is the basis for
 the other work being done in this project. 

# Resources, Utilities and Influences

## [How to Manually Load Firmware](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)

* Hold right trigger while connecting via USB
    * Mount "CRP DISABLD" will appear
    * cat new_firmware.bin > /mount/CRP\ DISABLD/firmware.bin
    * eject CRP DISABLD
* Used to load old firmware on controller to monitor update process via Steam
* Will be used to load custom firmware on to read/write EEPROM if necessary?

## [Teardown of Steam Controller](https://www.ifixit.com/Teardown/Steam+Controller+Teardown/52578)

* Picture shows main processor is LPC11U37F/501

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

Note that in a new project under Project Properties -> C/C++ Build -> Settings -> Build Steps -> Post-build steps, 
 the commands necessary to generate a bin file exist, but are commented out by
 default.

#### .bin File Layout

* Starts with vector table
    * From 20.7 of UM10462: The reserved ARM Cortex-M0 exception vector location 7 (offset 0x0000 001C in the vector table) should contain the 2’s complement of the check-sum of table entries 0 through 6. This causes the checksum of the first 8 table entries to be 0. The bootloader code checksums the first 8 locations in sector 0 of the flash. If the result is 0, then execution control is transferred to the user code.

* See Section 24.3.3.4 (Fig 24-80) of UM10462 for layout of vector table
    * 0x0000 00004 - Entry point to code in ARM Cortex-M0 vector table. See 20.15.1 of UM10462
    * 0x0000 0001C - Vector location 7. Must contain 2's complement of vector table entries 0-6. See 20.7 or UM10462

* Endianness
    * Certain sections default to little endian
    * Other sections are configurable
    * TODO: Where is this stated in UM10462? And what are these sections?

#### [LPC Image Checksums](https://community.nxp.com/thread/389046)

Note that in a new project under Project Properties -> C/C++ Build -> Settings -> Build Steps -> Post-build steps, 
 the commands necessary to generate a checksum for a bin file exist, but are commented out by
 default.

* Checksum is stored at 0x1C for Cortex-M based parts
    * checksum -p ${TargetChip} -d "${BuildArtifactFileBaseName}.bin"
* This checksum is calculated from the contents of the vector table, not the whole image (probably why I got away with chaning USB info strings)
    * For more details please see the user manual for the MCU that you are using.
* [Creating checksum manually](https://community.nxp.com/thread/388993)

# Disassembling the LPC11U37 Firwmare

This section details the approaches attempted and used to disassemble the 
 Steam Controller firmware. 

## [pinkySim](https://github.com/greggersaurus/pinkySim)

This is main method used for simulating the firmware. 

* Note: Need to use "make pinkySim" as building unit tests fails...
    * TODO: get to bottom of this.
* ARMv6-M Thumb instruction simulator.
* Forked and working on custom functionality.
    * Logging (--logExe chipType)
        * Creates csv log of all instructions and memory accesses executed during simulation.
        * Added memory table specific to LPC11U37, which would be confusing if logging for a different chip.
            * TODO: Think of how to do this better? chip specified as command line arg?
    * Disassembly
        * TODO
* Using gdb installed by LPCXpresso IDE (On OSX: /Applications/lpcxpresso_8.2.2_650/lpcxpresso/tools/bin/arm-none-eabi-gdb) to connect to simulator

### Simulation Steps

#### Launch Simulator 

The following command launches the emulator with the proper memory map for the 
 LPC11U37F501, has the emulator halt on the first instruction to execute after
 reset and instructs pinkySim to log all instructions executed to a file named
 exeLog{datetimestmap}.csv.

* ./pinkySim --breakOnStart --logExe LPC11U37 --flash 0 131072 --ram 268435456 8192 --ram 536805376 16384 --ram 536870912 2048 --ram 536887296 2048 --ram 1073741824 16384 --ram 1073758208 16384 --ram 1073774592 16384 --ram 1073790976 16384 --ram 1073807360 16384 --ram 1073823744 16384 --ram 1073840128 16384 --ram 1073856512 16384 --ram 1073971200 16384 --ram 1073987584 16384 --ram 1074003968 16384 --ram 1074020352 16384 --ram 1074036736 16384 --ram 1074053120 16384 --ram 1074102272 16384 --ram 1074118656 16384 --ram 1074135040 16384 --ram 1074266112 16384 --ram 1342177280 16484 --ram 3758096384 1048576 firmware.bin
    * Note: --ram 536805376 16384 --flash, but since we need to fill this ROM with the boot ROM code via gdb, this needs to be writable

#### Connect to Simulator

The following command launches gbd (installed by LPCXpresso IDE on OSX at /Applications/lpcxpresso_8.2.2_650/lpcxpresso/tools/bin/arm-none-eabi-gdb), attaches it to the running emulator and sets up register to values to desired states:

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

TODO: What about 0x40048030 and brown-out detect?
TODO: What about 0x10000258 and setting to 8 to indicate proper hw (after EEPROM "read")?

##### Breakdown of Input Arguments

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

#### Simulation Oddities

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

### Simulation Details

This section details different paths from which we may want to simulate the firmware.

[vcf_wired_controller_d0g_57bf5c10.c](./vcf_wired_controller_d0g_57bf5c10.c) is an attempt to turn the simulation results
 into C code with, at least some of, the same functionality as the vcf_wired_controller_d0g_57bf5c10.bin firmware.

#### Initialization

By default, simulation starts at instruction specified via Vector Table RESET entry point.
 
Simulation is allowed to run with minimal intervention (i.e. loops waiting for
 PLLs to lock or other hardware reactions are simulated as needed, pauses are
 made to adjust values "read" from EEPROM). 

Uknown paths are identified to be revisisted lated with further stimuli to
 simulation runs. These are being marked by TODO: UNKOWN PATHS.

Attempts are made being made to identify SRAM0 memory usage.

#### Exceptions 

This section details attempts to simulate certain exception paths. This is being 
 pursued as Reset/Init path eventually called WFI instruction. This implies
 interrupts occurring is a necessary part of system boot (i.e. either successful
 connection or shutdown due to connectin timeout).

Simulating an exception can be achieved by setting the PC register to the 
 instruction specified in the Vector Table. However, keep in mind the nuances
 of how IRQ actually work and that simply setting the PC will allow you to 
 simulate the interrupt accurately, but may be desctructive in terms of picking 
 back up where the main thread code was interrupted. 

According to 24.3.3.6.1 xPSR, PC, LR, R12, R3, R2, R1 and R0 are saved upon
 interrupt and restored upon exit. Save and restore these values if you want to
 pick up the main thread after simulating an interrupt.

Also, according 24.3.3.6.1 LR is set to EXC_RETURN upon interrupt entry. Thus a
 bx to LR (assuming no further stack pushes (without matching pops) to change 
 LR), will indicate an interrupt exit. In short, set LR to a know instruction 
 (maybe a WFI instruction or something that will cause an emulator break) before
 changing the PC to the interrupt handler so that you know when the interrupt 
 handler is exiting.

## FirmwareParser.py

This is only being used to display the Vector Table.

Original idea was to create disassembler that can recreate assembly file, 
distinguishing data versus instructions by evaulating code and all possible
branches. 

In the end this was a larger undertaking than expected. It would make more 
sense to leverage pinkySim's ability to decode instructions and their 
behavior to do this, as opposed to starting from scratch.

## [Reverse Engineering for Beginners](https://github.com/dennis714/RE-for-beginners)

Have not made much use of this yet. 

Free book on how to reverse engineer code.

## [Radare](http://www.radare.org/r/)

This proved to not be particularly useful for raw stripped binary.

* [Steep learning curve](https://www.gitbook.com/book/radare/radare2book/details)
* Has option to disassemble ARM, but how exactly does it handle ARMv6-M?
    * Does not automatically work with firmware.bin as input
    * What about #if 0  section iwth armv6 options?
* With have firmware binary format (i.e. no code/data sections information) this only helps so much. 
    * Simulation with pinkySim can be utilized to possibly rebuild this section information and then revisit this tool?

## [Fracture](https://github.com/draperlaboratory/fracture)

This proved to not be particularly useful for raw stripped binary.

* Decompiles to LLVM intermediate representation
    * It looks like this does not work with binary file format with no code/data sections information.
    * Simulation with pinkySim can be utilized to possibly rebuild this section information and then revisit this tool?

## LPCXpresso

This proved to not be particularly useful for raw stripped binary.

* Disassemble axf https://community.nxp.com/thread/388997.
    * It looks like this does not work with binary file format with no code/data sections information.

## [Online Disassembler](https://onlinedisassembler.com/odaweb/)

This proved to not be particularly useful for raw stripped binary.

* Good source to punch individual instruction in quickly
* Make minor mods to instructions to verify understanding of how they are being decoded and behaving (i.e. LDR)
* Finicky interface
    * Might work better if file is uploaded instead of copy paste
* Without code/data section information tool tries to decode everything
    * Simulation with pinkySim can be utilized to possibly rebuild this section information and then revisit this tool?

## [objdump](https://sourceware.org/binutils/docs/binutils/objdump.html)

This proved to not be particularly useful for raw stripped binary.

* ./arm-none-eabi-objdump -b binary -D vcf_wired_controller_d0g_57bf5c10.bin -m arm attempts to disassemble binary file
* With have firmware binary format (i.e. no code/data sections information) this only helps so much. 
    * Simulation with pinkySim can be utilized to possibly rebuild this section information and then revisit this tool?

## [ARMu](http://pel.hu/armu/)

Have not need to look into this much yet.

* For ARMv5TE, but might be worth understanding approach if this works from binary

## [Capstone](http://www.capstone-engine.org/)

Have not need to look into this much yet.

* Worth looking into?

# Understanding the Hardware

See [Luna_maiboard_V000456-00_rev3_pinout.xlsx](./Luna_maiboard_V000456-00_rev3_pinout.xlsx) 
 for details on the pinouts of the main chips on the Luna mainboard V000456-00 rev3 hardware.
