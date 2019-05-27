# Jingle Data

This document details what a Jingle is, and how that information is represented
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

The 14 Jingles that the official firmware uses present to a user that has not
 modified EEPROM are built into the firmware. This data does not follow the 
 format mentioned above exactly, as the compiler packs certain portions of the
 data to be more efficient. 

## EEPROM Jingle Data

Upon boot offset 0x400 bytes are read from offset 0x800 in EEPROM. If the data
 read contains valid Jingle Data (i.e. Magic Word is 0xbead), this will be used
 in place of the default Jingle Data.
