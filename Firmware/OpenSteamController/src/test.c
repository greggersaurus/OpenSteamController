/**
 * \file test.c
 * \brief Encompasses functions for regression testing different functionality
 *  (i.e. USB CDC UART).
 *
 * MIT License
 *
 * Copyright (c) 2019 Gregory Gluszek
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

#include "test.h"

#include "usb.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Command to stress test printing. This makes a very long string and then
 *  continuously prints it, with null term inserted randomly. 
 * Idea is to run this and make sure that 1) The system never locks up and
 *  2) Upon exit printing still works fine.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int testPrintCmdFnc(int argc, const char* argv[]) {
	const uint32_t STR_SZ = 2048;
	char* tst_str = malloc(STR_SZ);

	if (!tst_str) {
		printf("Could not allocate tst_str\n");
		return -1;
	}

//TODO: more options for more tests (maybe allow for selecting max string length and variability or something?
//	i.e. we want this really aggressive test to see if anything locks up and perminently screws up, but also
//	 want test to make sure things are printing in a sane manner repeatedly... (i.e. try adcRead and see strings are cut short once in a while... what is this?)
//	 Is that due to ADC interrupts?? (shouldn't it not affect UART output...?)

	// Initialize string with characters that have printable symbols
	for (int idx = 0; idx < STR_SZ; idx++) {
		tst_str[idx] = (idx % 94) + 33;
	}
	tst_str[STR_SZ-1] = 0;

	//TODO: add prompt before starting?

	//TODO: switch to using usb_tstc for exiting loop
	while (!usb_tstc()) {
		int rnd_idx = rand();	
	
		rnd_idx %= STR_SZ;	
	
		char tmp = tst_str[rnd_idx];

		// Change length of string
		tst_str[rnd_idx] = 0;

		//TODO: why are we getting bell sound...?
		printf("%s\n", tst_str);

		// Restore length of string
		tst_str[rnd_idx] = tmp;

		//TODO: Change to using sleep function (once implemented)
		for (volatile int cnt = 0; cnt < 0x4000; cnt++) {
		}
	}

	free(tst_str);

	return 0;
}

//TODO
void testCmdUsage(void) {
	printf(
		"usage: test ??? (WIP)\n"
	);
}
//TODO
int testCmdFnc(int argc, const char* argv[]) { 
	return -1;
}

/*
		//TODO: Add loopback mode for Testing for overflow in CDC UART. Remove this
		char c = usb_getc();
		usb_putc(c);
		if (c == '\r') {
			usb_putc('\n');
		} else { 
			usb_putc('_');
		}
		usb_flush();
*/

