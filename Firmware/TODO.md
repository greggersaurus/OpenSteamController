# [Firmware](./) TODO

The custom firmware Subproject is at a good starting point. Most of the primary
 controller peripherals have drivers and example usage. However, two peripherals
 have still not been integrated. 

1. The Radio Chip (i.e. nRF51822), which would allow for wireless communications 
    1. This could possibly allow for expanding the Nintendo Switch Controller build to function wirelessly
    1. This could allow for pointless/silly applications such as turning the controller into a bluetooth speaker that uses the haptics as speakers
        1. I have not actually done the math to check if we can get good enough rates for this to be worth it even purely for amusement
    1. Next steps
        1. Dig into using the UART interface to enact DFU and get custom code running on the Radio Chip
            1. WARNING: I do not know if Valve's wired FW update process will program this chip if it gets erased... Exploring this may leads to wireless function being bricked on your hardware
2. The Gyro Chip (i.e. MPU-6500), which allows for tracking how the controller moves (i.e. motion controls)
    1. It would be neat to see this data in action, even just with the DEV_BOARD_FW build printing status
    1. This could be used to improve the Nintendo Switch controller build
        1. Note that additional work would need to be done to reverse engineer the Pro Controller Wired protocol as the Power A has no gyro
            1. Note it does seem possible to do this (I may have previously come to the conclusion it was not). Look up videos of the The Exlene wired USB controller for Nintendo Switch for proof/example


# Notes

Below are less verbose notes on items I would like to dig into. These are a bit
 terse and may not be worded well. Please ask if you have any questions:

1. Faux Switch Controller
    1. Add startup jingle...?
        1. Mario theme??

1. Step through all usages and make sure they are consistent
    1. Are we using {arg_name} to distinguish arguments?

1. motion.c
    1. Add support for interfacing with MPU-6500 Six-Axis (Gyro + Accelerometer)
    1. Accumulate all relevant datasheets
1. radio.c
    1. Add support for interfacing with nRF51822 Radio Chip
    1. Accumulate all relevant datasheets
        1. Make sure we get enough info to run DFU and (hopefully) dump what is currently programmed onto Radio Chip
        1. Should be able to verify by seeing this change when switching to Beta Firmware with BLE support
        1. Will Steam be able to fix system where FW on Radio Chip is corrupted?
            1. How "safe" do I need to be here...
1. Building
    1. Test Release build and move away from working with Debug
        1. Debug isn't buying us anything on this platform anyways, right?
    1. Better build solution?
        1. Docker?
        1. Try with latest tools (IDE) from NXP?
    1. Consider C++ instead of C?
1. Documentation
    1. Create videos to illustrate custom firmware?
        1. Loading new FW
        1. Dev Board demo
        1. Wired Nintendo Switch controller demo
    1. READMEs for lpcexpresso projects
        1. Details on where lpc project came from
        1. Intention and requirements (i.e. lpc project) for OpenSteamController project
    1. Make sure all functions have usage 
1. haptic.c
    1. Check range on input arguments
    1. playHaptic() should copy data?
    1. Dig into infinite pulse on haptic bug
        1. There was a bug where Note was pointing to bogus data that resulted in a interrupt that never seemed to end
            1. Need bounds check on how interrupt variables are setup to stop this from happening???
1. init.c
    1. Verify values for OscRateIn and ExtRateIn
    1. Evaluate if (duration of?) busy wait loop in stage1Init can/should be changed
    1. Use #defines for GPIOs?
1. led_ctrl.c
    1. Command to allow LED to blink
        1. Use another counter to make interrupt driven?
    1. Revisit error function...
1. usb.c
    1. Make regression tests (i.e. testPrint)
        1. Buffer overflow related
        1. __disable_irq() related (i.e. ADC IRQs causing double prints or data loss...)
        1. Can we make input related ones
            1. Think through but just fixed where system would loop through rx buffer due to bug in wrapIdx logic
    1. Try to fix CDC UART overflow issues (can we without rewriting USB ROM code though...?)
        1. The issues stems from the fact that USB ROM code ACKs data packets before calling callbacks
            1. This means that in callback if we don't have room, we cannot tell the host to wait
            1. Other work arounds have not worked yet (i.e. disabling EP when buffer is full and reenabling later)
            1. Maybe we can hijack IRQ handler??
            1. Or maybe we look into not using ROM code and writing out own (see polling example fix for this issue: http://www.eevblog.com/forum/microcontrollers/usb-cdc-_flow-control_/)
        1. Reproduce issues by pasting entire jingle into console no longer locks it up, but it looks like some command get mangled...
1. test.c
    1. Clean up and add more tests
    1. Create (at least manual) test procedure?
1. trackpad.c
    1. Dig into oddities in X/Y data (~5 degree clockwise rotation... or maybe skew is better description...)
    1. Understand sample [18]. How, why and when could this be used.
        1. Official FW is sampling this, but how is it using it...?
    1. Add support for multi touch (could this be useful or at least "neat")?
        1. See notes in trackpadGetLastXY() about how trackpad seems to support this
    1. Add ability to sample ADCs in low power mode
1. eeprom_access.c
    1. Implement writing function
1. mem_access.c
    1. Implement write command
1. command.c
    1. Replace linear search with something more efficient?

