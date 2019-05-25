# TODO

These items relate to better understanding how the official Steam Controller
 Firmware works. The idea is to use this knowledge to help with adding features
 to the other Subprojects:


1. Jingle Sim related
    1. Details on CT32B0 ISR based on simulation results?
    1. Details on each function call
1. Clean, clean, clean
    1. gdbCmdFile, gdbCustomCmds, gdbOldRef
    1. PINT3 dump
    1. Cleaning other ISRs and looping paths so that they make when I come back after I have forgotten everything.
    1. Make sure all known functions have details in .h file

1. Send 64 bytes message defined in https://gitlab.com/Pilatomic/SteamControllerSinger/blob/master/main.cpp to EP3_HID_GetReport (or is it SetReport??) Or is it EP1 or EP2?
    1. We know from SteamControllerSinger that this causes haptics to vibrate
        1. Info on how to communicate with haptics
        1. Info on memory and setup related to haptics/SPI?
1. Is AD6 battery gauge/power?
    1. Try putting dead (or nearly dead) battery in and checking
        1. But maybe GPIOs, etc. need to be set differently?

1. Documentation
    1. Update http://media.steampowered.com/controller_config//firmware/vcf_wired_controller_d0g_57bf5c10.bin with datasheets, etc.
1. Make TODOs for maybe in the future getting more details on following IRQs
    1. PINT0
    1. PINT1
    1. I2C0
    1. USART0 
1. Decode functions from new sim data now that variable changed by SysTick handler moves us forward
    1. Don't lose track of other variables checked in previous wfi path. They might still matter...
1. Dev Board comms with Radio Chip via UART
    1. Monitor PIO1_5 and (anything else we can watch?) to check for change after sending same mesages?
1. Monitor GPIO1_5 when RF dongle is plugged in or not
    1. Scope with official FW?
1. USB related callbacks (at leats resume one...)
    1. Seems some variable related to wfi loop in main path may be related to USB (and maybe changed in callback functions?)

1. Consider latest firmware update from Valve (new firmware to enable BLE for new Steam Link app)
    1. Look at what has changed?
    1. Will new tooling help to decompose the new firmware faster?
    1. Is this a distraction not worth digging into right now?
        1. Majority of functional behavior from vcf_wired_controller_d0g_57bf5c10.bin should remain unchanged (even if underlying assembly changes). 
        1. Once functionality is fully understood, applying that understanding to newer firmware may be quicker than "starting over" with new firmware?
1. Simulation to C Conversion
    1. Run and decode non-straightforward paths
        1. See callback functions and IRQs with TODOs in [vcf_wired_controller_d0g_57bf5c10.c](./vcf_wired_controller_d0g_57bf5c10)
    1. Continue running straightforward (i.e. from reset vector entry point) sim (with USB_VBUS = 1)
        1. Looping on checking some variables with wfi (USART and USB related maybe?)
    1. Questions to consider
        1. What can we run via DevBoard firmware?
            1. What if we send USART messages to Radio chip and monitor GPIO1_5?
        1. Where is firmware for Radio chip?
            1. How is this updated by Valve?
                1. Check latest vcf as we know there was an update with latest firmware...?
    1. Low priority sections that we could have better understanding of
        1. ADC Math
            1. Simulate with AD0/2 and AD1/3 have realistic accumulation results?
                1. Low priority as figuring out details on math used to convert ADC samples is not super important to end goal.
        1. CT16B1 (and relation to ADC DR6 reading?)
            1. Maybe more apparent when we simulate IRQ for CT16B1?
        1. Button GPIO states checked during init
            1. What does it really affect if certain buttons are depressed on power up? 
                1. Special more select or something maybe?
1. Understand USART/Radio communications protocol?
    1. Try to reverse engineer protocol to leverage comms with dongle??
1. List more details about hardware [Luna_maiboard_V000456-00_rev3.md](./Luna_maiboard_V000456-00_rev3.md)
    1. Other processors (i.e. wireless comms chip, gyro, haptics chip)
    1. See if we can get information on each pin (focus on LPC chip)
    1. What is ADC connected to (specifically Channel 6)
        1. This may help with understanding some of these GPIOs...
    1. Find out more on these GPIOs (i.e. Ohm it out, test using custom firmware)
        1. PIO1_10
        1. PIO1_7 
        1. PIO0_19 
        1. PIO0_2
        1. PIO1_28
        1. PIO0_7
        1. PIO0_22 (AD6) What does this read...?
        1. PIO1_28 and PIO1_4 (USART/Radio chip related?)
        1. PIO1_5 (USART/Radio chip related. PINT2).
            1. What is state of this? Changes if we send messages via UART?
            1. What does trace lead to (ohm it out...)
    1. Add Chart/Table to label how hardware peripherals are used?
        1. CT16B1 = driving Steam Controller LED
        1. CT16B0 = used as timer for a delay during init. (That it?)
        1. SSP0 = CS ?? = Left Haptics, CS ?? = Right Haptics 
1. Decompose EEPROM dumps
    1. Captured from two different controllers to isolate some differences
    1. Change settings to isolate other differences
1. Can we (continue to) make pinkySim logging better 
    1. More automation or second steps for cleaning .c log file?
1. Look into wireless chip
    1. Is same one on dongle?
        1. If so can I use dongle as dev board?
            1. If not should I order a dev board (https://www.nordicsemi.com/eng/Products/nRF51-Dongle)

