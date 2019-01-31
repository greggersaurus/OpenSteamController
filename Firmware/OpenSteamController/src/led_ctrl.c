/**
 * \file led_ctrl.c
 * \brief Encompasses functions for controlling LED that illuminates Steam
 *	Controller button.
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

#include "led_ctrl.h"

#include "lpc_types.h"
#include "chip.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static LPC_TIMER_T* ledPwmRegs = LPC_TIMER16_1;

/**
 * Print command usage details to console.
 *
 * \return None.
 */
static void printUsage() {
	printf(
		"usage: led intensity\n"
		"\n"
		"intensity = 16-bit value indicating intensity to drive LED at\n"
	);
}

/**
 * Handle LED Control command line function.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int ledCmdFnc(int argc, const char* argv[]) {
	if (argc != 2) {
		printUsage();
		return -1;
	}

	uint32_t intensity = strtol(argv[1], NULL, 0);

	setLedIntensity(intensity);

	return 0;
}

/**
 * Setup all clocks, peripherals, etc. so Steam Controller Button LED can be
 *  controlled.
 *
 * \return 0 on success.
 */
void initLedCtrl() {
	Chip_TIMER_Init(ledPwmRegs);

	// Clear 16-bit Prescale Register (specifies the maximum value for the 
	//  Prescale Counter).
	ledPwmRegs->PR = 0;

	// Enable PWM mode for CT16B1_MAT0.
	ledPwmRegs->PWMC = 1;

	Chip_TIMER_SetMatch(ledPwmRegs, 3, 0xFFF);
	Chip_TIMER_SetMatch(ledPwmRegs, 0, 0x1000);

	Chip_TIMER_ResetOnMatchEnable(ledPwmRegs, 3);

	Chip_TIMER_Reset(ledPwmRegs);

	Chip_TIMER_Enable(ledPwmRegs);

	// Set PIO0_21 to function as CT16B1_MAT0 (Match output 0 for 16-bit timer 1).
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 21, IOCON_FUNC1);
}

/**
 * Set the intensity of the Steam Controller LED. 
 *
 * \param intensity 0 = LED off, 65535 = max intensity.
 * 
 * \return None.
 */
void setLedIntensity(uint16_t intensity) {
	Chip_TIMER_SetMatch(ledPwmRegs, 0, intensity);
}

/**
 * For cases in which we want to notify user of a fatal error, but console is
 *  dead or not present. 
 * Note: This is an infinite loop and meant for situations in which you do
 *  not want your code to continue;
 *
 * \return None.
 */
void ledSigErr(void) {
	while (1) {
		setLedIntensity(0);

		for (volatile int delay = 0; delay < 0x40000; delay++) {
		}

		setLedIntensity(0xFFF);

		for (volatile int delay = 0; delay < 0x40000; delay++) {
		}
	}
}
