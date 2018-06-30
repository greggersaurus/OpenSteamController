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
#include "console.h"
#include "usb.h"

/**
 * "Entry point" for Steam Controller dev kit. Keep in mind that you are most
 *  likely getting here after a call to ResetISR().
 */
int main(void){
	int retval = 0;

	// TODO: not sure if this should only be called from ResetISR() or not.
	//	 Might matter if low power modes come into play or something?
	stage1Init();

	uint32_t eeprom_data[2];

	// Read magic number and hw version from EEPROM
	retval = eepromRead(0, eeprom_data, sizeof(eeprom_data));
	if (CMD_SUCCESS != retval) {
		// Hard lock if we cannot read EEPROM
		volatile int lock = 1;
		while (lock) {}
	}

	stage2Init(eeprom_data[1]);

	// Configure USB (i.e. to act as virtual UART)
	usbConfig();
//TODO: return code check and blink LED on error?

	// Force the counter to be placed into memory
	volatile static int i = 0 ;
	// Enter an infinite loop, just incrementing a counter
	while(1) {
		i++ ;

		// Check serial input device for new characters to process
		handleConsoleInput();
	}
	return 0 ;
}
