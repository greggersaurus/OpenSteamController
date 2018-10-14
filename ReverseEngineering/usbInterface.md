# USB Interface

The purpoes of this document is to track details on the USB interface for the
 Steam Controller. This is done so that EP actions on the firmware side, 
 obvserved in simulation, can match up to real-world behavior. This will be
 used to categorize more actions, memory usage, etc.

## EP1 (Keyboard)

This interface mimics a USB keyboard. 

Below are the messages received via usbmod related to different controller actions:

* Analog Joystick Up = Keyboard Up Arrow
    * 00005200 00000000
* Analog Joystick Down = Keyboard Down Arrow
    * 00005100 00000000
* Analog Joystick Left = Keyboard Left Arrow
    * 00005000 00000000
* Analog Joystick Right = Keyboard Right Arrow
    * 00004f00 00000000
* Left Trackpad Click on Up Area = Keyboard Up Arrow
    * 00005200 00000000
* Left Trackpad Click on Down Area = Keyboard Down Arrow
    * 00005100 00000000
* Left Trackpad Click on Left Area = Keyboard Left Arrow
    * 00005000 00000000
* Left Trackpad Click on Right Area = Keyboard Right Arrow
    * 00004f00 00000000
* Left Arrow Button = Keyboard ??
    * 00002b00 00000000
* Right Arrow Button = Keyboard ??
    * 00002900 00000000
* A Button = Keyboard Enter Key
    * 00002800 00000000
* B Button = Keyboard ??
    * 00002900 00000000

## EP2 (Mouse)

This interface mimics a USB mouse. 

Below are the messages received via usbmod related to different controller actions:

* Right Trackpad = Mouse Movement
    * Mouse moving right slowly
        * 00010000
    * Mouse moving left slowly
        * 00ff0000
    * Mouse moving up slowly
        * 0000ff00
    * Mouse moving down slowly
        * 00000100
* Right Trigger = Mouse Left Click
    * 01000000
* Left Trigger = Mouse Right Click
    * 02000000

## EP3 (Controller)

This does not produce output by default (since custom HID only steam knows how
 to interact??).

TODO: Map out messages with Steam running and controller in EP3 mode, or look online for people who mapped this all out already

# Booting

This section details booting the controller via USB connection, with the focus
 being on what may be causing the jingle to play. 

* Connecting to wall adaptor (i.e. no OS interaction)
    * LED illuminates, but no jingle and Right Trackpad is not in mouse mode (not haptic feedback)
* Connecting to Chromebook (i.e. no Valve drivers or Steam)
    * LED illuminates, Jingle plays
    * EP1: Joystick, Left Trackpad, A/B Buttons act as keyboard
    * EP2: Right Trackpad and L/R Triggers acts as mouse
    * EP3: Nothing
* Connecting to OS with drivers installed (but Steam not running)
    * Same as Chromebook
* Connecting to OS with drivers installed and Steam running
    * USB messages sent to controller
    * EP1 and EP2 go silent
    * EP3 produces periodic output, react to all interfaces (except accelerometer)
