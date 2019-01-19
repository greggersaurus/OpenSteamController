/**
 * \file monitor.c
 * \brief Encompasses command for taking controller into and out of state where
 *  	USB UART prints periodic updates of state of controller (i.e. which
 *	buttons are being pressed and data being returned from peripherals).
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

#include "monitor.h"

#include "adc_read.h"
#include "buttons.h"

#include <stdio.h>

/**
 * Print command usage details to console.
 *
 * \return None.
 */
static void printUsage() {
	printf(
		"usage: monitor\n"
		"\n"
	);
}

/**
 * Handle Monitor command line function.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int monitorCmdFnc(int argc, const char* argv[]) {
/*
	//TODO: Add ability to clear terminal and loop for predetermined amount of time (or until key press) so changes are easier to see
	// Use usb_tstc (once it is available)
	char clr[] = {27, '[', '2', 'J', 0};

	printf("%s", clr);
*/

	printf("Steam Button State: %d\n", getSteamButtonState());
	printf("Front Left Button State: %d\n", getFrontLeftButtonState());
	printf("Front Right Button State: %d\n\n", getFrontRightButtonState());

	printf("Joystick Click State: %d\n\n", getJoyClickState());

	printf("X Button State: %d\n", getXButtonState());
	printf("Y Button State: %d\n", getYButtonState());
	printf("B Button State: %d\n", getBButtonState());
	printf("A Button State: %d\n\n", getAButtonState());

	printf("Right Grip Button State: %d\n", getRightGripState());
	printf("Left Grip Button State: %d\n\n", getLeftGripState());

	printf("Right Trackpad Click State: %d\n", getRightTrackpadClickState());
	printf("Left Trackpad Click State: %d\n\n", getLeftTrackpadClickState());

	printf("Right Digital Trigger State: %d\n", getRightTriggerState());
	printf("Left Digital Trigger State: %d\n\n", getLeftTriggerState());

	printf("Right Bumper State: %d\n", getRightBumperState());
	printf("Left Bumper State: %d\n\n", getLeftBumperState());

	for (int cnt = 0; cnt < 64; cnt++) {
		char* ptr_c = (char*)(0x50000000 + cnt);
		int* ptr_i = (int*)(0x50001000 + cnt*4);

		printf("0x%p: 0x%02x\t", ptr_c, *ptr_c);
		printf("0x%p: 0x%08x\n", ptr_i, *ptr_i);
	}

//TODO: Add ADC channels, etc.

	return 0;
}
