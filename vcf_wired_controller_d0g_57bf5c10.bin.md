# Introduction

The purpose of this document is to function as an overview of the results of 
 simulating vcf_wired_controller_d0g_57bf5c10.bin. See 
 vcf_wired_controller_d0g_57bf5c10.c for finer grain details of simulation 
 results breakdown. This document will also contain planning information for
 what is yet to be simulated and possible changes results parsing approach. 

The overall goal here is to use the simulation results to isolate code areas to 
 modify for controlled firmware behavior changes, as well as for creating 
 [custom firmware](https://github.com/greggersaurus/SteamControllerCustomFirmware) 
 to run on the Steam Controller (i.e. use the Steam Controller almost as a 
 development platform).

Note that [pinkySim](https://github.com/greggersaurus/pinkySim) is being used 
 to obtain all simulation results.

# Execution Breakdown

Below are overviews for various simulation paths and entry points of the firmware.

## Init

Simulation starts at instruction specified via Vector Table RESET entry point.
 
Simulation is allowed to run with minimal intervention (i.e. loops waiting for
 PLLs to lock or other hardware reactions are simulated as needed). 

Uknown paths are identified to be revisisted lated with further stimuli to
 simulation runs. These are being marked by TODO: UNKOWN PATHS.

Attempts are made being made to identify SRAM0 memory usage.

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

TODO

###  Interrupt 22 - IP_USB_IRQ

This is the interrupt generated for the USB controller. It is assumed this
 handles a lot of possibilities, but may handle some initial USB configuration
 as well as sending of the jingle to the haptics once USB coniguration is
 fully complete.

#### To Simulate

TODO

#### Simulation Result Details

TODO

## NMIs

TODO: It may be necessary to try and simulate Non Maskable Interrupts as some
 configuration may be taking place in these routines. Need to read documentation
 to understand how to properly simulate these (if possible).

# RAM Layout

See RAM LAYOUT comment section for tracking of how various portions of SRAM0
 are used and what they may possibly represent.

