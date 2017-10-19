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

### Interrupt 19 - CT32B1

This is 32-bit counter that is setup before the system system goes to sleep
 with WFI (Wait for Interrupt) command. In this state the interrupt controller
 simply clears MR0INT and sets RAM address 0x1000025c. I believe setting of
 0x1000025c is used to communicate to main thread that irq occurred to increment
 countdown before giving up on connection. 

Note there is also a different path based on GPREG1 being non-zero. Since there
 does not seem to be a path in the main thread before the WFI mentioned above,
 I believe this is code for using CT32B1 once controller has been connected to
 or something like that.

#### Simulation Result Details

See Interrupt_19_CT32B1() in vcf_wired_controller_d0g_57bf5c10.c.

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

