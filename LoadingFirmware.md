# Loading Firmware

The purpose of this document is to detail how firmware on the Steam Controller
 can be loaded, backed up and restored. This is necessary for many of the
 Subprojects as they require changing the firmware on the Steam Controller.

Note that most of the information in this document came from 
 [UnBricking Steam Controller (Manual Firmware Update, Rollback)](https://steamcommunity.com/sharedfiles/filedetails/?id=572740074).
 This document exists as a backup of that information and so that it can 
 be presented in the context of the Open Steam Controller Project.


Note that at this point only the firmware on the LPC11U37F processor is being
 modified. There is at least one other chip that has firmware running on it
 that could potentially be changed (the nRF51822 Bluetooth Chip), however, at
 this point the firmware on that chip is not modified as any part of this 
 project.

## Accessing the Firmware

The firmware on the Steam Controller's LPC11U37F processor can be accessed via
 the following steps:

* From a powered down state, hold the right trigger while connecting the controller via USB to a PC
    * This will activate the USB In-System Programming for the LPC11U37F
* The LPC11U37F will act as a FAT12 file system labeled "CRP DISABLD"
    * The file system will have a single file called "firmware.bin"

## Backing Up the Firmware

In order to return the controller to the last known working state, it is 
 recommend to make a backup of the firmware currently installed via Steam. The
 following steps outline how to do this:

* Connect the controller to a PC as outlined in Accessing the Firmware
* Save the firmware to file on your PC
    * On macOS (Tested specifically on 10.12.6):
        * Copy firmware.bin from "CRP DISABLD" mount to backup file (i.e. backup.bin):
            * cat /Volumes/CRP\ DISABLD/firmware.bin > backup.bin
    * On Linux systems (Test on Ubuntu 18.04):
        * Locate mount point of CRP DISABLD
            * The following command may help: `mount | grep DISABLD`
        * Backup the firmware file
            * dd if=<path to mount>/firmware.bin of=backup.bin
    * On Windows (Tested on Windows 10 May 2019 Update):
        * A new removable drive will appear with the name CRP DISABLD (i.e. "CRP DISABLD (E:)")
        * Navigate to the drive using Windows File Explorer and copy firmware.bin to another directory

## Manually Loading Firmware

These steps outline how to manually download new firmware onto the controller.
 It is recommend to back up the last officially loaded firmware via Steam as
 outlined above. 

* Connect the controller to a PC as outlined in Accessing the Firmware
* Download the firmware .bin file:
    * On macOS (Tested specifically on 10.12.6):
        * Mount "CRP DISABLD" will appear
        * Load new firmware binary with command `cat new_firmware.bin > /Volumes/CRP\ DISABLD/firmware.bin`
        * Eject "CRP DISABLD"
    * On Linux systems (As reported by @rigidsh):
        * Locate mount point of CRP DISABLD
            * The following command may help: `mount | grep DISABLD`
        * Load new firmware binary with command: `dd conv=nocreat,notrunc oflag=direct bs=512 if=<path to your firmware> of=<path to sc flash>/firmware.bin`
            * "cat new_firmware.bin > /mount/CRP\ DISABLD/firmware.bin" will corrupt downloaded firmware.
        * Unmount CRP DISABLD
    * On Windows (Tested on Window 10 May 2019 Update):
        * A new removable drive will appear with the name CRP DISABLD (i.e. "CRP DISABLD (E:)")
        * Navigate to the drive using Windows File Explorer and delete firmware.bin
        * Make sure the firmware updated file is named firmware.bin
        * Copy the update file (firmware.bin) to the new drive (i.e. E: CRP DISABLD)
        * Eject the drive

## Restoring Firmware

If the last official firmware loaded by Steam was not backed up properly, these
 steps outline how firmware can be obtained from Valve for manual download:

* Download [firmware.vdf](http://media.steampowered.com/controller_config/firmware/firmware.vdf) file from Valve 
* Open firmware.vdf in a text editor
* The "firmwaretimestamp" entry can give you an idea as to when a particular firmware was built
    * [Epoch Convert](https://www.epochconverter.com/hex) can be used to convert this to a human readable date
* The "firmwarebinary" will give you a portion of the URL you need to download the firmware
* Add the "firmwarebinary" string to the end of http://media.steampowered.com/controller_config/ to create a URL to download the firmware
    * For example http://media.steampowered.com/controller_config//firmware/vcf_wired_controller_d0g_57bf5c10.bin allows for downloading a copy of the firmware with the timestamp 57bf5c10 (Thursday, August 25, 2016 8:58:56 PM GMT).
* Pasting the URL into a web browser will allow you to download the .bin file that can be loaded onto the controller following the steps in Manually Loading Firmware

