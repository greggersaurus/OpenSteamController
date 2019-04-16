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
#include "trackpad.h"
#include "usb.h"
#include "time.h"

#include <stdio.h>

/**
 * Print command usage details to console.
 *
 * \return None.
 */
void monitorCmdUsage(void) {
	printf(
		"usage: monitor\n"
		"\n"
		"Enter a loop giving updates on all controller inputs.\n"
		"Press any key to exit loop.\n"
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
	// Sequence to clear terminal (as we will be updating more than one line...)
	const char const clr[] = {27, '[', '2', 'J', 0};

	const char empty_x_str[10] = "         ";

	while (!usb_tstc()) {
		usb_flush();

		usleep(100 * 1000);

		printf("%s", clr);

		updateAdcVals();
		trackpadLocUpdate(L_TRACKPAD);
		trackpadLocUpdate(R_TRACKPAD);

		printf("Monitoring Steam Controller. Time = 0x%08x. (Press any key to exit):\n", 
			getUsTickCnt());

		uint16_t adc_l_trig = getAdcVal(ADC_L_TRIG);
		uint16_t adc_r_trig = getAdcVal(ADC_R_TRIG);

		printf("%s                                                             %s\n", 
			adc_l_trig > 400 ? " == " : "    ", 
			adc_r_trig > 400 ? " == " : "    ");
		printf("%s                                                             %s\n", 
			adc_l_trig > 300 ? " == " : "    ", 
			adc_r_trig > 300 ? " == " : "    ");
		printf("%s                                                             %s\n", 
			adc_l_trig > 200 ? " == " : "    ", 
			adc_r_trig > 200 ? " == " : "    ");
		printf("%s                                                             %s\n", 
			adc_l_trig > 100 ? " == " : "    ", 
			adc_r_trig > 100 ? " == " : "    ");
		printf("%s                                                             %s\n", 
			getLeftTriggerState() ? "[LT]" : " LT ", 
			getRightTriggerState() ? "[RT]" : " RT ");
		printf("%s                                                             %s\n", 
			getLeftBumperState() ? "[LB]" : " LB ", 
			getRightBumperState() ? "[RB]" : " RB ");


		char tpad_l_x_str[10] = "         ";
		uint16_t tpad_l_x = 0;
		uint16_t tpad_l_y = 0;
		trackpadGetLastXY(L_TRACKPAD, &tpad_l_x, &tpad_l_y);
		int tpad_l_x_idx = (tpad_l_x / 100) * 9  /12;
		tpad_l_x_str[tpad_l_x_idx] = '=';

		char tpad_r_x_str[10] = "         ";
		uint16_t tpad_r_x = 0;
		uint16_t tpad_r_y = 0;
		trackpadGetLastXY(R_TRACKPAD, &tpad_r_x, &tpad_r_y);
		int tpad_r_x_idx = (tpad_r_x / 100) * 9  /12;
		tpad_r_x_str[tpad_r_x_idx] = '=';

		printf(" %s                                                  %s\n", 
			tpad_l_y > 600 ? tpad_l_x_str : empty_x_str,
			tpad_r_y > 600 ? tpad_r_x_str : empty_x_str);
		printf(" %s                                                  %s\n", 
			tpad_l_y <= 600 && tpad_l_y > 500? tpad_l_x_str : empty_x_str,
			tpad_r_y <= 600 && tpad_r_y > 500? tpad_r_x_str : empty_x_str);
		printf(" %s                                                  %s\n", 
			tpad_l_y <= 500 && tpad_l_y > 400? tpad_l_x_str : empty_x_str,
			tpad_r_y <= 500 && tpad_r_y > 400? tpad_r_x_str : empty_x_str);

		printf("%c%s%c                  %s%s%s                        %c%s%c\n", 
			getLeftTrackpadClickState() ? '[' : ' ',
			tpad_l_y <= 400 && tpad_l_y > 300? tpad_l_x_str : empty_x_str,
			getLeftTrackpadClickState() ? ']' : ' ',
			getFrontLeftButtonState() ? "[<]" : " < ", 
			getSteamButtonState() ? "[S]" : " S ", 
			getFrontRightButtonState() ? "[>]" : " > ",
			getRightTrackpadClickState() ? '[' : ' ',
			tpad_r_y <= 400 && tpad_r_y > 300? tpad_r_x_str : empty_x_str,
			getRightTrackpadClickState() ? ']' : ' ');

		printf(" %s                                                  %s\n", 
			tpad_l_y <= 300 && tpad_l_y > 200? tpad_l_x_str : empty_x_str,
			tpad_r_y <= 300 && tpad_r_y > 200? tpad_r_x_str : empty_x_str);
		printf(" %s                                                  %s\n", 
			tpad_l_y <= 200 && tpad_l_y > 100? tpad_l_x_str : empty_x_str,
			tpad_r_y <= 200 && tpad_r_y > 100? tpad_r_x_str : empty_x_str);
		printf(" %s                                                  %s\n", 
			tpad_l_y <= 100 && tpad_l_y > 0? tpad_l_x_str : empty_x_str,
			tpad_r_y <= 100 && tpad_r_y > 0? tpad_r_x_str : empty_x_str);


		char joy_x_str[10] = "         ";
		uint16_t adc_joy_x = getAdcVal(ADC_JOYSTICK_X);
		uint16_t adc_joy_y = getAdcVal(ADC_JOYSTICK_Y);
		joy_x_str[9 - adc_joy_x/100] = '=';

		printf("         %s\n", 
			adc_joy_y > 700? joy_x_str : empty_x_str);
		printf("         %s                          %s\n", 
			adc_joy_y <= 700 && adc_joy_y > 600? joy_x_str : empty_x_str,
			getYButtonState() ? "[Y]" : " Y ");
		printf("        %c%s%c                      %s   %s\n", 
			getJoyClickState() ? '[' : ' ',
			adc_joy_y <= 600 && adc_joy_y > 500? joy_x_str : empty_x_str,
			getJoyClickState() ? ']' : ' ',
			getXButtonState() ? "[X]" : " X ",
			getBButtonState() ? "[B]" : " B ");
		printf("         %s                          %s\n", 
			adc_joy_y <= 500 && adc_joy_y > 400? joy_x_str : empty_x_str,
			getAButtonState() ? "[A]" : " A ");
		printf("         %s\n", 
			adc_joy_y <= 400 && adc_joy_y > 300? joy_x_str : empty_x_str);
		printf("         %s\n", 
			adc_joy_y <= 300 && adc_joy_y > 200? joy_x_str : empty_x_str);
		printf("         %s\n", 
			adc_joy_y <= 200 && adc_joy_y > 100? joy_x_str : empty_x_str);
		printf("         %s\n", 
			adc_joy_y <= 100 && adc_joy_y > 0? joy_x_str : empty_x_str);

		printf("       %s                                          %s\n", 
			getLeftGripState() ? "[LG]" : " LG ",
			getRightGripState() ? "[RG]" : " RG ");

	}

	return 0;
}
