# Jingle 

We are defining a Jingle to be the series of Notes that are played on the 
 Right and Left Haptics when the Steam Controller powers up and powers down.
 The work in this directory will be geared towards understanding how Valve's 
 official firmware defines a Jingle and how we can leverage this to expand
 upon the default Jingles offered by Valve.

All the information in this document was deciphered via reverse engingineering
 Valve's official firmware. Refer to the 
 [Reverse Engineering](../ReverseEngineering/) subproject for details.


# Jingle Data Format

This section details what a Jingle is, and how that information is represented
 as bits and bytes.

## Note

A Note is defined a generating a particular frequency via a haptic for a 
 particular amount of time. Notes are specified via the following data
 structure:

| Byte Offset(s) | Field Name | Description | 
|---------------:|------------|-------------|
|              0 | Duty Cycle | Percentage time that pulse wave is high. 0 = 0% duty cycle and 511 = 100% duty cycle. |
|              1 | Reserved   | Unused. |
|            2:3 | Frequency  | Frequency of the pulse wave being generated for this Note in Hz. |
|            4:5 | Duration   | The number of milliseconds for which the pulse wave is generated. |

## Jingle

A Jingle defines a sequence of of Notes to be played on each channel (i.e. 
 Haptic). Jingles are specified via the following data structure:

| Byte Offset(s)                                                           |      Field Name | Description | 
|-------------------------------------------------------------------------:|-----------------|-------------|
|                                                                      0:1 | Num Notes Right | The Number of Notes to be played on the Right Haptic. |
|                                                                      2:3 |  Num Notes Left | The Number of Notes to be played on the Left Haptic. |
|                                              3:(2 + sizeof(Notes Right)) |     Notes Right | The sequence of Notes to be played on the Right Haptic. |
| (3 + sizeof(Notes Right)):(2 + sizeof(Notes Right) + sizeof(Notes Left)) |      Notes Left | The sequence of Notes to be played on the Left Haptic. |

## Jingle Data

The Jingle Data structure stores information on multiple Jingles. This is what
 the official firmware reads into memory and references when instructed to
 play a Jingle at a specified index. Jingle Data is specified via the following
 data structure:

|                                                Byte Offset(s) |       Field Name | Description | 
|--------------------------------------------------------------:|------------------|-------------|
|                                                           0:1 |       Magic Word | Word that indicates if Jingle Data is valid or not. 0xbead indicates valid data.  |
|                                                           2:3 |         Reserved | Unused. |
|                                                             4 |      Num Jingles | Defines how many Jingles are stored in this data structure. |
|                                                             5 |         Reserved | Unused. |
|                                       (6 + 2 * n):(7 + 2 * n) | Jingle[n] Offset | Byte offset (from beginning of Jingle Data structure) to beginning of data for Jingle at index n |
| (Jingle[n] Offset):(Jingle[n] Offset + sizeof(Jingle[n]) - 1) |        Jingle[n] | See [Jingle](#Jingle) Section for further details. |


# Jingle Data Locations

This section details the different memory locations where Jingle Data may be 
 read from.

## Default Jingle Data

TODO: Mention packed into firmware, then read into ... Finally copied over onto ... (if EEPROM data is invalid)

## EEPROM Jingle Data

Upon boot offset 0x200 bytes are read from offset 0x800 in EEPROM. If the data
 read contains valid Jingle Data (i.e. Magic Word is 0xbead), this will be used
 in place of the default Jingle Data.


# Creating Custom Jingles

This section details how a user is given the tools to create their own Jingles.

## Open Steam Controller Console Approach

## MusicXML Approach


# Custom Jingles

This section details different approaches for allowing a user to customize
 the available Jingles that can be played.

## Overwriting Default Jingle Data

TODO: mention changing packed data in firmware, or changing assembly to read blob from previously empty section of firmware that we fill in with custom data

### Approach

### Risks

## Specifying EEPROM Jingle Data

This approach leverages the fact that the official firmware checks EEPROM to 
 see if it has valid Jingle Data in it. If we can provide users a way to 
 create custom Jingle Data and save it to EEPROM, this will allow them to
 have custom Jingles without needing to modifying the official firmware. 

### Approach

### Risks

Some of the data stored in EEPROM is vital to proper operation of the controller
 (i.e. Hardware Revision information). If this gets corrupted we run the risk of
 bricking a controller, or at best making it a challenge to get it running 
 properly again. 

