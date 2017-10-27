/**
 * \file SteamControllerDevKit.c
 * \brief Main for Steam Controller Development Kit. The idea is to 
 *	take advante of the relatively open nature of the Steam Controller and
 *	provide a basis for people to tinker and interface with the variety of
 *	perihperals on the Steam Controller Hardware. 
 *
 * MIT License
 *
 * Copyright (c) 2017 Gregory Gluszek
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

#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif

#include <cr_section_macros.h>

#include <stdint.h>

#include "init.h"
#include "eeprom_access.h"
#include "usb.h"

/**
 * "Entry point" for Steam Controller dev kit. Keep in mind that you are most
 *  likely getting here after a call to ResetISR().
 */
int main(void){
	int retval = 0;
	volatile uint32_t* reg32 = (volatile uint32_t)0;
	uint32_t val = 0;

	// TODO: not sure if this should only be called from ResetISR() or not.
	//	 Might matter if low power modes come into play or something?
	stage1Init();

	uint32_t eeprom_data[2];

	// Read magic number and hw version from EEPROM
	retval = eeprom_read(0, eeprom_data, sizeof(eeprom_data));
//TODO: check retval

//TODO: pass in hw version read from EEPROM (eeprom_data[1])
	stage2Init(8);

	// Configure USB to act as virtual UART
	usbConfig();


//TODO: the following is simple code to prove firmware is running. Replace this with something more interesting...
	// PIO0_21 is connected to button LED

	// Set PIO0_21 output bit
	*((uint8_t*)0x50000015) = 0;

	// Set PIO0_21 to output via GPIO direction port 1 register
	reg32 = (volatile uint32_t*)0x50002000;
	val = *reg32;
	val |= 0x00200000;
	*reg32 = val;

	// Force the counter to be placed into memory
	volatile static int i = 0 ;
	// Enter an infinite loop, just incrementing a counter
	while(1) {
		i++ ;
		// When Y button is pressed Steam Button LED will light
		*((uint8_t*)0x50000015) = !(*((uint8_t*)0x5000002B));
	}
	return 0 ;
}
