# [USB Configuration](./) TODO

When first starting this project, there was a bit of a naive hope that the
 USB interface would a) have functions to help accomplish what I wanted in 
 terms of customizing Jingles and b) this functionality would be easy to 
 figure out. 

I started by mapping out USB exchanges for common functions (i.e. changing
 the LED brightness). However, after mapping out the basics I moved my focus
 to [Reverse Engineering](../ReverseEngineering/) and largely set this effort aside. 

At this point I only have one low priority pipe dream for this effort. The hope is 
 that by spending further effort Reverse Engineering the USB callbacks I might
 discover an unused USB command that would allow me to change EEPROM values
 at my own discretion, or at least change the values where custom Jingle Data
 resides. This could then be used to simplify the Jingle Customization process
 by not requiring the user to download custom firmware to their Steam Controller.


# Notes

Below are less verbose notes on items I would like to dig into. These are a bit
 terse and may not be worded well. Please ask if you have any questions:

* Work through reverse engineering to see if there is a USB command to allow for modifying EEPROM already exists
    * Could eliminate step of switching firmware when customizing EEPROM Jingle Data

