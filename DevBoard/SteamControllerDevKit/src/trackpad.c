/**
 * \file trackpad.c
 * \brief Encompasses functions communicating with trackpad via SPI.
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

#include "trackpad.h"

#include "console.h"

#include "lpc_types.h"
#include "chip.h"
#include "ssp_11xx.h"

static LPC_SSP_T* spiRegs = LPC_SSP0;

#define GPIO_R_TRACKPAD_CS 1, 15 
#define GPIO_R_TRACKPAD_DR 0, 23 
#define GPIO_L_TRACKPAD_CS 1, 6
#define GPIO_L_TRACKPAD_DR 1, 16

/**
 * Print command usage details to console.
 *
 * \return None.
 */
static void printUsage(void) {
	consolePrint(
		"usage: trackpad\n"
		"\n"
//TODO
	);
}

/**
 * Setup all clocks, peripherals, etc. so the ADC chnanels can be read.
 *
 * \return 0 on success.
 */
void initTrackpad(void) {
	Chip_SSP_DATA_SETUP_T xf_setup;
	uint8_t tx_data[2];
	uint8_t rx_data[2];

	NVIC_SetPriority(SSP0_IRQn, 0);

	// Set PIO1_29 to function as Serial clock for SSP0
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 29, IOCON_FUNC1);

	// Set PIO0_8 to function as Master In Slave Out for SSP0
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, IOCON_FUNC1);

	// Set PIO0_9 to function as Master Out Slave In for SSP0
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, IOCON_FUNC1);

	Chip_SSP_Init(spiRegs);

	Chip_SSP_SetFormat(spiRegs, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA1_CPOL0);

	Chip_SSP_Set_Mode(spiRegs, SSP_MODE_MASTER);

	Chip_SSP_SetBitRate(spiRegs, 6000000);

	Chip_SSP_Enable(spiRegs);


	// Chip select is active low
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS, true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_R_TRACKPAD_CS);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_TRACKPAD_CS, IOCON_DIGMODE_EN |
		IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_TRACKPAD_DR, IOCON_DIGMODE_EN | 
		IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Place Right Trackpad in shutdown mode
//TODO: make Pinnacle register read and write functions (take into account multi-reads. Are there multi-writes???)
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS, false);

	xf_setup.length = 2;
	xf_setup.tx_data = tx_data;
	xf_setup.rx_data = rx_data;

	tx_data[0] = 0x83;
	tx_data[1] = 0x02;

	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS, true);


	// Chip select is active low
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS, true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_L_TRACKPAD_CS);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_CS, IOCON_DIGMODE_EN |
		IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_DR, IOCON_DIGMODE_EN | 
		IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Place Left Trackpad in shutdown mode
//TODO: make Pinnacle register read and write functions (take into account multi-reads. Are there multi-writes???)
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS, false);

	xf_setup.length = 2;
	xf_setup.tx_data = tx_data;
	xf_setup.rx_data = rx_data;

	tx_data[0] = 0x83;
	tx_data[1] = 0x02;

	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS, true);

//TODO
//	NVIC_EnableIRQ(SSP0_IRQn);
}

void SSP1_IRQHandler (void) {
	//TODO: Setup to react to SPI transaction complete?
}

// 0 - GPIO pin interrupt 0
void FLEX_INT0_IRQHandler(void) {
	//TODO: Setup to react to data ready pin rising edge?
}

/**
 * Handle trackpad query/control command line function.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int trackpadCmdFnc(int argc, const char* argv[]) {
	int retval = 0;

//TODO

	return 0;
}
