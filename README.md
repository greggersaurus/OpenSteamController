# Steam Controller Customize 

The purpose of this project is to explore, deconstruct and, hopefully, expand 
 upon configuration options and usages for the Steam Controller. 

This project is broken up into several subdirectories, each which focus on a
 different facet of the project.

# Subdirectories/Subprojects

## Reverse Engineering

The work in this directory focuses on deconstructing the firmware and hardware
 of the Steam Controller. The understanding gained from this is the basis for
 the other work being done in this project. 

## Development Board

The work in this directory is geared towards a completely custom firwmare for 
 the main LPC11U37 chip that will act a development environment for interfacing
 with the peripherals available on the Steam Controller hardware. 

## USB Configuration

Steam offers the ability to change certain configuration options for a Steam
 Controller (i.e. change LED brightness, change jingle to one of a few 
 pre-defined options). This directory contains work for understanding, 
 reproducing and, maybe, expanding upon this functionality.

## Custom Jingle

The work in this directory will be geared towards granting the ability to 
 completely customize the jingle played on the Steam Controller haptics 
 on power on and power off. 

# Resources and Influences

## [Pilatomic Steam Controller Singer](https://gitlab.com/Pilatomic/SteamControllerSinger)

* Example of using the haptics to play music
* Example of code written in C using libusb
* Downside is that it takes over controller when playing custom music

## [Standalone Steam Controller Driver](https://github.com/ynsta/steamcontroller)

* OK source for breaking down some USB commands
* Goal of this project is to work outside Steam platform. I want to add configuration ability to controller, not subvert or reproduce (though some reproduction will be necessary) what is already there

## [Steam Controller Update News](http://store.steampowered.com/news/?appids=353370)

Use to get an idea of what changed from firmware to firwmare release

## Steam Communities

* [Official Group](http://steamcommunity.com/games/353370#announcements/detail/901091250587237164)
* [Custom Sounds Discussion](https://steamcommunity.com/app/353370/discussions/0/458607699626517823/)
