# Open Steam Controller

Welcome to the Open Steam Controller Project!

The primary intention of this project is to explore, deconstruct and, hopefully,
 expand upon configuration options and usages for the Steam Controller. 

A secondary intention is for this project to be an educational and/or 
 motivational resource for people trying to do more with the Steam Controller.
 This means that an emphasis will be placed on communication and explanation,
 both in documentation and comments. 

Note that this project started with a much smaller scope and you may come across
 echoes of that. Originally the hope was that this would be a small project that
 would provide a method to modify Valve's official firmware in order
 to customize the start up and shutdown Jingles played by the controller using
 the haptics. However, the process of attempting to figure out to do this
 turned into a significant reverse engineering effort. The knowledge gained so 
 far is allows for doing much more with the Steam Controller than originally 
 intended, and while a major goal still is to provide a user friendly method to 
 allow a user to modify the Jingles played by Valve's official firmware, 
 that is no longer the sole goal of this project. 

For a starting point on additional details on the accomplishments and future
 intentions of this project please see the Subprojects section below. 


# Notice and Warning

Please recognize that by utilizing any part of this project you are doing so at
 your own risk. This is a reverse engineering effort and I make no claims to
 have a complete understanding of the hardware I am providing custom or modified
 firmware for. Hopefully the testing I perform before publishing my findings and
 modifications will prevent damage to your Steam Controller or anything you may
 connect your Steam Controller to (i.e. the Nintendo Switch). However, I cannot
 guarantee that. 

Also please note this is being shared primarily for educational purposes. While
 this may allow a user to do things such as use the Steam Controller on the
 Nintendo Switch, please recognize that this is almost definitely going to be
 coming with trade-offs compared to officially supported hardware and firmware.
 I recommend that if you want a cheap Switch Controller and are OK with it being
 wired check out what ones PowerA or Hori makes. If you want a fully featured
 controller consider spending the extra money and get a Pro Controller (it 
 really is worth it).  

Finally note that I say all of this with the hope of not discouraging anyone.
 Please dive in and get your hands dirty. Just please me away that there is a 
 (hopefully) very small, but non-zero, chance that a mistake could lead to 
 bricked or broken hardware. 


# Subprojects

## [Reverse Engineering](./ReverseEngineering/)

The work in this directory focuses on deconstructing the firmware and hardware
 of the Steam Controller. The understanding gained from this is the basis for
 the other work being done in this project. 

## [Firmware](./Firmware/)

The work in this directory is geared towards creating a completely custom firmware
 build for the Steam Controller. This project can be configured to have the
 controller act as a development system that allows for querying peripherals
 and exploring or verifying system behaviors. Additionally the project can
 be configured for more practical purpose, such as behaving like a wired
 Nintendo Switch controller.

## [Jingle](./Jingle)

The work in this directory is geared towards creating a user friendly way to
 customize what Jingles are when the Steam Controller starts up and shuts down.

## [USB Configuration](./UsbConfiguration)

Steam offers the ability to change certain configuration options for a Steam
 Controller (i.e. change LED brightness, change which Jingle will be played
 from system memory). This directory contains work for understanding, 
 reproducing and, maybe, expanding upon this functionality.


# Contributing

This is an open source project, so please make use of this however you would
 like (as long as it does not violate any licenses stated within). 

If you would like to offer contributions back to this repo that would be great.
 That could come in the form of issues/discussions on how this could be more
 user friendly or by adding features or support that is not yet there. 

Note that there is are TODO documents for each Subproject. The idea
 is to outline goals I would like to accomplish. If you want to help with any of
 these that would be awesome. It would be even more awesome if you started a
 conversation regarding your intentions so as to limit the chances of duplicated
 effort or incongruous designs.


# Bugs and Issues

I will do my best to provide easy to use and robust software. However, there
 will still most likely be bugs. If you find a bug, please report it. 

The best way to report a bug is to use the Issues tab on GitHub. 

When reporting a bug/issue, please make sure to provide sufficient details. Key
 details to include are:
* Exactly what steps did you take?
* Can you reproduce the error by following your own steps?
* Do you have any files that make the bug occur (i.e. specific .musicxml file)?


# Resources and Influences

This section lists some projects that influenced this project, as well as links
 to some resources that may continue to be helpful to stay abreast of.

## [Pilatomic Steam Controller Singer](https://gitlab.com/Pilatomic/SteamControllerSinger)

* Example of using the haptics to play music.
* Example of PC side code written in C using libusb.
* Downside is that this takes over the controller when playing custom music and requires the PC to constantly be sending Notes.

## [HID Joy-Con Whispering](https://github.com/shinyquagsire23/HID-Joy-Con-Whispering)

* Application for communicating with Pro Controller via USB HID. 
* Helpful for Switch Faux Controller development to get USB communications in place. 

## [Nintendo Switch Reverse Engineering](https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering)

* Notes on Reverse Engineering of Nintendo Switch Controllers.
* Helpful details on HID communications with Pro Controller. 

## [Standalone Steam Controller Driver](https://github.com/ynsta/steamcontroller)

* Source for breaking down some USB commands.
* Goal of this project is to be able to use Steam Controller as a game controller outside of the Steam platform. 

## [Steam Controller Update News](http://store.steampowered.com/news/?appids=353370)

* Useful to get an idea of what changed from firmware to firmware release.

## [UnBricking Steam Controller (Manual Firmware Update, Rollback)](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)

* Step by step guide on how to manually change the Steam Controller firmware. 
* Much of this information is captured in [Loading Firmware](./LoadingFirmware.md)
    * This presents the information in the context of how it applies to this Project and its Subprojects

## Steam Communities

* [Official Group](http://steamcommunity.com/games/353370#announcements/detail/901091250587237164)
* [Custom Sounds Discussion](https://steamcommunity.com/app/353370/discussions/0/458607699626517823/)


# TODO

Future improvements, goals and known bugs are outlined in [TODO](./TODO.md).
