/**
 * \file adc_read.c
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

#include "adc_read.h"

#include "console.h"

#include "lpc_types.h"
#include "chip.h"
#include "adc_11xx.h"

static LPC_ADC_T* adcRegs = LPC_ADC;
static ADC_CLOCK_SETUP_T adcSetup; 

/**
 * Print command usage details to console.
 *
 * \return None.
 */
static void printUsage() {
	consolePrint(
		"usage: adc\n"
		"\n"
//TODO
	);
}

/**
 * Handle ADC Read command line function.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int adcReadCmdFnc(int argc, const char* argv[]) {
	int retval = 0;

//TODO

	consolePrint("ADC Channel 6 Reads 0x%04x\n", adcReadChan(ADC_CH6));

	return 0;
}

/**
 * Setup all clocks, peripherals, etc. so the ADC chnanels can be read.
 *
 * \return 0 on success.
 */
void initAdc() {

//	NVIC_SetPriority(ADC_IRQn, 2);

	Chip_ADC_Init(adcRegs, &adcSetup);

	adcSetup.burstMode = 1;

	Chip_ADC_SetSampleRate(adcRegs, &adcSetup, 400000);

//	Chip_ADC_SetStartMode(adcRegs, ADC_NO_START, ADC_TRIGGERMODE_RISING);

	// Set PIO0_22 to function as AD6
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_FUNC1);

	// Specify AD pin to be sampled and converted
	Chip_ADC_EnableChannel(adcRegs, ADC_CH6, ENABLE);

	// Enable AD6 to generate interrupt
//	Chip_ADC_Int_SetChannelCmd(adcRegs, ADC_CH6, ENABLE);
	
//	NVIC_EnableIRQ(ADC_IRQn);

// TODO: extract code from the following functions and beyond 
	//?? fnc0x00002cf0( arg0x00002cf0_0, arg0x00002cf0_1, arg0x00002cf0_2, arg0x00002cf0_3, arg0x00002cf0_4, arg0x00002cf0_5, arg0x00002cf0_6, arg0x00002cf0_12, )
}

/**
 * \param chan ADC channel to retrieve data from.
 * 
 * \return Most recently aquired data from specified ADC channel.
 */
uint16_t adcReadChan(uint8_t chan) {
	uint16_t retval = 0;

	//TODO: return ?? if ADC channel is not enabled??

	Chip_ADC_SetStartMode(adcRegs, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

	while (Chip_ADC_ReadStatus(adcRegs, chan, ADC_DR_DONE_STAT) != SET) {}

	/* Read ADC value */
	Chip_ADC_ReadValue(adcRegs, chan, &retval);

	return retval;
}
