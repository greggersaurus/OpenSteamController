# Reverse Engineering

The work in this directory focuses on deconstructing the firmware and hardware
 of the Steam Controller. The understanding gained from this is the basis for
 the other work being done in this project. 


# Understanding the Hardware

See [Luna_maiboard_V000456-00_rev3.md](./Luna_maiboard_V000456-00_rev3.md) 
 for information regarding the Steam Controller hardware pertaining to 
 Luna_mainboard V000456-00 rev3.


# Understanding the Software

See [LPC11U37_Software.md](./LPC11U37_Software.md) for information regarding
 the software running on the LPC11U37 main/master processor.


# TODO

This section is a running list of priorities to focus on in hopes of reaching
 goal of modifying haptics jingle. This will be updated as tasks are completed
 and more knowledge is gained about the Steam Controller.

1. Modify gdbCmdFile with all default register settings and re-run sim and compare if it's any different than what we currently have
2. Verify communication with haptics occurs via SPI (SSP1?) from LPC11U37F
    1. It seems SPI is interface enabled on haptics (ohmed out ~470K resistor between pins 12 and pin 13 on Cirque 1CA027). 
    2. Simulate Get/Set report for HID to see how/what it sends to haptics and if SSP0/1 is being used.
    3. Why is there no SSP1 setup if that is how communications to haptics are occurring? 
    4. Are clocks enabled for SSP0/1 in sim? 
    5. Stay cautious that this might be the wrong path here...
3. Update [Luna_maiboard_V000456-00_rev3.md](./Luna_maiboard_V000456-00_rev3.md)
    1. See if we can get information on each pin.
4. Compare EEPROM dumps between two controllers
5. List more details about hardware 
    1. Other processors (i.e. wireless comms chip, gyro, haptics chip)
6. How is communication occurring with radio chip?
