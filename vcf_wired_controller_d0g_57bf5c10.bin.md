# Introduction

Purpose of this document is to encapsulate breakdown of 
 vcf_wired_controller_d0g_57bf5c10.bin based on simulation results. This will be
 used to isolate code areas to modify for controlled firmware behavior changes, 
 as well as for creating [custom firmware](https://github.com/greggersaurus/SteamControllerCustomFirmware) 
 to run on the Steam Controller.

Note that [pinkySim](https://github.com/greggersaurus/pinkySim) is being used 
 to obtain these results.

# Execution Breakdown

Below are details for particular simulated paths through the firmware.

## Init

In this simulation run the system is run from reset with no external input
 (except steps necessary to simulate expected hardware unit reactions). Possible
 triggering of IRQs are ignored. Parsed from runLogFile_00000000001494552728.csv
 (not revisioned due to log size, but noted for now to track progress).

#### To Simulate

* Run from reset until WFI
    * See README for details on how to have gdb get past situations where hardware responses are expected.
* TODO: what about after WFI?
    * Initial force through this seems that 0x1000025d, 0x1000025c and 0x10000200 were checked to be 0 or 1 (booleans) and eventually led down what looks like possible shutdown path

#### Simulation Result Details

See init() in vcf_wired_controller_d0g_57bf5c10.c.

## IRQs

This section details attempts to simulate certain IRQ paths. This is being 
 pursued as Init path is mostly simple setup and does seem to touch some code paths
 that would be more interesting (i.e. playing jingle via haptics).

TODO: Need to read up more on IRQs to make sure I am simulating them correctly
 (if that is possible with pinkySim). Add bulleted list of steps to simulate:
* Enterting an exception
    * When an exception other that reset preempts an instruction stream, the processor automatically saves key context information onto the stack, and execution branches to the code pointed to by the corresponding exception vector.
        * When pushing context to the stack, the hardware saves eight 32-bit words, comprising xPSR, ReturnAddress, LR (R14), R12, R3, R2, R1, and R0
        * How is xPSR changed
            * IPSR is changed to number of exception being processed
* Returning from an exception
    * The Exception Return Link, a value stored in the link register on exception entry, determines the target of the exception return.
    * TODO: See B1.5.8 Exception return behavior

### Interrupt 19 - CT32B1

This is 32-bit counter that is setup before the system system goes with sleep
 with WFI (Wait for Interrupt) command. The working theory is that the controller
 has completed all setup and it waiting for a connection via USB or wireless.
 If this timer expires and interrupts the system before a connection is established
 the controller powers down. Part of power down is to send out a shutdown jingle 
 to the haptics.

#### To Simulate

* Run from reset until WFI
    * See README for details on how 
* Set PC to IRQ 19 entry point
    * set $pc = 0x000001b8

* Set ISPR to 19?
    * TODO
* Set LR to WFI instruction?
    * set $lr = 0x00000f52

* Simulate interrupt on MR0?
    * set {int}0x40018000 = 1
* Make sure GPREG1 is not clear to show setup has occurred??
    * set {int}0x40038008 = 1
* Clearing of MR0 interrupt?
    * set {int}0x40018000 = 0 at instruction 0x6180
        * This just causes IRQ to exit and WFI
* IRQ checks 0x10000848, is this set by USB IRQ firing?

#### Simulation Result Details

* Read PMU 0x40038008 (GPREG1) and check if register is 0
    * If GPREGP1 register is 0
        * Reset MR0INT by writing 1 to 0x40018000
        * Check if 0x10000200 is 0
            * Is 0
                * TODO
            * Is not 0
                * TODO
    * If GPREGP1 register is not 0
        * TODO

###  Interrupt 22 - IP_USB_IRQ

This is the interrupt generated for the USB controller. It is assumed this
 handles a lot of possibilities, but may handle some initial USB configuration
 as well as sending of the jingle to the haptics once USB coniguration is
 fully complete.

#### To Simulate

TODO

#### Simulation Result Details

* Read PMU 0x40038008 (GPREG1) and check if register is 0
    * If GPREGP1 register is 0
        * Read 0x40080000 and check if bit 8 SETUP is set
            * If SETUP is set 
                * To Sim: set {int}0x40080000 = 0x00010180
                * Clear bit 29 of 0x20004000 and 0x20004008
            * Check if an enabled interrupt has triggered
                * Enabled interrupt has triggered
                    * Check if 0x40080000 Bit 26 DRES_C (device received bus reset)
                        * It is set
                            * TODO
                        * It is not set
                            * Check if 0x40080000 Bit 24 DCON_C (device status connect change)
                                * It is set
                                    * TODO
                                * It is not set
                                    * Check if 0x40080000 Bit 25 DSUS_C (device status suspend change)
                                        * It is set
                                            * TODO
                                        * It is not set
                                            * Check if enabled interrupt 9 EP4IN triggered 
                                                * It triggered
                                                    * TODO
                                                * It did not trigger 
                                                    * Check if enabled interrupt 30 FRAME_INT triggered 
                                                        * It triggered
                                                            * TODO
                                                        * It did not trigger 
                                                            *  Check if 0x200040f4 is 0
                                                                * It is 0
                                                                    * Exit ISR?
                                                                * It is not 0
                                                                    * TODO
                * Enabled interrupt has not triggered
                    * TODO
    * If GPREG1 register is not 0

## NMIs

TODO: It may be necessary to try and simulate Non Maskable Interrupts as some
 configuration may be taking place in these routines. Need to read documentation
 to understand how to properly simulate these (if possible).

# RAM Layout

This section exists to track RAM usage. The hope is to gain an understanding
 of how RAM is being used for scenarios in which is it used to communciate
 across IRQs or subroutines. 

* TODO: Starting stack pointer. Pay attention to how this changes through execution.
* TODO: Details on memory setup in init and called from IRQs?
* 0x10000200 
    * Initialized to specific values starting at step 33912
    * Possibly checked by CT32B1 IRQ
    * Checked by main path after WFI - Used to signal shutdown?
* ...
* 0x1000025c 
    * Checked by main path after WFI - Used to signal shutdown?
* 0x1000025d 
    * Checked by main path after WFI - Used to signal shutdown?
    * Presumably some interrupt handler sets this to something after some setup has occurred?
* 0x10001c1c - Initialized to specific values starting at step 33912
* 0x10001c20 - Initial Stack Pointer? (counts down?)
* 0x200040f4 may be checked by IP_USB_IRQ to see if it is 0 or not

# TODO

This section encapsulates thoughts and ideas based on current progress. It is 
 messy and mostly here so I can remember where I left off, or to pick up an
 idea I did not have bandwidth to pursue at the time. YMMV.

* Need to reevaluate and clean up this section... It's getting a bit cluttered...
    * Want to move steps for simulating IRQs out of TODO?

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
            *  58103,   alignedMemWrite: IOCON (0x40044018 = 0x00000001),    44609 -> PIO0_6 Set pin function to USB CONNECT
            *  58122,   alignedMemWrite: IOCON (0x400440a4 = 0x00000002),    44626 -> PIO1_17 Set pin function to RXD
            *  58141,   alignedMemWrite: IOCON (0x400440a8 = 0x00000002),    44643 -> PIO1_18 Set pin function to TXD
            *  63368,   alignedMemWrite: USB (0x40080000 = 0x00000000),      48920 -> 
            *  63401,   alignedMemWrite: USB (0x40080008 = 0x20004000),      48938 -> 
            *  63407,   alignedMemWrite: USB (0x4008000c = 0x20004240),      48941 -> 
            *  64106,   alignedMemWrite: USB (0x40080018 = 0x00000000),      49486 -> 
            *  64110,   alignedMemWrite: USB (0x40080014 = 0x00000000),      49488 -> 
            *  64116,   alignedMemWrite: USB (0x4008001c = 0x000003ff),      49491 -> Double buffer setup for EP1-4 IN and OUT
            *  64122,   alignedMemWrite: USB (0x40080020 = 0xc00003ff),      49494 -> Clear EP0-4 IN and OUT interrupts
            *  64128,   alignedMemWrite: USB (0x40080024 = 0x800003ff),      49497 -> Enable DEV_INT_EN and EP_INT_EN for EP0-4 IN and OUT
            *  64132,  alignedMemRead: USB (0x40080000 --> 0x00000000),      49499 -> 
            *  64136,   alignedMemWrite: USB (0x40080000 = 0x00000080),      49502 -> 
            *  64155,  alignedMemRead: USB (0x40080000 --> 0x00000080),      49511 -> 
            *  64159,   alignedMemWrite: USB (0x40080000 = 0x00000080),      49514 -> 
            *  64163,  alignedMemRead: USB (0x40080000 --> 0x00000080),      49516 -> 
            *  64168,   alignedMemWrite: USB (0x40080000 = 0x00000080),      49520 -> 
            *  65135,  alignedMemRead: USB (0x40080000 --> 0x00000080),      50237 -> 
            *  65140,   alignedMemWrite: USB (0x40080000 = 0x00010080),      50241 -> 
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
            *  65256,   alignedMemRead: private peripheral bus (0xe000e414 --> 0x00400000),  50319 -> Read Interrupt Priority Register 5
            *  65260,    alignedMemWrite: private peripheral bus (0xe000e414 = 0x00400000),  50322 -> Set IP_USB_IRQ to one below highest priority
            *  65283,   alignedMemRead: private peripheral bus (0xe000ed20 --> 0x00000000),  50343 -> Read System Handler Priority Register 3 
            *  65287,    alignedMemWrite: private peripheral bus (0xe000ed20 = 0x00400000),  50346 -> Set Priority of system handler 14, PendSV to one below highest priority?
                * TODO: find section where we know what priorities mean
            * 307241,    alignedMemWrite: private peripheral bus (0xe000e280 = 0x00080000), 281832 -> ICPR: Clear interrupt pend of interrupt 19 (0-based) -> CT32B1 -> CT32B1 interrupt
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

        * TODO: Need to understand exception entry better. I think I am doing something wrong, or at least inconsistent. See B1.5.6.  
            * What is setup? How are registers and stack affected?
            * What is case for exiting exception (is it always the same)?

        * Simulation thus far could be somewhat inaccurate if interrupt handler code changes states and such... which might explain why certain mods don't seem to have any effect...
        * Continue down path of simulating interrupts?
            * Results thus far seem to be lacking something? Could be PenSV or something else happening I am not thinking of? Read up on this?
        * Try forcing down other paths in IRQs (32-bit counter and USB)?
        * Create memory map so when system tries to access memory and doesn't go down path because it's zero, we know what it might be looking for?
        * Create better execution overview so we understand what we might be missing and what system is doing?
            * It seems like we are missing some key setup or configurations somewhere...
    * Interrupts to simulate
        * PendSV?
            * TODO: When would this occur?
            * TODO: What address would this jump to?
            * TODO: does simulating this even make sense?
        * Interrupt 22 - IP_USB_IRQ
            * Setup
                * Step until interrupt is enabled and setup (code is in counting loop waiting for interrupt to occur?)
                    * stepi 50700
            * Called for multiple reasons (i.e. EP1-4 IN/OUT or FRAME_INT or DEV_INT)
                * DEV_INT - Device status 
                    * set {int}0x40080020 = 0x80000000
                    * This does some checking of memory, but the proceeds to wait for connection and times out and sleeps (same code as if no interrupt fires)
            * PC Start Address for IRQ can be found at code offset 0x98
                * For vcf_wired_controller_d0g_57bf5c10.bin this is 0x000001e9
                    * set $pc = 0x000001e8
        * Interrupt 21 - USART
            * Probably don't care about this so much
            * PC Start Address for IRQ can be found at code offset 0x94
                * For vcf_wired_controller_d0g_57bf5c10.bin this is 0x000001d5
                    * set $pc = 0x000001d4
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
