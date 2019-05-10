# Jingle 

We are defining a Jingle to be the series of Notes that are played on the 
 Right and Left Haptics when the Steam Controller powers up and powers down.
 The work in this directory will be geared towards understanding how Valve's 
 official firmware defines a Jingle and how we can leverage this to expand
 upon the default Jingles offered by Valve.

All the information in this document was deciphered via reverse engingineering
 Valve's official firmware. Refer to the 
 [Reverse Engineering](../ReverseEngineering/) subproject for details.

TODO: mention jingle_data.c and how that is an implementation of the data captured here (also mention haptic.c as it details how haptics are controlled to produce Jingle)


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

For Firmware vcf_wired_controller_d0g_57bf5c10.bin:
	(uint32_t*)0x000065d0 Stores how many bytes are in default Jingle Data (i.e. 0x3fa or 0x400)
	(uint32_t*)0x000065d4 Stores location of where Jingle Data resides (i.e. 0x100003b8, which could be changed to address in flash that we fill with custom data...?)

### Approach

1. Modify default Jingle Data read from firmware
    1. Not Great: data is packed into firmware (i.e. all 0 bytes/words are not actually in firmware, but handled upon read in) and this is another level to understand (and which may not be consistent with never versions)
1. Modify firmware to read default Jingle Data from different location
    1. Create Jingle Data blob (unpacked)
    1. Write into unused section of firmware
        1. Assumes we can detect unused section
        1. Assumes contiguous 0x400 byte of unused space
    1. Modify copy to 0x10001200 to read from flash instead of RAM
        1. Will this actually work?

### Risks

## Specifying EEPROM Jingle Data

This approach leverages the fact that the official firmware checks EEPROM to 
 see if it has valid Jingle Data in it. If we can provide users a way to 
 create custom Jingle Data and save it to EEPROM, this will allow them to
 have custom Jingles without needing to modifying the official firmware. 

### Approach

1. USB command (if it exists)
    1. Need to simulate and see if this even possibly exists
1. DevBoard/CustomFW
    1. Clunky having to use cli to input new data

### Risks

Some of the data stored in EEPROM is vital to proper operation of the controller
 (i.e. Hardware Revision information). If this gets corrupted we run the risk of
 bricking a controller, or at best making it a challenge to get it running 
 properly again. 



# TODO

* Add Open Steam Controller Dev Kit Firmware bin to directory
* Add executables for SCJingleConverter to directory
    * Mac, Linux, Windows
    * External library issues a concern or all statically linked and built into exe?
* Revisit this md
    * Start byte targeting user who just wants to add Jingle Data to their SC
    * Mention why musicXML
    * Walkthrough ways to get music (i.e. use musescore, convert midi to musicxml)
    * Keep technical info, but maybe link to separate .md for those who are interested?
    * Make section on SCJingleConverter
        * What is purpose?
        * How to use 
        * How to build
    * Add steps on loading firmware to steam controller
        * Make a separate .md and link here and in Firmware section
            * This way we do not miss updates and improvements
* Comments, comments, comments for SCJingleConverter
* Make class to represent each music composition
    * Idea is to capture all Lines/Notes/Chords from musicxml and allow user to choose what to play on each channel
* What happens when there is no serial port available...
    * Make sure GUI still acts gracefully
* Make child SCSerial class for case of not having controller connected?
    * Allows user to select method that writes commands out to file for manual input later??
* GUI
    * Add window to show serial communications
        * Gives user idea of what we are doing when Jingles are being updated on controller
    * Think through how full featured we want this to be, v.s. having a user need to prune their jingles before converting to musicxml
        * Maybe first pass of GUI will be less full featured?
    * Allow for changing tempo via GUI
        * Default fill with value read from xml
    * Allow for changing octave via GUI?
    * Specify start and stop measure instead of number of notes
        * This should be a single setting for jingle and not unique to channels
    * Display total bytes/percentage of space used and space used by each jingle as settings are changed
    * Add button to clear Jingle Data
        * i.e. remove magic word from EEPROM to use defaults
* Repeated notes bleed together to form single long tone...
    * Need to shorten notes and add rests in between if repeated?
* Add jingle command to print hex values for use in FW 
    * i.e. we want Mario theme to play for Power A build config of open FW
    * should we add some sort of industrial tune (terminator?) for dev kit power up?
* Need to add jingle_data.c support for adding jingles, etc.
