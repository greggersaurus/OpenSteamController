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
#include "time.h"

#include <stdio.h>

static LPC_SSP_T* spiRegs = LPC_SSP0;

typedef enum Trackpad_t {
	R_TRACKPAD = 0,
	L_TRACKPAD = 1
} Trackpad;

#define GPIO_SSP0_SCK0 1, 29
#define GPIO_SSP0_MISO0 0, 8
#define GPIO_SSP0_MOSI0 0, 9

#define GPIO_R_TRACKPAD_CS_N 1, 15 
#define GPIO_R_TRACKPAD_DR 0, 23 
#define GPIO_L_TRACKPAD_CS_N 1, 6
#define GPIO_L_TRACKPAD_DR 1, 16

//TODO: revisit these reg definitions are it looks like Trackpad is being configured in AnyMeas mode
#define TPAD_REG_FW_ID 0x00
#define TPAD_REG_FW_VER 0x01
#define TPAD_REG_STATUS_1 0x02
#define TPAD_REG_SYS_CFG_1 0x03
#define TPAD_REG_FEED_CFG_1 0x04
#define TPAD_REG_FEED_CFG_2 0x05
#define TPAD_REG_FEED_CFG_3 0x06
#define TPAD_REG_CAL_CFG_1 0x07
#define TPAD_REG_PS2_AUX_CTRL 0x08
#define TPAD_REG_SAMPLE_RATE 0x09
#define TPAD_REG_Z_IDLE 0x0A
#define TPAD_REG_Z_SCALAR 0x0B
#define TPAD_REG_SLEEP_INTV 0x0C
#define TPAD_REG_SLEEP_TIMER 0x0D
#define TPAD_REG_DYN_EMI 0x0E

#define TPAD_REG_GPIO_A_CTRL 0x18
#define TPAD_REG_GPIO_A_DATA 0x19
#define TPAD_REG_GPIO_B 0x1A

#define TPAD_REG_ERA_VAL 0x1B
#define TPAD_REG_ERA_H_ADDR 0x1C
#define TPAD_REG_ERA_L_ADDR 0x1D
#define TPAD_REG_ERA_CTRL 0x1E

#define TPAD_REG_PROD_ID 0x1F

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
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, false);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, false);
	}

	// Number of words to transfer
	xf_setup.length = 2;
	// Used to count how many words have been transmitted
	xf_setup.tx_cnt = 0;
	xf_setup.tx_data = tx_data;
	// Used to count how many words have been received
	xf_setup.rx_cnt = 0;
	xf_setup.rx_data = rx_data;

	// Register write indicated by setting bit 7
	tx_data[0] = 0x80 | (0x1F & addr);
	tx_data[1] = val;

	// TODO: should this be done by hand? Maybe this is why we are getting poor results from the Trackpad ASIC?
	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, true);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, true);
	}
}

/**
 * Read a register on the Pinnacle ASIC (i.e. the Trackpad controller).
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
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, false);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, false);
	}

	// Number of words to transfer
	xf_setup.length = 4;
	// Used to count how many words have been transmitted
	xf_setup.tx_cnt = 0;
	xf_setup.tx_data = tx_data;
	// Used to count how many words have been received
	xf_setup.rx_cnt = 0;
	xf_setup.rx_data = rx_data;

	// Register read indicated by setting bits 7 and 5
	tx_data[0] = 0xA0 | (0x1F & addr);
	// Filler bytes
	tx_data[1] = 0xFB;
	tx_data[2] = 0xFB;
	tx_data[3] = 0xFB;

	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, true);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, true);
	}

	return rx_data[3];
}

/**
 * Trackpad ASIC Extended Register Access (ERA) Write with Address Increment
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param addr 16-bit extended register address.
 * \param len Number of bytes to be sequentially written.
 * \param[in] data Pointer to data to be written.
 *
 * \return None.
 */
static void writePinnacleExtRegs(Trackpad trackpad, uint16_t addr, uint8_t len, 
	const uint8_t* data) {

	// Write address 
	writePinnacleReg(trackpad, TPAD_REG_ERA_H_ADDR, 0xFF & (addr >> 8));
	writePinnacleReg(trackpad, TPAD_REG_ERA_L_ADDR, 0xFF & addr);

	for (int idx = 0; idx < len; idx++) {
		// Write value
		writePinnacleReg(trackpad, TPAD_REG_ERA_VAL, data[idx]);

		// Write ERA auto-increment write to ERA Control
		writePinnacleReg(trackpad, TPAD_REG_ERA_CTRL, 0x0A);

		// Read ERA Control until it contains 0x00
		while (readPinnacleReg(trackpad, TPAD_REG_ERA_CTRL)){
		}
	}
}

//TODO: rename. This is recreation of fnc0x0000573c(). Read 0x12 and 0x11 and clear flags, all in one SPI transaction
// I think this is getting the raw ADC value (given the Trackpad was setup in AnyMeas mode)
//  Question is understanding how to switch which ADC is being sampled...
static uint16_t rapidReadAndClear(Trackpad trackpad) {
	Chip_SSP_DATA_SETUP_T xf_setup;
	uint8_t tx_data[7];
	uint8_t rx_data[7];

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, false);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, false);
	}

	// Number of words to transfer
	xf_setup.length = 7;
	// Used to count how many words have been transmitted
	xf_setup.tx_cnt = 0;
	xf_setup.tx_data = tx_data;
	// Used to count how many words have been received
	xf_setup.rx_cnt = 0;
	xf_setup.rx_data = rx_data;

	// Auto-incremented read starting at register 0x11
	tx_data[0] = 0xA0 | 0x11;
	tx_data[1] = 0xFC;
	tx_data[2] = 0xFC;
	tx_data[3] = 0xFC;
	tx_data[4] = 0xFB;
	// Clear flags
	tx_data[5] = 0x80 | 0x02;
	tx_data[6] = 0x00;

	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, true);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, true);
	}

	// Concatenate 0x12 and 0x11 into a single 16-bit word
	return (rx_data[4] << 8) | rx_data[3];
}

/**
 * Setup all clocks, peripherals, etc. so the ADC chnanels can be read.
 *
 * \return 0 on success.
 */
void initTrackpad(void) {
	// Set Interrupt Priority for SSP0 to one below highest (even though
	//  we don't use SSP0 interrupts...)
	NVIC_SetPriority(SSP0_IRQn, 1);

	// Setup SSP0 pins
	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_SSP0_SCK0, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_SSP0_MISO0, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, GPIO_SSP0_MOSI0, IOCON_FUNC1);

	// Configure SPI
	Chip_SSP_Init(spiRegs);
	Chip_SSP_SetFormat(spiRegs, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, 
		SSP_CLOCK_CPHA1_CPOL0);
	Chip_SSP_Set_Mode(spiRegs, SSP_MODE_MASTER);
	Chip_SSP_SetBitRate(spiRegs, 6000000);
	Chip_SSP_Enable(spiRegs);

	// Right Trackpad comms setup
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, true);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_R_TRACKPAD_CS_N);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_TRACKPAD_CS_N, IOCON_DIGMODE_EN |
		IOCON_MODE_INACT, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_TRACKPAD_DR, IOCON_DIGMODE_EN | 
		IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Place Right Trackpad in shutdown mode
	writePinnacleReg(R_TRACKPAD, TPAD_REG_SYS_CFG_1, 0x02);

	// Left Trackpad comms setup
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_L_TRACKPAD_CS_N);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_CS_N, IOCON_DIGMODE_EN |
		IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_DR, IOCON_DIGMODE_EN | 
		IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Place Left Trackpad in shutdown mode
	writePinnacleReg(L_TRACKPAD, TPAD_REG_SYS_CFG_1, 0x02);


	// Place the Trackpad in reset
	writePinnacleReg(R_TRACKPAD, TPAD_REG_SYS_CFG_1, 0x01);

	usleep(50 * 1000);

	// Wait for Software Command Complete SW_CC
	while (!(0x08 & readPinnacleReg(R_TRACKPAD, 0x02))) {
	}

	// Clear flags
	writePinnacleReg(R_TRACKPAD, TPAD_REG_STATUS_1, 0x00);

	usleep(10 * 1000);

	// Read firmware ASIC ID (should be 0x07)
// TODO: what if it's not?!
	readPinnacleReg(R_TRACKPAD, TPAD_REG_FW_ID);

	// Read Firmware Revision Number (should be 0x3A)
// TODO: what if it's not?!
	readPinnacleReg(R_TRACKPAD, TPAD_REG_FW_VER);
	
	// Track disable
	writePinnacleReg(R_TRACKPAD, TPAD_REG_SYS_CFG_1, 0x08);

	usleep(10 * 1000);
	
	// Clear flags
	writePinnacleReg(R_TRACKPAD, TPAD_REG_STATUS_1, 0x00);
	
	// Default settings:
	writePinnacleReg(R_TRACKPAD, TPAD_REG_FEED_CFG_2, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_FEED_CFG_3, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_CAL_CFG_1, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_PS2_AUX_CTRL, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_SAMPLE_RATE, 0x00);
	// TODO: can get more data on these registers??
	writePinnacleReg(R_TRACKPAD, 0x13, 0x00);
	writePinnacleReg(R_TRACKPAD, 0x14, 0x00);
	writePinnacleReg(R_TRACKPAD, 0x15, 0x00);
	writePinnacleReg(R_TRACKPAD, 0x16, 0x00);
	writePinnacleReg(R_TRACKPAD, 0x17, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_GPIO_A_CTRL, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_GPIO_A_DATA, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_GPIO_B, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_Z_IDLE, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_Z_SCALAR, 0x13);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_DYN_EMI, 0x41);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_SLEEP_INTV, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_SLEEP_TIMER, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_PS2_AUX_CTRL, 0x00);

	// Load up Compensation Matrix data??

	uint8_t data[8];

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x07;
	data[3] = 0xf8;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x05;
	data[7] = 0x50;
	writePinnacleExtRegs(R_TRACKPAD, 0x015B, 8, data);

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x07;
	data[3] = 0xf8;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x06;
	data[7] = 0x60;
	writePinnacleExtRegs(R_TRACKPAD, 0x0163, 8, data);

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x07;
	data[3] = 0xf8;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x04;
	data[7] = 0xc8;
	writePinnacleExtRegs(R_TRACKPAD, 0x016B, 8, data);

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x07;
	data[3] = 0xf8;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x07;
	data[7] = 0x80;
	writePinnacleExtRegs(R_TRACKPAD, 0x0173, 8, data);

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x07;
	data[3] = 0xf8;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x05;
	data[7] = 0x28;
	writePinnacleExtRegs(R_TRACKPAD, 0x017B, 8, data);

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x07;
	data[3] = 0xf8;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x06;
	data[7] = 0x18;
	writePinnacleExtRegs(R_TRACKPAD, 0x0183, 8, data);

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x07;
	data[3] = 0xf8;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x04;
	data[7] = 0xB0;
	writePinnacleExtRegs(R_TRACKPAD, 0x018B, 8, data);

	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x0193, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x02;
	data[5] = 0x3B;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x01DF, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x04;
	data[5] = 0x76;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x01E7, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0xED;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x01EF, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x01;
	data[5] = 0xDA;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x01F7, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x03;
	data[5] = 0xB4;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x01FF, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x07;
	data[5] = 0x68;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x0207, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x06;
	data[5] = 0xD1;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x020F, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x05;
	data[5] = 0xA3;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x0217, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x03;
	data[5] = 0x47;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x021F, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x06;
	data[5] = 0x8E;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x0227, 8, data);

	data[0] = 0x0F;
	data[1] = 0xFF;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x05;
	data[5] = 0x1D;
	data[6] = 0x00;
	data[7] = 0x00;
	writePinnacleExtRegs(R_TRACKPAD, 0x022F, 8, data);

	writePinnacleReg(R_TRACKPAD, TPAD_REG_FEED_CFG_2, 0xc2);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_FEED_CFG_3, 0x02);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_CAL_CFG_1, 0x01);
	// 40 Sample/Second
	writePinnacleReg(R_TRACKPAD, TPAD_REG_SAMPLE_RATE, 0x28);

	data[0] = 0x64;
	data[1] = 0x03;
	writePinnacleExtRegs(R_TRACKPAD, 0x00D8, 2, data);

	writePinnacleReg(R_TRACKPAD, TPAD_REG_STATUS_1, 0x00);

	writePinnacleReg(R_TRACKPAD, TPAD_REG_SYS_CFG_1, 0x18);
	
	usleep(2 * 1000);

	readPinnacleReg(R_TRACKPAD, 0x11);
	readPinnacleReg(R_TRACKPAD, 0x12);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_STATUS_1, 0x00);

	writePinnacleReg(R_TRACKPAD, 0x15, 0x07);
	writePinnacleReg(R_TRACKPAD, 0x16, 0xf8);

	writePinnacleReg(R_TRACKPAD, TPAD_REG_GPIO_A_DATA, 0x05);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_GPIO_B, 0x50);

	writePinnacleReg(R_TRACKPAD, TPAD_REG_SYS_CFG_1, 0x18);

	usleep(2 * 1000);

	readPinnacleReg(R_TRACKPAD, 0x11);
	readPinnacleReg(R_TRACKPAD, 0x12);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_STATUS_1, 0x00);

	writePinnacleReg(R_TRACKPAD, 0x13, 0x0F);
	writePinnacleReg(R_TRACKPAD, 0x14, 0xFF);
	writePinnacleReg(R_TRACKPAD, 0x15, 0x00);
	writePinnacleReg(R_TRACKPAD, 0x16, 0x00);
	writePinnacleReg(R_TRACKPAD, 0x17, 0x02);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_GPIO_A_CTRL, 0x3B);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_GPIO_A_DATA, 0x00);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_GPIO_B, 0x00);
	
	writePinnacleReg(R_TRACKPAD, TPAD_REG_SYS_CFG_1, 0x18);

	usleep(2 * 1000);

	readPinnacleReg(R_TRACKPAD, 0x11);
	readPinnacleReg(R_TRACKPAD, 0x12);
	writePinnacleReg(R_TRACKPAD, TPAD_REG_STATUS_1, 0x00);

	writePinnacleReg(R_TRACKPAD, TPAD_REG_STATUS_1, 0x00);

// TODO: resume at line 121865
	// This is where we start setting up PINT3/4
}

void SSP1_IRQHandler (void) {
	//TODO: Setup to react to SPI transaction complete?
}

// 3 - GPIO pin interrupt 3
// Right Haptic DR
void FLEX_INT3_IRQHandler(void) {
	//TODO: Setup to react to data ready pin rising edge?
}

// 4 - GPIO pin interrupt 4
// Left Haptic DR
void FLEX_INT4_IRQHandler(void) {
	//TODO: Setup to react to data ready pin rising edge?
}

/**
 * Print command usage details to console.
 *
 * \return None.
 */
void trackpadCmdUsage(void) {
	printf(
		"usage: trackpad monitor\n"
		"       trackpad read regAddr\n"
		"       trackpad write regAddr val\n"
		"\n"
//TODO
	);
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
		printf("DR Low for Right Trackpad\n");
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
