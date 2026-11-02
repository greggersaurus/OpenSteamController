# LPC11U37 to nRF51822 radio link

Summary of how Valve's firmware connects the main chip to the radio. Derived
by disassembling Valve's BLE firmware for both chips, and confirmed on a
controller by running a replacement radio firmware against Valve's unmodified
LPC firmware.

| LPC11U37 | nRF51822 | Use |
|---|---|---|
| `PIO1_18`, USART0 TXD | `P0.15` (pin 21), RXD | LPC to radio. Also a mode strap the radio samples at boot |
| `PIO1_17`, USART0 RXD | `P0.14` (pin 20), TXD | Radio to LPC |
| `PIO1_5` (most likely) | `P0.11` | Flow control: the radio pulses it low to pull the next 4 bytes |
| `PIO1_24` | `SWDIO/nRESET` (pin 23) | Radio reset, and SWD programming |
| `PIO1_8` | `SWDCLK` (pin 24) | SWD programming |

- **UART:** 921600 baud nominal, 8N1, no hardware flow control. The LPC's
  divisors give about 916,667 baud.
- **Flow control:** the LPC writes at most 4 bytes into the FIFO and queues the
  rest. It sends more only when the radio pulls `P0.11` low. A radio that never
  does this receives the first 4 bytes after boot and nothing else.
- **Framing:** `02` starts a frame, `03` ends it, `1F` escapes. Then an opcode
  byte. No checksum, no sequence numbers, no handshake.
- **Lizard mode:** in BLE mode the LPC sends only keyboard and mouse reports
  until lizard mode is turned off with a settings feature report. Steam sends
  it over Bluetooth, and Valve's radio relays it to the LPC as a `0x50` frame.

The full opcode tables, the controller state record layout, and the evidence
behind each line (addresses in the firmware images) are in
[VALVE_LINK.md](https://github.com/dimitrieh/steamcontroller-ble-gamepad/blob/main/docs/VALVE_LINK.md)
in the steamcontroller-ble-gamepad project, which uses this link to make the
controller pair as a standard Bluetooth gamepad.
