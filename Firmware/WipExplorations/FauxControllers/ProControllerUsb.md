# Introduction

This documents the USB communications between the Switch and a Pro Controller. 

The commands sent by the Switch were obtained by the LPCXpresso11U37H Evaluation 
 Board OM13074 appear as a Pro Controller and then recording the USB packets
 received. 

The responses of the Pro Controller were obtained by using the 
 [HID-Joy-Con-Whispering](https://github.com/shinyquagsire23/HID-Joy-Con-Whispering)
 to communicate with a real Pro Controller to see how it responds to the Switch
 commands. 

Note that the Switch was configured with "Pro Controller Wired Communication"
 set to "On".


# Initialization

This is the initial handshaking between the Pro Controller and Switch when
 the USB device is first connected:

Note "..." Indicates last value repeats until end of packet

| Sender         | Size | Data      | Purpose | Expected Response | Notes |
|----------------|------|-----------|---------|-------------------|-------|
| Switch         | 2    | 0x00 0x00 | TODO    | N/A?              |       |
| Pro Controller | 64   | 81 01 00 03 86 b3 38 eb 68 dc 00 ... | Only sent as response on first connection |
| Switch         | 2    | 0x00 0x00 | TODO    | N/A?              |       |
| Switch         | 2    | 0x80 0x05 | Allows the Joy-Con or Pro Controller to time out and talk Bluetooth again?? [source](https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/USB-HID-Notes.md) | N/A ? | | 

At this point the system does not handshake further with the controller... Previously
 my thought was wireless comms were required to move any further. However, 
 YouTube user pxc mentioned The Exlene wired USB controller for Nintendo Switch. 
 Looking at videos on how to use this controller, it seems it is successfully 
 acting like a Pro Controller with only a wired USB interface. 
