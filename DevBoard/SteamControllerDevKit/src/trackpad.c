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

#include "lpc_types.h"
#include "chip.h"
#include "ssp_11xx.h"

#include <stdio.h>

static LPC_SSP_T* spiRegs = LPC_SSP0;

typedef enum Trackpad_t {
	R_TRACKPAD,
	L_TRACKPAD
} Trackpad;

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
	printf(
		"usage: trackpad\n"
		"\n"
//TODO
	);
}

/**
 * Write to a register on the Pinnacle ASIC (i.e. the Trackpad controller).
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param addr Register address to write to.
 * \param val Value to write to register.
 *
 * \return None.
 */
static void writePinnacleReg(Trackpad trackpad, uint8_t addr, uint8_t val) {
	Chip_SSP_DATA_SETUP_T xf_setup;
	uint8_t tx_data[2];
	uint8_t rx_data[2];

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS, false);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS, false);
	}

	xf_setup.length = 2;
	xf_setup.tx_cnt = 0;
	xf_setup.tx_data = tx_data;
	xf_setup.rx_cnt = 0;
	xf_setup.rx_data = rx_data;

	// Register write indicated by setting bit 7
	tx_data[0] = 0x80 | (0x1F & addr);
	tx_data[1] = val;

	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS, true);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS, true);
	}
}

/**
 * Read a register on the Pinnacle ASIC (i.e. the Trackpad controller).
 *  TODO: What about multiple back to back reads?
 *  TODO: What about waiting for DR to be high?
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param addr Register address to read.
 *
 * \return The value read from the register.
 */
static uint8_t readPinnacleReg(Trackpad trackpad, uint8_t addr) {
	Chip_SSP_DATA_SETUP_T xf_setup;
	uint8_t tx_data[4];
	uint8_t rx_data[4];

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS, false);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS, false);
	}

	xf_setup.length = 4;
	xf_setup.tx_cnt = 0;
	xf_setup.tx_data = tx_data;
	xf_setup.rx_cnt = 0;
	xf_setup.rx_data = rx_data;

	// Register read indicated by setting bits 7 and 5
	tx_data[0] = 0xA0 | (0x1F & addr);
	// Filler bytes
	tx_data[1] = 0xFB;
	tx_data[2] = 0xFB;
	tx_data[3] = 0xFB;

	rx_data[3] = 0xF1;

	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS, true);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS, true);
	}

	return rx_data[3];
}

/**
 * Setup all clocks, peripherals, etc. so the ADC chnanels can be read.
 *
 * \return 0 on success.
 */
void initTrackpad(void) {
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
// TODO: This is what the Steam Controller does with the trackpad on startup,
//	however, the datahsheet states: "Request additional instructions from 
//	Cirque for exiting Sleep and Shutdown modes if your application requires 
//	SPI at 1 MHZ or greater." Figure this out later via further rev eng?
	writePinnacleReg(R_TRACKPAD, 0x03, 0x02);


	// Chip select is active low
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS, true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_L_TRACKPAD_CS);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_CS, IOCON_DIGMODE_EN |
		IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_DR, IOCON_DIGMODE_EN | 
		IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Place Left Trackpad in shutdown mode
//TODO: See note above 
	writePinnacleReg(L_TRACKPAD, 0x03, 0x02);


	// Test to setup Right trackpad to get data...

/*
	// Clear Flags 
	writePinnacleReg(R_TRACKPAD, 0x02, 0x00);
	// Configures SysConfig1(normal mode, active)
	writePinnacleReg(R_TRACKPAD, 0x03, 0x00);
	// Configures FeedConfig2(disable relative mode features)
	writePinnacleReg(R_TRACKPAD, 0x05, 0x1F);
	// Configures FeedConfig1(absolute mode, enable feed)
	writePinnacleReg(R_TRACKPAD, 0x04, 0x03);
	// Configures Z-idle
	writePinnacleReg(R_TRACKPAD, 0x0A, 0x05);
*/

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

	// Reset right trackpad ASIC
	writePinnacleReg(R_TRACKPAD, 0x03, 0x01);

	printf("Reset ASIC\n");

	// Sleep
	for (volatile int cnt = 0; cnt < 0x20000; cnt++) {
	}

	uint8_t status1 = 0;

	while (status1 != 0x08) {
		status1 = readPinnacleReg(R_TRACKPAD, 0x02);

		printf("status1 = 0x%02x\n", status1);
	}

	// Clear HW_DR
	writePinnacleReg(R_TRACKPAD, 0x02, 0x00);


	//TODO: sleep


	uint8_t fw_asic_id = readPinnacleReg(R_TRACKPAD, 0x00);

	printf("fw_asic_id = 0x%02x\n", fw_asic_id);

	uint8_t fw_ver = readPinnacleReg(R_TRACKPAD, 0x01);

	printf("fw_ver = 0x%02x\n", fw_ver);

	// Set undocument bit, normal mode, active, no reset
	writePinnacleReg(R_TRACKPAD, 0x03, 0x08);


	//TODO: sleep


	// Clear HW_DR
	writePinnacleReg(R_TRACKPAD, 0x02, 0x00);

	// Enable Intellimouse, etc.
	writePinnacleReg(R_TRACKPAD, 0x05, 0x00);

	// FeedConfig3
	writePinnacleReg(R_TRACKPAD, 0x06, 0x00);

	// CalConfig1
	writePinnacleReg(R_TRACKPAD, 0x07, 0x00);

	// PS/2 Aux Control
	writePinnacleReg(R_TRACKPAD, 0x08, 0x00);

	return 0;

//TODO
	uint8_t data[5];

	data[0] = readPinnacleReg(R_TRACKPAD, 0x00);
	data[1] = readPinnacleReg(R_TRACKPAD, 0x01);

	printf("Firmware ID = 0x%02x\n", data[0]);
	printf("Firmware Version = 0x%02x\n\n", data[1]);

	// Wait for trackpad to say it has new data
	while (!Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD_DR)) {
		//printf("DR Low for Right Trackpad\n");
	}

	// Clear Flags 
	writePinnacleReg(R_TRACKPAD, 0x02, 0x00);
	// Configures SysConfig1(normal mode, active)
	writePinnacleReg(R_TRACKPAD, 0x03, 0x00);
	// Configures FeedConfig2(disable relative mode features)
	writePinnacleReg(R_TRACKPAD, 0x05, 0x1F);
	// Configures FeedConfig1(absolute mode, enable feed)
	writePinnacleReg(R_TRACKPAD, 0x04, 0x03);
	// Configures Z-idle
	writePinnacleReg(R_TRACKPAD, 0x0A, 0x05);

// TODO: This shouldn't just lockup and loop like this... Provide exit capability (maybe even clearing/backspacing on screen to stop infinite scroll?)
// 	This does seem to give sane data, but it is kind of sporadic (i.e. you might get no samples, even if finger is down).
//	When samples are coming in data seems to track finger location properly. Probably more settings needed here for this to work better??
//	(i.e. sensitivity or sample timing? Look into what might be going on here? Or maybe this has to do with pull-down on GPIO being too strong?)
//
// 	Ideas of where the problem lies:
//		1. Bad Setup - Reverse engineering Valve's setup procedure for different modes (i.e. intellimouse)
//		2. Printing is leading to too long a delay in handling new data which messes things up...
while(1) {

	// Wait for trackpad to say it has new data
	while (!Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD_DR)) {
		//printf("DR Low for Right Trackpad\n");
	}

	data[0] = readPinnacleReg(R_TRACKPAD, 0x12);
	data[1] = readPinnacleReg(R_TRACKPAD, 0x14);
	data[2] = readPinnacleReg(R_TRACKPAD, 0x15);
	data[3] = readPinnacleReg(R_TRACKPAD, 0x16);
	data[4] = readPinnacleReg(R_TRACKPAD, 0x17);

/*
	printf("ABS Data Packets = 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n\n",
		data[0], data[1], data[2], data[3], data[4]);
*/

	uint16_t x = (0xF&data[3]) << 8 | data[1];
	uint16_t y = (0xF0&data[3]) << 4 | data[2];
	printf("x = %d, y = %d, z = %d\n", x, y, data[4]);

	// Clear Flags 
	writePinnacleReg(R_TRACKPAD, 0x02, 0x00);
}

	return 0;
}
