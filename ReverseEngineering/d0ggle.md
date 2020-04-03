# D0ggle Hardware

The d0ggle is the dongle that comes with the steam controller. It is a very
simple, off-the-shelf nRF24LU1+ USB device capable of sending radio packets. It
comes preloaded with Valve's `d0ggle.bin` firmware, that can be found in the
Steam client files inside a folder named `controller_base`.

# nRF24LU1+

The nRF24LU1+ is an Intel 8051 microcontroller coupled with an nRF24L01 2.4GHz
Transceiver. This chip is used in a lot of similar hardware, such as the
Logitech Unifying dongles, or many other proprietary wireless keyboard/mouse
dongles.

## Resources

- [Product Specification v1.0](https://web.archive.org/web/20191011024449/https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24LU1P_1_0.pdf)

## Pinout

| Pin Number    | Datasheet Name        | Pin Function     | Pin Direction | Notes         |
|--------------:|-----------------------|------------------|---------------|---------------|
|             1 | VDD                   |                  |               |  |
|             2 | VBUS                  |                  |               |  |
|             3 | VDD                   |                  |               |  |
|             4 | D+                    |                  |               |  |
|             5 | D-                    |                  |               |  |
|             6 | VSS                   |                  |               |  |
|             7 | PROG                  |                  |               |  |
|             8 | RESET                 |                  |               |  |
|             9 | VDD                   |                  |               |  |
|            10 | P0.0                  |                  |               |  |
|            11 | P0.1                  |                  |               |  |
|            12 | VSS                   |                  |               |  |
|            13 | P0.2                  |                  |               |  |
|            14 | P0.3                  |                  |               |  |
|            15 | P0.4                  |                  |               |  |
|            16 | P0.5                  |                  |               |  |
|            17 | VSS                   |                  |               |  |
|            18 | VSS                   |                  |               |  |
|            19 | VDD                   |                  |               |  |
|            20 | VDD_PA                |                  |               |  |
|            21 | ANT1                  |                  |               |  |
|            22 | ANT2                  |                  |               |  |
|            23 | VSS                   |                  |               |  |
|            24 | VDD                   |                  |               |  |
|            25 | IREF                  |                  |               |  |
|            26 | VSS                   |                  |               |  |
|            27 | VDD                   |                  |               |  |
|            28 | DEC1                  |                  |               |  |
|            29 | DEC2                  |                  |               |  |
|            30 | VSS                   |                  |               |  |
|            31 | XC2                   |                  |               |  |
|            32 | XC1                   |                  |               |  |

## Software

The software running on the dongle can be found in a file called d0ggle.bin in
Steam's client files. TODO: Find where it can be downloaded.

The dongle exposes a USB device with VID 0x28de and PID 0x1104. It exposes four
HID interfaces, each corresponding to a single controller connection. They can
be manipulated mostly independently (e.g. when pairing, it's recommended to send
the "pairing packet" to all four interfaces).

Each HID interface follows the Valve Controller Abstraction Protocol. The Packet
IDs accepted by the dongle are documented in the [valve protocol documentation].

The dongle also features a USB Bootloader mode that can be used to flash
alternative firmwares. To enter this bootloader mode, one must send the
[RebootToISP] packet. This bootloader is a derivative of the [Nordic nRF24
Bootloader], modified to use USB HID instead of raw USB. It has the VID 0x28de
and PID 0x1004.

[valve protocol documentation]: valve_protocol.md
[Nordic nRF24 Bootloader]: https://www.sparkfun.com/datasheets/Wireless/Nordic/nRF24LU1P_1_0.pdf#G1308975
[RebootToISP]: valve_protocol.md#RebootToISP