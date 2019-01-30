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

#include "lpc_types.h"
#include "chip.h"
#include "adc_11xx.h"
#include "clock_11xx.h"
#include "usb.h"

#include <stdio.h>
#include <string.h>

#define GPIO_ANALOG_TRIGGER_PWR_N 1, 1 //!< Active low enable for powering 
	//!< circuitry to measure analog trigger positions.
#define GPIO_JOYSTICK_PWR 0, 19 //!< Active low enable for powering 
	//!< circuitry to measure analog trigger positions.

#define ADC_R_TRIG ADC_CH2
#define ADC_L_TRIG ADC_CH0
#define ADC_JOYSTICK_X ADC_CH1
#define ADC_JOYSTICK_Y ADC_CH3

static LPC_ADC_T* adcRegs = LPC_ADC;
static ADC_CLOCK_SETUP_T adcSetup; 

/**
 * Setup all clocks, peripherals, etc. so the ADC chnanels can be read.
 *
 * \return 0 on success.
 */
void initAdc(void) {
	NVIC_SetPriority(ADC_IRQn, 2);

	Chip_ADC_Init(adcRegs, &adcSetup);

	// Burst mode means that we will be sampling all enabled channels
	adcSetup.burstMode = 1;

	Chip_ADC_SetSampleRate(adcRegs, &adcSetup, ADC_MAX_SAMPLE_RATE);

	Chip_ADC_SetStartMode(adcRegs, ADC_NO_START, ADC_TRIGGERMODE_FALLING);

	// Start with power to analog trigger disabled
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_ANALOG_TRIGGER_PWR_N, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, GPIO_ANALOG_TRIGGER_PWR_N, true);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_ANALOG_TRIGGER_PWR_N, 
		IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Start with power to joystick disabled
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_JOYSTICK_PWR, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, GPIO_JOYSTICK_PWR, true);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_JOYSTICK_PWR, IOCON_MODE_PULLDOWN, 
		IOCON_FUNC0);

	// Set PIO0_22 to function as AD6: (possibly) Battery/Power Level?
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_FUNC1);
	// Set PIO0_13 to function as AD2: Right Analog Trigger Position
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 13, IOCON_FUNC2);
	// Set PIO0_11 to function as AD0: Left Analog Trigger Position
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, IOCON_FUNC2);
	// Set PIO0_12 to function as AD1: Analog Joystick X Position
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 12, IOCON_FUNC2);
	// Set PIO0_14 to function as AD3: Analog Joystick X Position
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 14, IOCON_FUNC2);

	// AD6 is always enabled for conversion
	Chip_ADC_EnableChannel(adcRegs, ADC_CH6, ENABLE);

	// Enable AD6 to generate interrupt. Since will be running in burst
	//  mode where all enabled channels are sampled, we only need one to 
	//  generate interrupt
	Chip_ADC_Int_SetChannelCmd(adcRegs, ADC_CH6, ENABLE);

	// Disable clock to ADC until we need to update readings
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_ADC);
	
	NVIC_EnableIRQ(ADC_IRQn);
}

/**
 * Control for enabling/disabling analog trigger measurements.
 *
 * \param en True to enable ADC channels related to triggers, false to disable.
 * 
 * \return None.
 */
void enableTriggers(bool en) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ADC);

	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_ANALOG_TRIGGER_PWR_N, !en);

	Chip_ADC_EnableChannel(adcRegs, ADC_R_TRIG, en?ENABLE:DISABLE);
	Chip_ADC_EnableChannel(adcRegs, ADC_L_TRIG, en?ENABLE:DISABLE);

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_ADC);
}

/**
 * Control for enabling/disabling Joystick X and Y position measurements.
 *
 * \param en True to enable ADC channels related to Joystick, false to disable.
 * 
 * \return None.
 */
void enableJoystick(bool en) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ADC);

	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_JOYSTICK_PWR, en);

	Chip_ADC_EnableChannel(adcRegs, ADC_JOYSTICK_X, en?ENABLE:DISABLE);
	Chip_ADC_EnableChannel(adcRegs, ADC_JOYSTICK_Y, en?ENABLE:DISABLE);

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_ADC);
}

static volatile uint16_t adcData[8]; //!< Stores most recently averaged ADC
	//!< values. Call updateAdcVals() to update this.

static volatile int adcUpdateCnt = 0; //!< Used to count how many times ADC data is 
	//!< accumulated in ISR.

static const int ADC_UPDATE_CNT_DONE = 8; //!< Defines when ISR has fired enough
	//!< times and adcData is up to date with latest ADC readings.

/**
 * This will start new conversion of all enabled ADC channels, and not return
 *  until 
 */
void updateAdcVals(void) {
	// Clear counter used by ISR
	adcUpdateCnt = 0; 

	// Clear adcData
	memset((void*)adcData, 0, sizeof(adcData));

	// Enable clock to ADC
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ADC);

	// Enable burst mode to start conversions
	Chip_ADC_SetBurstCmd(adcRegs, ENABLE);

	// Wait for ADC samples to be accumulated and averaged
	while (adcUpdateCnt < ADC_UPDATE_CNT_DONE) {
		__WFI();
	}
}

/**
 * Interrupt handler for ADC component. This is called when an ADC conversion 
 *  has completed.
 *
 * \return None.
 */
void ADC_IRQHandler (void) {
	if (adcUpdateCnt < ADC_UPDATE_CNT_DONE)
		adcUpdateCnt++;

	if (adcUpdateCnt == ADC_UPDATE_CNT_DONE) {
		// We are at end of averaging cycle, so stop conversions
		Chip_ADC_SetBurstCmd(adcRegs, DISABLE);
	}

	// Accumulate samples
	for (int idx = 0; idx < 8; idx++) {
		uint16_t retval = 0;
		if (SUCCESS == Chip_ADC_ReadValue(adcRegs, idx, &retval))
			adcData[idx] += retval;
	}

	if (adcUpdateCnt == ADC_UPDATE_CNT_DONE) {
		// Divide accumulated values to get average
		for (int idx = 0; idx < 8; idx++) {
			adcData[idx] /= ADC_UPDATE_CNT_DONE;
		}

		// Shutdown ADC until next request for update
		Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_ADC);
	}
}

/**
 * \param chan ADC channel to retrieve data from.
 * 
 * \return Most recently aquired data from specified ADC channel.
 */
uint16_t adcReadChan(uint8_t chan) {

//TODO: return status code instead?

	if (chan < 8)
		return adcData[chan];

	return 0;
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

/**
 * Print command usage details to console.
 *
 * \return None.
 */
void adcReadCmdUsage(void) {
	printf(
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

//TODO

while (!usb_tstc()) {
	updateAdcVals();

	printf("ADC[6] = 0x%04x, ", adcReadChan(ADC_CH6));

	printf("ADC[0] = 0x%04x, ", adcReadChan(ADC_CH0));
	printf("ADC[2] = 0x%04x, ", adcReadChan(ADC_CH2));

	// Joystick X direction (left = 0x338, neutral = 0x20a right = 0x0f0)
	printf("ADC[1] = 0x%04x, ", adcReadChan(ADC_CH1));
	// Joystick Y direction (up = 0x32a, neutral = 0x207, down = 0xf8)
	printf("ADC[3] = 0x%04x\n", adcReadChan(ADC_CH3));
}

	return 0;
}

