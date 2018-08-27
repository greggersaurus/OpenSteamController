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
static void printUsage(void) {
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

	consolePrint("ADC Channel 6 Reads 0x%04x\n\n", adcReadChan(ADC_CH6));

	consolePrint("ADC Channel 0 Reads 0x%04x\n", adcReadChan(ADC_CH0));
	consolePrint("ADC Channel 2 Reads 0x%04x\n\n", adcReadChan(ADC_CH2));

	// Joystick X direction (left = 0x338, neutral = 0x20a right = 0x0f0)
	consolePrint("ADC Channel 1 Reads 0x%04x\n", adcReadChan(ADC_CH1));
	// Joystick Y direction (up = 0x32a, neutral = 0x207, down = 0xf8)
	consolePrint("ADC Channel 3 Reads 0x%04x\n\n", adcReadChan(ADC_CH3));

	return 0;
}

/**
 * Setup all clocks, peripherals, etc. so the ADC chnanels can be read.
 *
 * \return 0 on success.
 */
void initAdc(void) {

// Try setting all of these manually, maybe something is not getting calculted to give a messed up clock rate or something?
	NVIC_SetPriority(ADC_IRQn, 0);

	Chip_ADC_Init(adcRegs, &adcSetup);

	adcSetup.burstMode = 1;

	Chip_ADC_SetSampleRate(adcRegs, &adcSetup, 400000);

	Chip_ADC_SetStartMode(adcRegs, ADC_NO_START, ADC_TRIGGERMODE_FALLING);

	// Set PIO0_22 to function as AD6
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_FUNC1);

	// Specify AD pin to be sampled and converted
	Chip_ADC_EnableChannel(adcRegs, ADC_CH6, ENABLE);

	// Set PIO0_13 to function as AD2
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 13, IOCON_FUNC2);

	// Specify AD pin to be sampled and converted
	Chip_ADC_EnableChannel(adcRegs, ADC_CH2, ENABLE);

	// Set PIO0_11 to function as AD0
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, IOCON_FUNC2);

	// Specify AD pin to be sampled and converted
	Chip_ADC_EnableChannel(adcRegs, ADC_CH0, ENABLE);

	// Set PIO0_12 to function as AD1
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 12, IOCON_FUNC2);

	// Specify AD pin to be sampled and converted
	Chip_ADC_EnableChannel(adcRegs, ADC_CH1, ENABLE);

	// Set PIO0_14 to function as AD3
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 14, IOCON_FUNC2);

	// Specify AD pin to be sampled and converted
	Chip_ADC_EnableChannel(adcRegs, ADC_CH3, ENABLE);

	// Enable AD6 to generate interrupt
	Chip_ADC_Int_SetChannelCmd(adcRegs, ADC_CH6, ENABLE);
	
	NVIC_EnableIRQ(ADC_IRQn);
}

static volatile uint16_t adcData[8] = {
	0xDEAD,	
	0xDEAD,	
	0xDEAD,	
	0xDEAD,	
	0xDEAD,	
	0xDEAD,	
	0xDEAD,	
	0xDEAD
};

void ADC_IRQHandler (void) {
	static int blah = 0;
	blah++;
/*
	if (blah > 8) {
//TODO: this shouldn't be necessary, right? This is here so IRQ does not starve user thread (i.e. UART I/O) from running...
		Chip_ADC_SetBurstCmd(adcRegs, DISABLE);
		blah = 0;
	}
*/
	for (int cnt = 0; cnt < 8; cnt++) {
		uint16_t retval = 0;
		if (SUCCESS == Chip_ADC_ReadValue(adcRegs, cnt, &retval))
			adcData[cnt] = retval;
	}
}

/**
 * \param chan ADC channel to retrieve data from.
 * 
 * \return Most recently aquired data from specified ADC channel.
 */
uint16_t adcReadChan(uint8_t chan) {

//TODO: return status code instead?

// TODO: FIX THIS!!!
//	There is an issue with IRQs and the console. It's better now that WFI
//	 was added, but I think I have some sauce in the console processing, 
//	 etc. that does not deal with interrupts well.... Leaving this scrappy
//	 for now, but need to fix up soon!
	Chip_ADC_SetBurstCmd(adcRegs, ENABLE);

// This GPIO enables joystick to actually generate ADC values
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 19, true);

	if (chan < 8)
		return adcData[chan];

	return 0xDEAD;
}

/**
 * Convert ADC reading for X direction of analog stick to bounds expected by
 *  Wired Controller Plus (by PowerA) for Nintendo Switch.
 *
 * \return X position of Analog stick where Left=0x00, Neutral=0x80, Right=0xff
 */
uint8_t getleftAnalogXPowerA(void) {
	// Joystick X direction (left = 0x338, neutral = 0x20a right = 0x0f0)
	uint16_t adcVal = adcReadChan(ADC_CH1);

	if (adcVal < 0x100) {
		adcVal = 0;
	} else {
		adcVal -= 0x100;
	}

	adcVal >>= 1;

	if (adcVal > 0xff) {
		adcVal = 0xff;
	} else if (adcVal < 0x90 && adcVal > 0x70) {
		adcVal = 0x80;
	} else if (adcVal < 0x08) {
		adcVal = 0x00;
	}

	return ~adcVal;
}

/**
 * Convert ADC reading for Y direction of analog stick to bounds expected by
 *  Wired Controller Plus (by PowerA) for Nintendo Switch.
 *
 * \return X position of Analog stick where Up=0x00, Neutral=0x80, Down=0xff
 */
uint8_t getleftAnalogYPowerA(void) {
	// Joystick Y direction (up = 0x32a, neutral = 0x207, down = 0xf8)
	uint16_t adcVal = adcReadChan(ADC_CH3);

	if (adcVal < 0x100) {
		adcVal = 0;
	} else {
		adcVal -= 0x100;
	}

	adcVal >>= 1;

	if (adcVal > 0xff) {
		adcVal = 0xff;
	} else if (adcVal < 0x90 && adcVal > 0x70) {
		adcVal = 0x80;
	} else if (adcVal < 0x08) {
		adcVal = 0x00;
	}

	return ~adcVal;
}
