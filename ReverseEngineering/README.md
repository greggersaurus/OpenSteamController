# Reverse Engineering

Welcome to Reverse Engineering Subproject portion the Open Steam Controller effort.

The work in this directory is the result of trying to understand the hardware
 and its capabilities based on the available resources. In this case the resources
 available were the circuit board itself and the raw binary firmware for the
 controller's main processor (the LPC11U37F).

There is a lot of really neat and useful information captured here (i.e. the
 fact that there is a section of EEPROM where Jingle Data can be stored to
 change the official firmware's default behavior, how the interface to the
 Trackpads works) and the result of these efforts are the basis of many of the
 Subprojects in the Open Steam Controller effort. If anything is unclear, or
 you think I am not drawing proper attention to feature I have unearthed,
 please be sure to let me know.

See the sections below for further details on the data that has been captured
 in regards to the hardware and software, but please note that this is my first attempt
 at reverse engineering. I may have not gone about this in the most efficient
 or understandable manner, but I have done my best to make sure the results
 are captured concisely so that others may benefit from it.


# Understanding the Hardware

This section captures details on the controller hardware (i.e. what pins are
 connected to what peripherals or pins on other chips). This data was sometimes
 obtained simply by using digital multimeter to ohm out connections. Other times
 reverse engineering the firmware, or running tests with custom firmware were
 required to fully understand how the hardware was designed.

See [Luna_maiboard_V000456-00_rev3.md](./Luna_maiboard_V000456-00_rev3.md)
 for information regarding the Steam Controller hardware pertaining to
 Luna_mainboard V000456-00 rev3.


# Understanding the Software

## PinkySim and Ghidra

This section captures details on the software running on the controller
 processors. This data was primarily obtained by using a modified version
 of [pinkySim](https://github.com/greggersaurus/pinkySim), which allowed for
 simulating the main processor (LPC11U37F) and logging relevant actions.
 Verification of different behaviors often required running custom firmware
 to ensure the proper paths were being simulated. A separate effort doing static
 analysis using [Ghidra](https://github.com/NationalSecurityAgency/ghidra) is
 also described.

See [LPC11U37F_Software.md](./LPC11U37F_Software.md) for information regarding
 the software running on the LPC11U37 main/master processor.

# TODO

See [TODO](./TODO.md) for details.
