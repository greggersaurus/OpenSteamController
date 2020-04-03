# Valve Protocol

Valve uses a unified protocol for most of their devices: The Steam Controller,
Steam Controller Dongle, Knuckle Controllers, and even the frontplate of some
Steam Machines?

The protocol sits on top of HID, and uses Feature Reports for its
communications. All usb devices compatible with this protocol have a USB VID of
0x28de and a USB PID between 0x1101 and 0x1201 (exclusive). It's a sort of
remote procedure call protocol, where the Host will send a packet via
SET_FEATURE_REPORT to the device, and optionally retrieve the result with
GET_FEATURE_REPORT.

Each feature report is 0x40 bytes in size. The first byte of the Feature Report
is a packet id, while the second byte is the number of additional bytes the
report contains.

Host->Device:


  ID   |  Name                                | Bootloader |  Firmware  | Dongle
-------|--------------------------------------|------------|------------|--------
 0x80  | SetControllerMappings                | ⛔         | ✅         | ⛔
 0x81  | [ClearControllerMappings]            | ⛔         | ✅         | ❓
 0x82  | GetControllerMappings                | ⛔         | ✅         | ✅
 0x83  | [ControllerInfoRequest]              | ✅         | ✅         | ✅
 0x84  |                                      | ⛔         | ⛔         | ✅
 0x85  | ResetControllerMappings              | ⛔         | ✅         | ⛔
 0x86  | FactoryReset                         | ⛔         | ⛔         | ⛔
 0x87  | [SetSettings]                        | ⛔         | ✅         | ⛔
 0x89  |                                      | ⛔         | ✅         | ✅
 0x8a  |                                      | ⛔         | ⛔         | ✅
 0x8b  |                                      | ⛔         | ⛔         | ✅
 0x8c  |                                      | ⛔         | ⛔         | ✅
 0x8d  |                                      | ⛔         | ✅         | ⛔
 0x8e  | SetSettingsDefaultValues             | ⛔         | ✅         | ⛔
 0x8f  | [TriggerHapticPulse]                 | ⛔         | ✅         | ✅
 0x90  | [RebootToISP]                        | ✅         | ✅         | ✅
 0x91  | EraseLPCFirmware                     | ✅         | ⛔         | ⛔
 0x92  | FlashLPCFirmware                     | ✅         | ⛔         | ⛔
 0x93  | VerifyLPCFirmware                    | ✅         | ⛔         | ⛔
 0x95  | [ResetSOC]                           | ✅         | ✅         | ⛔
 0x96  | SetPrngEntropy                       | ⛔         | ⛔         | ✅
 0x97  | EraseNRFFirmware                     | ✅         | ✅         | ⛔
 0x98  | FlashNRFFirmware                     | ✅         | ✅         | ⛔
 0x99  | VerifyNRFFirmware                    | ✅         | ✅         | ⛔
 0x9a  |                                      | ⛔         | ✅         | ⛔
 0x9b  |                                      | ⛔         | ⛔         | ✅
 0x9c  |                                      | ⛔         | ⛔         | ❓
 0x9d  |                                      | ⛔         | ⛔         | ❓
 0x9e  |                                      | ⛔         | ⛔         | ❓
 0x9f  | TurnOffController                    | ⛔         | ✅         | ❓
 0xa0  | SetHardwareVersion                   | ✅         | ⛔         | ⛔
 0xa1  |                                      | ⛔         | ⛔         | ✅
 0xa2  |                                      | ⛔         | ⛔         | ✅
 0xa3  |                                      | ⛔         | ⛔         | ✅
 0xa4  |                                      | ⛔         | ⛔         | ✅
 0xa5  |                                      | ⛔         | ⛔         | ✅
 0xa6  |                                      | ⛔         | ⛔         | ✅
 0xa7  | CalibrateTrackpads                   | ⛔         | ✅         | ⛔
 0xa9  |                                      | ⛔         | ✅         | ⛔
 0xaa  |                                      | ⛔         | ✅         | ✅
 0xab  |                                      | ⛔         | ✅         | ✅
 0xac  |                                      | ⛔         | ✅         | ⛔
 0xad  | SetDonglePairingMode                 | ⛔         | ⛔         | ✅
 0xae  | ControllerInfoRequest                | ⛔         | ✅         | ⛔
 0xaf  |                                      | ⛔         | ⛔         | ✅
 0xb0  |                                      | ⛔         | ⛔         | ✅
 0xb1  | SetControllerKeyboardMouseInputState | ⛔         | ⛔         | ✅
 0xb2  | PairingFailed                        | ⛔         | ⛔         | ✅
 0xb3  | PairingSuccess                       | ⛔         | ⛔         | ✅
 0xb4  | GetControllerInfo                    | ⛔         | ⛔         | ✅
 0xb5  | CalibrateIMU                         | ⛔         | ✅         | ⛔
 0xb6  | PlayAudio                            | ⛔         | ✅         | ⛔
 0xb7  | StartFlashJingle                     | ⛔         | ✅         | ⛔
 0xb8  | FlashJingle                          | ⛔         | ✅         | ⛔
 0xb9  | EndFlashJingle                       | ⛔         | ✅         | ⛔
 0xba  | GetChipID                            | ⛔         | ✅         | ⛔
 0xbb  | ReadUID                              | ⛔         | ✅         | ⛔
 0xbf  | CalibrateJoystick                    | ⛔         | ✅         | ⛔
 0xc1  | SetAudioMapping                      | ⛔         | ✅         | ⛔
 0xc5  | SetUserLedColor                      | ⛔         | ⛔         | ⛔
 0xc6  | SendIRCode                           | ⛔         | ⛔         | ⛔
 0xc7  | StopIR                               | ⛔         | ⛔         | ⛔

## ClearControllerMappings
[ClearControllerMappings]: #ClearControllerMappings

Sent when exiting lizard mode. Clears the controller mappings.

## ControllerInfoRequest
[ControllerInfoRequest]: #ControllerInfoRequest

Asks the controller to send its ControllerInfo. The response will contain a
Packet ID 0x83, and its data will be an array of HardwareInfo.

### HardwareInfo

Offset | Type | Field Name
-------|------|-----------
0      | u8   | Type
1      | u32  | Value, little endian

### HardwareInfo types

Here are the different types byte understood by the Steam Client software
(as of 1/04/2020):

Type ID | Name | Bootloader | Description
----------|--------|-----------|---------
0 | Unknown | ⛔ | Firmware returns eeprom field 8.
1 | USB PID | ✅ | The same as the USB PID. Likely more useful for wireless transmission.
2 | flags?  | ⛔ | Firmware always returns 3
3 | Unknown | ⛔ | Not sent by firmware.
4 | Firmware Version | ✅ | Version/timestamp of the firmware running on the LPC side (e.g. 57bf5c10).
5 | NRF Firmware Version | ⛔ | Version/timestamp of the firmware running on the NRF side
9 | Hardware Version | ✅ | Version of the controller hardware (as stored in EEPROM)
10 | Unknown | ⛔ | Firmware returns the data stored at DAT_10000078
11 | Unknown | ⛔ | Not sent by firmware.

## SetSettings
[SetSettings]: #SetSettings

Sets the controller settings. The additional data is an array of ControllerSetting.

### Controller settings

Offset | Type | Field Name
-------|------|-----------
0      | u8   | Setting Type
1      | u16  | Setting Value, little endian

### Setting types

 ID  | Name | Min | Max    | Default | Description
-----|------|-----|--------|---------|---------------
0x00 |      | 0   | 10     | 8       |
0x01 |      | 0   | 10     | 3       |
0x02 |      | 0   | 360    | 345     |
0x03 |      | 0   | 25000  | 1200    |
0x04 |      | 0   | 1      | 0       |
0x05 |      | 0   | 1      | 0       |
0x06 |      | 0   | 6      | 0       |
0x07 |      | 0   | 8      | 3       |
0x08 |      | 0   | 8      | 0       | Lizard mode related? Data is 7 when sent to dongle.
0x09 |      | 0   | 1      | 1       |
0x0a |      | 0   | 0x4000 | 7000    |
0x0b |      | 0   | 30     | 6       |
0x0c |      | 0   | 50     | 5       |
0x0d |      | 1   | 500    | 100     |
0x0e |      | 0   | 200    | 50      |
0x0f |      | 0   | 2000   | 923     |
0x10 |      | 0   | 2000   | 382     |
0x11 |      | 1   | 10     | 2       |
0x12 |      | 0   | 20000  | 8000    |
0x13 |      | 0   | 4096   | 0xF830  |
0x14 |      | 0   | 4096   | 0xF254  |
0x15 |      | 0   | 500    | 5       |
0x16 |      | 0   | 8      | 2       |
0x17 |      | 0   | 8      | 2       |
0x18 |      | 0   | 20     | 15      |
0x19 |      | 0   | 0x7FFF | 4000    |
0x1a |      | 0   | 0      | 0       |
0x1b |      | 1   | 0x7FFF | 27500   |
0x1c |      | 0   | 0x1000 | 0xF830  |
0x1d |      | 0   | 0x1000 | 0xF254  |
0x1e |      | 0   | 2000   | 0       |
0x1f |      | 1   | 0x4000 | 12000   |
0x20 |      | 0   | 1      | 1       |
0x21 |      | 0   | 8000   | 4000    |
0x22 |      | 1   | 500    | 200     |
0x23 |      | 0   | 1      | 0       |
0x24 |      | 0   | 1      | 0       |
0x25 |      | 10  | 1500   | 300     |
0x26 |      | 1   | 1000   | 50      |
0x27 |      | 0   | 1      | 0       |
0x28 |      | 1   | 180    | 20      |
0x29 |      | 0   | 25000  | 600     |
0x2a |      | 0   | 1      | 0       |
0x2b |      | 0   | 1      | 0       |
0x2c |      | 0   | 100    | 100     |
0x2d |      | 0   | 100    | 100     | Sent periodically
0x2e |      | 0   | 1      | 0       |
0x2f |      | 0   | 1      | 0       | Haptics ADC Sample Size. 0 = 256, 1 = 128.
0x30 |      | 0   | 0x7FFF | 0       |
0x31 |      | 1   | 2      | 2       |
0x32 |      | 0   | 0x7FFF | 1800    |
0x33 |      | 0   | 0x7FFF | 0       |
0x34 |      | 0   | 0      | 0       |
0x35 |      | 0   | 0      | 0       |
0x36 |      | 0   | 0      | 0       |
0x37 |      | 0   | 0      | 0       |
0x38 |      | 0   | 0      | 0       |
0x39 |      | 0   | 0      | 0       |
0x3a |      | 0   | 0      | 0       |
0x3b |      | 0   | 0      | 0       |

## SetDonglePairingMode
[SetDonglePairingMode]: #SetDonglePairingMode

Arguments:
- Don't pair: 0x00 0x00
- Start pairing: 0x02 0x3c

## TriggerHapticPulse
[TriggerHapticPulse]: #TriggerHapticPulse

Triggers a Haptic vibration.

Offset | Type | Name
-------|------|-------
0      | u8   | Haptic side (0 = right, 1 = left)
1      | u16  | ??
3      | u16  | ??
5      | u16  | ??
7      | u8   | Optional - defaults to zero if not present.

## RebootToISP
[RebootToISP]: #RebootToISP

Reboots this device to the in-system programmer. The specifics of that mode are
device-specific. See the device-specific documentation for more information.

Depending on the type of device, it may take additional arguments:

- For the Steam Controller Dongle, takes 4 bytes of arguments: `7a 6f 74 21`. It
  is unknown what those arguments are.
- For the Steam Controller, takes no arguments.

## ResetSOC
[ResetSOC]: #ResetSOC

Restarts the device. May optionally be passed an argument to restart the device
in a specified mode. For the steam controller:

- `C0 BA AA EC` will reset the controller in bootloader mode.
- Otherwise, it will restart normally.
