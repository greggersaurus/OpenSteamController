# Reverse Engineering

The work in this directory focuses on deconstructing the firmware and hardware
 of the Steam Controller. The understanding gained from this is the basis for
 the other work being done in this project. 


# Understanding the Hardware

See [Luna_maiboard_V000456-00_rev3.md](./Luna_maiboard_V000456-00_rev3.md) 
 for information regarding the Steam Controller hardware pertaining to 
 Luna_mainboard V000456-00 rev3.


# Understanding the Software

See [LPC11U37F_Software.md](./LPC11U37F_Software.md) for information regarding
 the software running on the LPC11U37 main/master processor.


# TODO

This section is a running list of priorities to focus on in hopes of reaching
 goal of modifying haptics jingle. This will be updated as tasks are completed
 and more knowledge is gained about the Steam Controller.

1. Push through Right haptic init via gdbCmdFile, then fix pinkysim simplified logging?
    1. Go back and add landmarks for functions being called, etc. so we can find our way back through later (i.e. if we need to try a different path)
        1. Add echos and fnc0x...() names and '{''}' to show how we simulate through certain function calls
        1. Echo after each watch point?? (sometimes screen is just filled with watchpoints and we don't know where we are in execution...)
    1. Need to simulate PINT3 IRQ?
1. Update pinkySim simplified C output to handle pop's where registers are not restored (pop should still occur for value, but it will be lost, right?)
    1. Seems that we need to handle special case of writing SP (reg13) and how that might wipe out values pushed onto it...
    1. Perhaps solution is to remove each regs entry being an array and instead make a stack array that handles holding data dynamically
        1. i.e. on push store value/string and const specifier to stack, on pop write those to register, on SP adjust... Do anything? (i.e. use SP value as way of calculating index into SP array?)
        1. Make sure to have side-by-side test to see how simplified.c test output changes... (i.e. did we miss anything before or screw up anything with this *fix*)
    1. Also think about special cases of setting LR and PC and other special registers and how we want to handle that??
1. Check EEPROM read values at 0x800 and 0x600 on both controllers...
    1. Update gdbCmdFile with values (see TODOs)
1. Things to try immediately (see farther below for more details)
    1. What if 0x100010d8 is >= 5?
        1. Check main loop
            1. This is leading us down a new path. Keep digging!
        1. Check USB_Configure_Event and USB_Resume_Event
    1. Revisit gdbCmdFile watchpoints
        1. Rather than settings and removing watchpoints one by one, can we watch multiple and react differently based on pc?
            1. This will make repeated steps that need intervention (i.e. skipping WriteEP or making it through SPI transactions) cleaner (less code duplication)
        1. Can gdb command files have functions and loops?
            1. Make functions to visit different IRQs and callbacks (so we are not constantly removing or changing code...)
    1. Consider: How do we get to path where startup tune is sent to haptics?
        1. Connecting to Linux causes jingle (just power over USB does not)
            1. USB HID related exchange causes this to happen?
        1. Compare usbmon connection capture to other HID devices?
            1. Any special instruction that could point to jingle playing?
        1. Note that trackpad being powered and Right being in mouse mode with haptic feedback occurs...
            1. What is holding this up from happening if we seem to be in main loop and may even try to WriteEP for EP3...?
    1. Look into getting details on fields in 0x10001100 struct EP3UpdateMsg
        1, Look into what this contains when controller is connected with Valve FW (and no driver/Steam in new ubuntu vm)
            1. Holes where trackpad data exist or accelerometer??
    1. What about functions checking periph related to EP1 and EP2?
        1. Are these not generating messages because nothing was perceived to change? Or is it just a different USB HID interface (i.e. not interrupt)?
        1. Try re-running simulation with different inputs (i.e. related buttons pressed...)
    1. Send 64 bytes message defined in https://gitlab.com/Pilatomic/SteamControllerSinger/blob/master/main.cpp to EP3_HID_GetReport (or is it SetReport??) Or is it EP1 or EP2?
        1. We know from SteamControllerSinger that this causes haptics to vibrate
            1. Info on how to communicate with haptics
            1. Info on memory and setup related to haptics/SPI?
    1. Is AD6 battery gauge/power?
        1. Try putting dead (or nearly dead) battery in and checking
            1. But maybe GPIOs, etc. need to be set differently?

    1. (Fresh look) Make sure all valid paths in all enabled IRQs are covered
        1. PINT3!!
            1. Sets 0x1000024a to non-zero when complete?
        1. PINT0
        1. PINT1
        1. PINT2
        1. ADC
        1. I2C0
        1. CT16B0
        1. USART0 
        1. CT32B0
    1. Decode functions from new sim data now that variable changed by SysTick handler moves us forward
        1. Don't lose track of other variables checked in previous wfi path. They might still matter...
    1. Dev Board comms with Radio Chip via UART
        1. Monitor PIO1_5 and (anything else we can watch?) to check for change after sending same mesages?
    1. Monitor GPIO1_5 when RF dongle is plugged in or not
        1. Scope with official FW?
    1. USB related callbacks (at leats resume one...)
        1. Seems some variable related to wfi loop in main path may be related to USB (and maybe changed in callback functions?)

1. Consider converting project(s) to CPP
    1. What version does compiler support?
        1. constexpr goodness?
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
        1. Should focus be Focus on USB_VBUS = 0 instead of USB_VBUS = 1?
            1. For USB_VBUS = 1 case jingle seems to be triggered by USB activity (i.e. driver probing/enumerating)
            1. For USB_VBUS = 0 case jingle seems to be triggered by communicating with RF chip
                1. Note that once controller has synced with RF dongle, plugging in USB does not change USB_VBUS...
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
