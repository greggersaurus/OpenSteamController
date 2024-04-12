# Open Steam Controller

Welcome to the Open Steam Controller Project!

This is a project where I have been working to deconstruct, document, 
 repurpose and expand upon the Steam Controller firmware and hardware. 


# The Steam Controller

First, it is important to talk a little about the Steam Controller (the
 hardware that this project is focused on).

![SteamControllerBox](./img/SteamControllerBox.png)

The Steam Controller is a video game controller released by Valve Corporation
 on November 10, 2015. This was part of Valve's initiative to give PC gamers the
 option of a console-like experience (i.e. the couch gamer who sits in front of
 a TV and uses a controller instead of keyboard and mouse).

![SteamControllerFront](./img/SteamControllerFront.png)

The Steam Controller tried to keep some aspects familiar to modern video game
 controllers (i.e. a left analog stick, four face buttons, two shoulder and 
 bumper buttons) while also changing up some of the norms we have come to
 expect (i.e. replacing the directional pad and right analog stick with trackpads). 

![SteamControllerBack](./img/SteamControllerBack.png)

Additional details on the controller can be found on the official 
  [Product Page](https://store.steampowered.com/app/353370/Steam_Controller/)


# Why?

You may be wondering why this project exists and why I have invested a 
 non-negligible amount of my time and energy into it over the past couple of
 years.

First, I found the design of this controller to be compelling. Having two 
 trackpads in place of the D-pad and right analog stick is an interesting idea. 
 The trackpads can be configured to act in a variety of ways and not only
 mimic what they replaced, but also provide new interaction paradigms. Also, 
 the idea of using the  haptics to play a little Jingle when the controller 
 starts up or shuts down is a neat touch that made the controller further
 stand out. Being able to fully customize these Jingles was a near immediate
 desire when I first started using the controller. 

Second, the controller was left fairly accessible. Being able to upload 
 firmware updates manually made the barrier to entry for running custom
 firmware much lower. There was still a considerable challenge to get even
 a "Hello World" (i.e. blink the Steam Button LED) working. However, this was
 not nearly as tough as I imagine doing something similar on a controller 
 that is designed to be more tamper resistant would be. I really appreciate 
 the fact that Valve left this hardware as accessible as they did and do 
 not discourage people from trying to hack it. 

Third, iFixit did a really nice [teardown](https://www.ifixit.com/Teardown/Steam+Controller+Teardown/52578)
 of the controller. This gave me an idea of what the controller had going on
 internally, without needing to (initially) take apart my hardware. This 
 pointed out a lot of neat peripherals and processors that I thought would
 be neat to work with, especially in the context of a product that already
 was designed for a particular purpose. 


# The Open Steam Controller Project

Now that the controller and some of its (default) capabilities have been explained,
 we can start talking about this project. Namely I want to cover what has been 
 accomplished, and what I hope to accomplish moving forward. 

Note, the following is a bit verbose and narrative. There is a lot of content
 in this project and this is an attempt to break down the details in a way that will
 give an easy to follow lay of the land for people new to this effort. If you are a TLDR kind 
 of person, check out the Notice and Warning Section below and then feel free to
 jump into the Subprojects Section to get to the meat of effort. You should also be sure to
 check out the demo videos that show some of the major accomplishments in action. 

As mentioned previously, the primary intention of this project is to explore, deconstruct and
 expand upon configuration options and usages for the Steam Controller. However,
 that is a bit vague. So let's start with some examples. First, a major goal 
 of this project was to be able to write custom firmware for the main processor
 on the controller. This is what opens up the door for a lot of other 
 possibilities moving forward. 

However, before I could start writing firmware I needed more 
 info on the controller. Valve was nice enough to leave the door open to
 accessing the binary version of their firmware and the ability to update
 the main processor easily. However, they did not provide schematics or 
 a list of components or datasheets that an embedded software engineer
 would typically have if tasked with the process of writing custom
 firmware. Therefore, I needed to turn to reverse engineering. This essentially
 boils down to deconstructing the hardware and official firmware so that I
 could understand how to write my own firmware. At this point the Reverse
 Engineering effort has mostly run its course, as a lot has been discovered,
 documented and utilized for other efforts. Details on the Reverse Engineering
 effort can be found in the [Reverse Engineering Subproject](./ReverseEngineering/).

Now that a Reverse Engineering effort has been established and I
 have started gathering information on the hardware, I was able to start writing
 custom Firmware. Check out the [Firmware Subproject](./Firmware/) to see the current
 state of software. Mostly this is utility software to allow a user to 
 query different peripherals or take measurements at different states (as
 some of that information needed to be captured and fed back into the Reverse
 Engineering process to keep it moving forward). However, it eventually became
 clear that there was enough functionality that was captured here that it would not
 be too much additional work to make this hardware act as a controller for
 other consoles (i.e. the Nintendo Switch).

Next we come to what was originally the sole intention of this project. That is,
 the goal of being able to have full control of the songs (Jingles) that the 
 Steam Controller plays (via the Trackpad Haptics) on power up and shut down. 
 Due to some discoveries via the Reverse Engineering effort, this project
 allows for Jingles to be fully customized and for these customizations to
 persist while still running Valve's official firmware. For details on how
 to customize the Jingles on your controller, or for more details (i.e. how Jingle 
 Data is formatted and how we can customize Jingles without modifying the official
 firmware) see The [Jingles Subproject](./Jingle).

Finally, it is important to mention that another intention of this project is 
 for it to to be an educational and/or motivational resource. Maybe you are 
 interested in the Steam Controller. Or maybe you are interested in writing
 embedded software for a commercial device. Either way the hope is for this
 project to provide clear and concise information to inspire and enable
 people to to learn and do more. I know I learned a lot from this effort, and
 if any of that can be shared and help others out that would be great. 

For a starting point on additional details and the accomplishments and future
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
 If you want a cheap Switch Controller and are OK with it being
 wired, check out the ones PowerA or Hori makes. If you want a fully featured
 controller, consider spending the extra money and get a Pro Controller (it 
 really is worth it).  

Finally note that I say all of this with the hope of not discouraging anyone.
 Please dive in and get your hands dirty. Just please be aware that there is a 
 (hopefully) very small, but non-zero, chance that a mistake could lead to 
 bricked or broken hardware. 


# Subprojects

This section gives brief outlines and demonstrations (if applicable) of the 
 major efforts of this project. Further details on each Subproject can
 be found in their respective directories.

## [Firmware](./Firmware/)

The work in this directory is geared towards creating a completely custom firmware
 build for the Steam Controller. This project can be configured to have the
 controller act as a development system that allows for querying peripherals
 and exploring or verifying system behaviors. Additionally the project can
 be configured for more practical purpose, such as behaving like a wired
 Nintendo Switch controller.
 
 Below are some videos demonstrating the different firmware builds:
 
 Open Steam Controller: Nintendo Switch
 
 <a href="http://www.youtube.com/watch?feature=player_embedded&v=fT7ddPzb7A8
" target="_blank"><img src="http://img.youtube.com/vi/fT7ddPzb7A8/0.jpg" 
alt="Open Steam Controller: Nintendo Switch" width="240" height="180" border="10" /></a>

Open Steam Controller: Development Board

 <a href="http://www.youtube.com/watch?feature=player_embedded&v=6n_3tYg0XP8
" target="_blank"><img src="http://img.youtube.com/vi/6n_3tYg0XP8/0.jpg" 
alt="Open Steam Controller: Development Board" width="240" height="180" border="10" /></a>

## [Jingle](./Jingle)

The work in this directory is geared towards creating a user friendly way to
 customize what Jingles are played when the Steam Controller starts up and 
 shuts down.
 
 Below is a video demonstrating how to customize Jingles on the Steam Controller:
 
 Open Steam Controller: Jingle Customization

 <a href="http://www.youtube.com/watch?feature=player_embedded&v=TDFC0Q24lQA
" target="_blank"><img src="http://img.youtube.com/vi/TDFC0Q24lQA/0.jpg" 
alt="Open Steam Controller: Jingle Customization" width="240" height="180" border="10" /></a>
 
## [Reverse Engineering](./ReverseEngineering/)

The work in this directory focuses on deconstructing the firmware and hardware
 of the Steam Controller. The understanding gained from this often is a  
 significant part of the other Subprojects. 

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

Note that there are TODO documents for each Subproject. The idea
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
 to some resources that may continue to be helpful to stay current on.

## [Pilatomic Steam Controller Singer](https://gitlab.com/Pilatomic/SteamControllerSinger)

* Example of using the haptics to play music.
* Example of PC side code written in C using libusb.
* Allows for playing songs of any length, but at cost of controller being occupied by Singer software.

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

* Useful to get an idea of what changed from official firmware to firmware release.

## [UnBricking Steam Controller (Manual Firmware Update, Rollback)](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074)

* Step by step guide on how to manually change the Steam Controller firmware. 
* Much of this information is captured in [Loading Firmware](./LoadingFirmware.md).
    * [Loading Firmware](./LoadingFirmware.md) presents the information in the context of how it applies to Open Steam Controller Project and its Subprojects.

## Steam Communities

* [Official Group](http://steamcommunity.com/games/353370#announcements/detail/901091250587237164)
* [Custom Sounds Discussion](https://steamcommunity.com/app/353370/discussions/0/458607699626517823/)


# TODO

While a lot has been accomplished in this project thus far, there is still so much that 
 can be done with the Steam Controller (especially given the groundwork that has
 been laid up to this point). In order to capture and track these ideas TODO documents
 have been made for each Subproject. A good place to start is the top-level
 [TODO Document](./TODO.md). This covers the highest level goals for the project
 while providing links to Subproject specific TODOS.
