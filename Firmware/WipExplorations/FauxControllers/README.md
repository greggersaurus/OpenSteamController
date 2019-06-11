# Faux Controllers

The purpose of this project is a workspace to learn about how USB HIDs work, 
 specifically in the context of attempting to use the LPCXpresso11U37H 
 Evaluation board OM13074 to act as a known commercial controllers (i.e.
 Nintendo Switch Pro Controller).

Ideally, once the USB communications side of things is understood this project
 will be merged into the Open Steam Controller Firmware efforts and married
 to logic interfacing with the Steam Controller peripherals so that the 
 Steam Controller can act as known commercial controllers, such as the Nintendo
 Switch Pro Controller. 

Note this is being pursued for educational and hobby purposes only. This seemed
 like a fun way to make use of all the peripheral control information being
 gained from reverse engineering the Steam Controller and to learn more about
 USB devices. 

Also note this project is based off of the nxp_lpcxpresso_11u37_usbd_rom_hid_mouse
 example project distributed by NXP via lpcopen_v2_03_lpcxpresso_nxp_lpcxpresso_11u37h. 
 Sorry for shoddy work and not taking time to change function names, etc. 

# Building

Open the LPCXpresso IDE and import projects [FauxControllers](.)
 and [lpc_chip_11uxx_lib](../lpc_chip_11uxx_lib).                                
                                                                                
A project can be imported into LPCXpresso by selecting:                         
 File -> Import -> General -> Existing Project into Workspace -> Select root directory
                                                                                
Run Project -> Build All to compile.

