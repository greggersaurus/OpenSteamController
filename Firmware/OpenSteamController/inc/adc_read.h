/**
 * \file adc_read.h
 * \brief Encompasses functions for reading data from ADC channels.
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

#ifndef _ADC_READ_
#define _ADC_READ_

#include <stdint.h>
#include "lpc_types.h"
#include "chip.h"
#include "adc_11xx.h"

/**
 * Defines which ADC Channels map to which functionality.
 */
typedef enum AdcChan_t {
	ADC_R_TRIG = ADC_CH2,
	ADC_L_TRIG = ADC_CH0,
	ADC_JOYSTICK_X = ADC_CH1,
	ADC_JOYSTICK_Y = ADC_CH3,
} AdcChan;

void initAdc(void);

void enableTriggers(bool en);
void enableJoystick(bool en);

void updateAdcVals(void);
uint16_t getAdcVal(AdcChan chan);

int adcReadCmdFnc(int argc, const char* argv[]);
void adcReadCmdUsage(void);

#endif /* _ADC_READ_ */
