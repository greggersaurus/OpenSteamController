# LPC11U37F/501 Software

The purpose of this document is to track information regarding the software that
 runs on the LPC11U37F. The LPC11U37F is a 32-bit processor ARMv6-M architecture 
 with 16-bit Thumb ISA and includes Thumb-2 technology.


# Resources 

* [Datasheet](http://www.nxp.com/documents/data_sheet/LPC11U3X.pdf?fasp=1&WT_TYPE=Data%20Sheets&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=pdf&WT_ASSET=Documentation&fileExt=.pdf)

* [User Manual](http://www.nxp.com/documents/user_manual/UM10462.pdf)

* [ISA](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.subset.architecture.reference/index.html)


# Disassembling the Firwmare

This section details the approaches attempted and ultimately used to reverse 
 engineer the Steam Controller firmware running on the LPC11U37.

## [pinkySim](https://github.com/greggersaurus/pinkySim)

This is the primary method used for simulating the firmware and deconstructing 
 its behavior.

* Note: Need to build pinkySim specifically with command "make pinkySim" as building unit tests fails...
    * TODO: get to bottom of this.
* Note: cloning is also messy for me. See updated README for details how I needed to do this on my dev system...
    * TODO: get to bottom of this.
* ARMv6-M Thumb instruction simulator.
* I have forked this repo and am working on custom functionality geared specfically towards the Steam Controller reverse engineering effort.
    * Logging (--logExe chipType)
        * Creates csv log of all instructions and memory accesses executed during simulation.
        * Creates C log of actions, which allows seconday way of visualizing simulation.
        * Currently "LPC11U37" is the only supported chip type
    * Disassembly
        * TODO: Worth spending time on option to out asm after running simulation? (will only cover instructions that were executed during sim)
* Using gdb installed by LPCXpresso IDE (On OSX: /Applications/lpcxpresso_8.2.2_650/lpcxpresso/tools/bin/arm-none-eabi-gdb) to connect to the simulator

### Simulation Steps

#### Launch Simulator 

The following command launches the emulator with the proper memory map for the 
 LPC11U37F501, has the emulator halt on the first instruction to execute after
 reset and instructs pinkySim to log all instructions executed to a file named
 exeLog{timestmap}.csv. exeLog{timestmap}.c is also created, which makes a simple
 attempt at a C-like decomposition of the simulated actions. 

* ./pinkySim --breakOnStart --logExe LPC11U37 --flash 0 131072 --ram 268435456 8192 --ram 536805376 16384 --ram 536870912 2048 --ram 536887296 2048 --ram 1073741824 16384 --ram 1073758208 16384 --ram 1073774592 16384 --ram 1073790976 16384 --ram 1073807360 16384 --ram 1073823744 16384 --ram 1073840128 16384 --ram 1073856512 16384 --ram 1073971200 16384 --ram 1073987584 16384 --ram 1074003968 16384 --ram 1074020352 16384 --ram 1074036736 16384 --ram 1074053120 16384 --ram 1074102272 16384 --ram 1074118656 16384 --ram 1074135040 16384 --ram 1074266112 16384 --ram 1342177280 16484 --ram 3758096384 1048576 firmware.bin
    * Note: --ram 536805376 16384 --flash, but since we need to fill this ROM with the boot ROM code via gdb, this needs to be writable

#### Simulate

The following command launches gbd (installed by LPCXpresso IDE on OSX at /Applications/lpcxpresso_8.2.2_650/lpcxpresso/tools/bin/arm-none-eabi-gdb) and executes the commands in gdbCmdFile:

* ./gdb -ex "source gdbCustomCmds" -ex "source gdbCmdFile"
    * See comments in [gdbCustomCmds](./gdbCustomCmds) for details on gdb user-defined commands to simulate through certain function calls.
    * See comments in [gdbCmdFile](./gdbCmdFile) for details on simulation is doing, and can be configured to do.
    * A binary file named LPC11U3x16kBbootROM.bin is expected to exist (in the current directory). The file should contain the binary dump of 0x1FFF0000 to 0x1FFF4000 from an LPC11U37.
        * This can be obtained from a Steam Controller using custom firmware from the [Development Board](../DevBoard) project. Use read commands to peek at the 16 kB boot ROM.

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

## [FirmwareParser.py](./FirmwareParser.py)

This is only being used to display the Vector Table.

The original idea was to create disassembler that can recreate assembly file, 
distinguishing data versus instructions by evaulating code and all possible
branches. 

In the end this was a larger undertaking than expected. It would make more 
sense to leverage pinkySim's ability to decode instructions and their 
behavior to do this, as opposed to starting from scratch.

## [Reverse Engineering for Beginners](https://github.com/dennis714/RE-for-beginners)

Have not made much use of this yet. 

Free book geared towards beginners on how to reverse engineer code.

## [Radare](http://www.radare.org/r/)

This may be worth learning and using as a supplementary tool (now that time spent
 with pinkySim has give me better understanding of assembly flow). 

Somewhat different approach than pinkySim in that we are looking at assembly and trying 
 to assess actions that could be taken and potential purpose, as opposed to focusing on
 actions taken during simulation and why they were taken (and maybe should not have been).

* [Steep learning curve](https://www.gitbook.com/book/radare/radare2book/details)
    * Videos for learning basics (specifically related to firmware reverese engineering)
        * https://www.youtube.com/watch?v=R3sGlzXfEkU
        * https://www.youtube.com/watch?v=GIU4yJn2-2A
        * https://www.youtube.com/watch?v=iTKra0XD6z4
* Has option to disassemble ARM, but how exactly does it handle ARMv6-M?
    * What about #if 0  section iwth armv6 options?
* How to deal with firmware stripped binary format (i.e. no code/data sections information)?
    * Build up scripting tooling to handle this? (i.e. don't decode instructions we "know" we "cannot reach")
        * Scripting to identify code/data semi-automatically? 
* Can use flow visualization to identify different sections of code to focus on?
    * Allows us to see paths we are not taking (and where they might go), rather than just knowing a branch was not taken.

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

