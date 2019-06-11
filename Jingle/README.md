# Jingle 

One of the features I found the most intriguing when I bought a Steam Controller is 
 that when the controller powers up and shuts down, it plays a little song. These
 songs (or Jingles as I often refer to them) can be configured via Steam. 
 However, the choices are limited, with only 14 options being available. 

The work in this directory is the result of me wanting to understand
 how these Jingles are produced and wondering if there was any way to customize
 them, without otherwise impacting the operation of the official firmware. It
 turns out there is a very clean way to accomplish exactly what I wanted, as
 it seems Valve may have been planning to add such customization options. 
 Specifically what I am referring to here is that the official firmware 
 has a mechanism for checking non-volatile memory (EEPROM) and if that data
 is valid it uses it instead of the default Jingles. My guess is that this was
 the beginnings of a feature that never received support from Steam. However,
 now that I understand how it works, I am able to provide software to take
 advantage of it. 

Check out the video below for how this Subproject allows you to customize the
 start up and shut down Jingles on a Steam Controller:
 
 Open Steam Controller: Jingle Customization

 <a href="http://www.youtube.com/watch?feature=player_embedded&v=TDFC0Q24lQA
" target="_blank"><img src="http://img.youtube.com/vi/TDFC0Q24lQA/0.jpg" 
alt="Open Steam Controller: Jingle Customization" width="240" height="180" border="10" /></a>


# Building SCJingleConverter

This section outlines how to build the SCJingleConverter GUI:

* Download Qt (Open Source Version)
* Install and make sure Qt Creator AND a toolchain are selected for installation
    * For macOS 10.12.6 known to successfully build with Qt Creator 4.8.0 and Qt 5.12.0 Clang 64-it toolchain/kit
    * For Windows 10 May 2019 known to successfully build with Qt Creator 4.9.0 and Qt 5.12.12 MinGW 64-bit toolchain/kit
    * For Linux Ubuntu 18.04.2 known to successfully build with Qt Creator 4.9.0 and Qt 5.12.12 GCC 64-bit toolchain/kit
* Open Qt Creator
* Open SCJingleConverter.pro
* Select the appropriate toolchain/kit and Configure the Project
* You should be able to build and run the project via Qt Creator once it finishes configuration


# Customizing Jingles

This section details how a user can create and download custom Jingles onto
 the Steam Controller. These Jingles will persist when the official firmware is
 loaded and allows for customization of start up and shutdown Jingles without
 having to move away from the official firmware provided by Valve.

## Specifying New Jingles

New Jingles are specified via Uncompressed MusicXML formatted files. MusicXML
 is a standard format for sharing sheet music. This was chosen as the 
 input format as there is already a plethora of music available. For example,
 refer to [musescore.com](https://musescore.com) for a wide variety of songs. 

Note that downloading a program like [musescore](https://musescore.org) may
 be necessary. Not only does this allow for exporting music to the necessary
 format (Uncompressed MusicXML with the extension .musicxml), but it also
 provides tools for creating or precise modifying of already created songs.

Once you have a series of .musicxml files that contain songs, or portions of
 songs, you are looking to download as Jingles to the Steam Controller you
 are ready for the next step of Prepping the Controller.

## Prepping the Controller

In order to custom the Jingles on the Steam Controller, the Steam Controller
 must be running a specific version of the Open Steam Controller Firmware.
 The binary that works with the SCJingleConverter GUI is 
 [OpenSteamController.bin](./bin/OpenSteamController.bin).

First follow steps section Backing Up the Firmware of [Loading Firmware](../LoadingFirmware.md) 
 to create a back up the firmware currently on your Steam Controller. 

Second, follow steps section Manually Loading Firmware of [Loading Firmware](../LoadingFirmware.md)
 to download [OpenSteamController.bin](./bin/OpenSteamController.bin) to your
 controller.

Once your controller is connected to your PC with the Open Steam Controller
 Firmware running on it, you are ready to start Downloading Jingle Data to
 the Controller. 

## Downloading Jingle Data to the Controller

SCJingleConverter is a GUI created Qt which converts .musicxml files to Jingle Data
 format and sends that data to the Steam Controller running Open Steam Controller 
 Firmware.

First, make sure the Steam Controller is connected to your PC and running
 [OpenSteamController.bin](./bin/OpenSteamController.bin).

Second, build and launch the SCJingleConverter GUI. See Building SCJingleConverter
 section above for details.

Third, load your .musicxml files using the Browse and Convert buttons. 

Fourth, customize each Jingle by selecting them in the Converted Jingles 
 display and then using the controls on the right to select Start/End Measure, 
 Left/Right Channel Source, etc. Note that you can test play Jingles as you
 customize what portion will play to see how they will sound. 

Finally, save the Jingle Data to EEPROM. Once you have configured all the 
 Jingles and customized them so that they fit within the Jingle Memory (see
 the Jingle Mem Used Bar in the GUI for details), you can press the
 Save to EEPROM button. Once this complete successfully you rae ready to
 restore the Official Controller Firmware.


## Restoring the Official Controller Firmware

Once custom Jingle Data has been saved to EEPROM you can restore the Steam
 Controller firmware to the officially supported firmware and use the controller
 like normal. 

Use the previously backed firmware and follow the steps in [Loading Firmware](../LoadingFirmware.md) 
 to manually restore this firmware. 

If you did not backup the firmware or it became corrupted or was lost, follow the
 Restoring Firmware section in [Loading Firmware](../LoadingFirmware.md).

Once the officially supported firmware is loaded onto the controller it should
 behave like normal, except that the custom Jingle Data you loaded will play
 on start up and shutdown.


## Clearing Jingle Data

If you decide you want to go back to the default Jingle offered by Valve, follow
 the steps outlined above, but after launching the SCJingleConverter GUI, click the 
 "Clear EEPROM" button and then restore the Official Controller Firmware. 


# Jingle Data Details 

See the [Jingle Data Details](./JingleData.md) document for details on what a
 Jingle is and how the official firmware expects this to be organized.


# Resources

All the information in this document was deciphered via reverse engineering
 Valve's official firmware. Refer to the 
 [Reverse Engineering](../ReverseEngineering/) Subproject for details. 

Additionally the [Firmware](../Firmware) Subproject has details and examples of
 how this information is used to play a Jingle via the Steam Controller. See
 [jingle_data.c](../Firmware/OpenSteamController/src/jingle_data.c) and 
 [haptic.c](../Firmware/OpenSteamController/src/haptic.c) specifically for 
 further details.


# TODO

See [TODO](./TODO.md) for details.
