/**
 * \file fw_cfg.h
 * \brief This encapsulates settings which dictate the intended operation of
 * 	the firmware being built. This firmware is designed to have similar
 *	core code, but be able to have the Steam Controller act as a variety
 *	of different devices. These configurations need to be compile time as
 *	the different behaviors of the device are mutually exclusive.  
 *
 * MIT License
 *
 * Copyright (c) 2018 Gregory Gluszek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _FIRMWARE_CONFIG_
#define _FIRMWARE_CONFIG_

// These define how the built firwmware causes the Steam Controller to behave
//  If you are adding a new *_FW definition make sure the value does not match
//  any of those already defined.
#define DEV_BOARD_FW (0) // This defines a firmware where the USB port acts
	// as a UART. A user can connect to the Steam Controller via USB and 
	// then have a console interface with various commands to interact
	// with different peripherals on the Steam Controller hardware.
#define SWITCH_WIRED_POWERA_FW (1) // This defines a firmwhere where the 
	// Steam Controller acts as a Wired Controller Plus (by PowerA) for
	// the Nintendo Switch. The Steam Controller can be connected to a 
	// Nintendo Switch via USB and will act as a Switch Wired controller. 

#define FIRMWARE_BEHAVIOR (DEV_BOARD_FW) // Set this to one of the *_FW defines
	// above and do a clean build of the project to change the behavior of
	// the Steam Controller.

#endif /* _FIRMWARE_CONFIG_ */


