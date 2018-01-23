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

1. Finish decoding sim straightforward sim
    1. Pay attention to ADC interrupt...?
1. Run and decode other paths
    1. Interrupts (Note that GPREG1 is set to 1 in init2()...)
        1. ADC interrupt seems potetnially important...
    1. Callbacks
        1. USB_Configure_Event at 0x00004e59
        1. HID_GetReport at 0x00005af1
        1. HID_SetReport at 0x00009565
1. Can we (continue to ) make pinkySim logging better 
    1. Update RevEng documentation
    1. Add details on each register (at least name? but maybe what changed since last write/access?)
    1. Add diffs when writing a register (i.e. what was it before write, so we can quickly see bits getting set, etc.)
1. Verify communication with haptics occurs via SPI (SSP0?) from LPC11U37F
    1. It seems SPI is interface enabled on haptics (ohmed out ~470K resistor between pins 12 and pin 13 on Cirque 1CA027). 
    1. Simulate Get/Set report for HID to see how/what it sends to haptics and if SSP0/1 is being used.
    1. Why is there no SSP0/1 setup if that is how communications to haptics are occurring? 
    1. Are clocks enabled for SSP0/1 in sim? 
    1. Stay cautious that this might be the wrong path here...
1. Decompose EEPROM dumps
    1. Captured from two different controllers to isolate some differences
    1. Change settings to isolate other differences
1. List more details about hardware [Luna_maiboard_V000456-00_rev3.md](./Luna_maiboard_V000456-00_rev3.md)
    1. Other processors (i.e. wireless comms chip, gyro, haptics chip)
    1. See if we can get information on each pin (focus on LPC chip)
1. How is communication occurring with radio chip?
