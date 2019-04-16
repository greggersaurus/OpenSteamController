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
#include "usb.h"
#include "eeprom_access.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANYMEAS_EN (1) //!< This flag controls whether the trackpad ASICs are
	//!< configured to perform movement tracking calcualtions or whether
	//!< all access to Trackpad ASICs is in AnyMeas mode (i.e. a raw
	//!< data access mode). 
	//!< Note: AnyMeas Mode is the way the official firmware uses the 
	//!<  and either Normal Mode does not work well or I am missing some
	//!<  setup or configuration steps... Going to focus on AnyMeas Mode
	//!<  since we at least have official FW as reference. Leaving
	//!<  Normal Mode option in here just in case anyone wants to try
	//!<  and work with it in the future.
	//!< Note: AnyMeas Mode seems to be poorly documented. Code below
	//!<  was obtained by replicating official firmware behavior and
	//!<  using https://github.com/cirque-corp/Cirque_Pinnacle_1CA027/blob/master/Additional_Examples/AnyMeas_Example/Pinnacle.h
	//!<  as a reference. 

static LPC_SSP_T* const spiRegs = LPC_SSP0;

#define GPIO_SSP0_SCK0 1, 29 //!< SPI Clock Pin
#define GPIO_SSP0_MISO0 0, 8 //!< SPI Master In Slave Out Pin
#define GPIO_SSP0_MOSI0 0, 9 //!< SPI Master Out Slave In Pin

#define GPIO_R_TRACKPAD_CS_N 1, 15 //!< Chip select pin for communicating with
		//!< Right Trackpad.
#define GPIO_R_TRACKPAD_DR 0, 23 //!< Data Ready pin for Right Trackpad.
		//!< Indicates Trackpad Data Registers have data to be read.
#define GPIO_L_TRACKPAD_CS_N 1, 6 //!< Chip select pin for communicating with
		//!< Left Trackpad.
#define GPIO_L_TRACKPAD_DR 1, 16 //!< Data Ready pin for Right Trackpad.
		//!< Indicates Trackpad Data Registers have data to be read.

#define PINT_R_TRACKPAD 3 //!< GPIO Pin Interrupt configured for Right Trackpad.
#define PINT_L_TRACKPAD 4 //!< GPIO Pin Interrupt configured for Left Trackpad.


#if (!ANYMEAS_EN)

typedef struct TrackpadAbsData {
	uint16_t xPos;
	uint16_t yPos;
	uint16_t zPos;
} TrackpadAbsData;

volatile TrackpadAbsData tpadAbsDatas[2][2]; //!< Two copies of absolute data
	//!< for each trackpad. This is filled in by ISR and can be read by
	//!< function(s) requesting most up to date X/Y position.
volatile int tpadAbsDataIdxs[2]; //!< Defines which copy in tpadAbsData will be 
	//!< written to on next ISR, and thus defins which copy is most recent.
volatile bool tpadIsrBusys[2]; //!< Defines if the interrupt is currently being 
	//!< being handled.


#else  // if (ANYMEAS_EN)


#define NUM_ANYMEAS_X_ADCS (11) //!< The number of ADC reading used for 
	//!< calculating the X axis position.
#define NUM_ANYMEAS_Y_ADCS (7) //!< The number of ADC reading used for 
	//!< calculating the Y axis position. 
#define NUM_ANYMEAS_ADCS (NUM_ANYMEAS_X_ADCS + NUM_ANYMEAS_Y_ADCS) //!< The
	//!< total number of AnyMeas ADCs read for computing X/Y position.

#define ANYMEAS_X_ADC_ADDR (0x01df) //!< Start address for AnyMeas ADCs
	//!< relating to X position
#define ANYMEAS_Y_ADC_ADDR (0x015b) //!< Start address for AnyMeas ADCs
	//!< relating to Y position

static int16_t tpadAdcComps[2][NUM_ANYMEAS_ADCS]; //!< Compensation values 
	//!< for AnyMeas ADC channels used to calculate X/Y position.
static volatile int16_t tpadAdcDatas[2][NUM_ANYMEAS_ADCS]; //!< The ADC 
	//!< values relating to X/Y position filled in by ISR. Read tpadAdcIdxs
	//!< to tell if these are currently being updated.
static volatile int tpadAdcIdxs[2]; //!< Tracks how tpadAdcDatas is being
	//!< updated. If this is NUM_ANYMEAS_ADCS, it means tpadAdcDatas
	//!< are all safe to be read.


#endif // ANYMEAS_EN


// Trackpad ASIC Registers. See https://github.com/cirque-corp/Cirque_Pinnacle_1CA027/blob/master/Additional_Examples/AnyMeas_Example
//  for reference regarding usage (especially AnyMeas specific registers):
#define TPAD_FW_ID_ADDR (0x00)
#define TPAD_FW_VER_ADDR (0x01)
#define TPAD_STATUS1_ADDR (0x02)
	#define TPAD_STATUS1_SW_DR_BIT (0x04) // Software Data Ready
	#define TPAD_STATUS1_CC_BIT (0x08) // Command Complete
#define TPAD_SYSCFG1_ADDR (0x03)
	#define TPAD_SYSCFG1_RESET_BIT (0x01)
	#define TPAD_SYSCFG1_SHUTDOWN_BIT (0x02)
	#define TPAD_SYSCFG1_SLEEP_BIT (0x04)
	#define TPAD_SYSCFG1_TRACKDIS_BIT (0x08) // Disable internal tracking
		// calculations
	#define TPAD_SYSCFG1_ANYMEASEN_BIT (0x10) // Enables low level Any 
		// Measure mode (i.e. raw mode). Changes registers 0x05 and on.
	#define TPAD_SYSCFG1_GPIOCTRLEN_BIT (0x20)

#if (!ANYMEAS_EN)

#define TPAD_FEEDCFG1_ADDR (0x04)
	#define TPAD_FEEDCFG1_FEEDEN_BIT (0x01) // Enable data flow
	#define TPAD_FEEDCFG1_ABSEN_BIT (0x02) // Absolute mode enable (relative
		// mode disable)
	#define TPAD_FEEDCFG1_FILTDIS_BIT (0x04) // Filter disable
	#define TPAD_FEEDCFG1_XDIS_BIT (0x08) // X disable
	#define TPAD_FEEDCFG1_YDIS_BIT (0x10) // Y disable
	#define TPAD_FEEDCFG1_XINVERT_BIT (0x40) // X data invert
	#define TPAD_FEEDCFG1_YINVERT_BIT (0x80) // Y data disable
#define TPAD_FEEDCFG2_ADDR (0x05)
	#define TPAD_FEEDCFG2_INTMOUSE_BIT (0x01) // Intellimouse Enable
	#define TPAD_FEEDCFG2_SWAPXY_BIT (0x80) // Swap X & Y (i.e. 90 degree
		// rotation)
#define TPAD_FEEDCFG3_ADDR (0x06)
#define TPAD_CALCFG_ADDR (0x07)
	#define TPAD_CALCFG_CALIBRATE (0x01) //
#define TPAD_PS2AUXCTRL_ADDR (0x08)
#define TPAD_SAMPLERATE_ADDR (0x09)
	#define TPAD_SAMPLERATE_100 (0x64) // 100 Samples/Second
#define TPAD_ZIDLE_ADDR (0x0A)
#define TPAD_ZSCALER_ADDR (0x0B)
#define TPAD_SLEEPINTERVAL_ADDR (0x0C)
#define TPAD_SLEEPTIMER_ADDR (0x0D)
#define TPAD_DYNEMI_ADDR (0x0E)
#define TPAD_PACKETBTE0_ADDR (0x12)
#define TPAD_PACKETBTE1_ADDR (0x13)
#define TPAD_PACKETBTE2_ADDR (0x14)
#define TPAD_PACKETBTE3_ADDR (0x15)
#define TPAD_PACKETBTE4_ADDR (0x16)
#define TPAD_PACKETBTE5_ADDR (0x17)

#else  // if (ANYMEAS_EN)

#define TPAD_ADCCFG1_ADDR (0x05) 
	typedef enum TpadAdcGain_t {
		TPAD_ADC_GAIN0 = 0xC0, // Lowest gain
		TPAD_ADC_GAIN1 = 0x80, // ~1.3x gain
		TPAD_ADC_GAIN2 = 0x40, // ~1.6x gain
		TPAD_ADC_GAIN3 = 0x00 // ~2x gain
	} TpadAdcGain; 
	typedef enum TpadAdcToggleFreq_t {
		TPAD_ADC_TOGGLE_FREQ_0 = 0x02, // 500,000 Hz (based on default aperture of 500ns)
		TPAD_ADC_TOGGLE_FREQ_1 = 0x03, // 444,444 Hz (based on default aperture of 500ns)
		TPAD_ADC_TOGGLE_FREQ_2 = 0x04, // 400,000 Hz (based on default aperture of 500ns)
		TPAD_ADC_TOGGLE_FREQ_3 = 0x05, // 363,636 Hz (based on default aperture of 500ns)
		TPAD_ADC_TOGGLE_FREQ_4 = 0x06, // 333,333 Hz (based on default aperture of 500ns)
		TPAD_ADC_TOGGLE_FREQ_5 = 0x07, // 307,692 Hz (based on default aperture of 500ns)
		TPAD_ADC_TOGGLE_FREQ_6 = 0x09, // 267,000 Hz (based on default aperture of 500ns)
		TPAD_ADC_TOGGLE_FREQ_7 = 0x0B, // 235,000 Hz (based on default aperture of 500ns)
	} TpadAdcToggleFreq; 
#define TPAD_ADCCTRL_ADDR (0x06)
	#define TPAD_ADCCTRL_STARTBUSY_BIT (0x80) 
	#define TPAD_ADCCTRL_INTFLAG_BIT (0x40) 
	#define TPAD_ADCCTRL_EN_BIT (0x20) 
	typedef enum TpadAdcSampleLen_t {
		TPAD_ADC_SAMPLEN_128 = 0x01,
		TPAD_ADC_SAMPLEN_256 = 0x02,
		TPAD_ADC_SAMPLEN_512 = 0x03
	} TpadAdcSampleLen;
#define TPAD_ADCMUXCTRL_ADDR (0x07)
	typedef enum TpadAdcMuxSel_t {
		TPAD_ADC_MUXSEL_SENSEP1GATE = 0x01 //!< Enables Sense P1.
	} TpadAdcMuxSel;
#define TPAD_ADCCFG2_ADDR (0x08) 
	// Contains various configuration settings not likely to be used...
#define TPAD_ADCWIDTH_ADDR (0x09) 
	typedef enum TpadAdcAperture_t {
		TPAD_ADC_APETURE_250NS = 0x02,
		TPAD_ADC_APETURE_375NS = 0x03,
		TPAD_ADC_APETURE_500NS = 0x04,
		TPAD_ADC_APETURE_625NS = 0x05,
		TPAD_ADC_APETURE_750NS = 0x06,
		TPAD_ADC_APETURE_875NS = 0x07,
		TPAD_ADC_APETURE_1000NS = 0x08,
		TPAD_ADC_APETURE_1125NS = 0x09,
		TPAD_ADC_APETURE_1250NS = 0x0A,
		TPAD_ADC_APETURE_1375NS = 0x0B,
		TPAD_ADC_APETURE_1500NS = 0x0C,
		TPAD_ADC_APETURE_1625NS = 0x0D,
		TPAD_ADC_APETURE_1750NS = 0x0E,
		TPAD_ADC_APETURE_1875NS = 0x0F
	} TpadAdcAperture;
#define TPAD_ADC_START_ADDR_HI_ADDR (0x0A) 
#define TPAD_ADC_START_ADDR_LO_ADDR (0x0B) 
#define TPAD_MEASINDEX_ADDR (0x0C) 
	#define TPAD_MEASINDEX_RSTSTART_BITS (0x00) 
	#define TPAD_MEASINDEX_STARTMEAS_BITS (0x01) 
	#define TPAD_MEASINDEX_WAIT_BITS (0x02) 
#define TPAD_ANYMEASSTATE_ADDR (0x0D)
#define TPAD_MEASCTRL_ADDR (0x0E) 
	#define TPAD_MEASCTRL_REPEAT_BIT (0x80) 
	#define TPAD_MEASCTRL_POSTMEASPWR_BIT (0x40) //!< Turn ADC off after
		//!< measurements. Longer startup between measuremnts, but
		//!< lower power usage.
	#define TPAD_MEASCTRL_NUMMEAS_MASK (0x3F)
#define TPAD_MEASRESULT_HI_ADDR (0x11) 
#define TPAD_MEASRESULT_LO_ADDR (0x12) 
// Toggle is part of taking ADC measurement... but don't fully understand it
#define TPAD_TOGGLE_HIHI_ADDR (0x13) //!< Toggle(31:24)
#define TPAD_TOGGLE_HILO_ADDR (0x14) //!< Toggle(23:16)
#define TPAD_TOGGLE_LOHI_ADDR (0x15) //!< Toggle(15:8)
#define TPAD_TOGGLE_LOLO_ADDR (0x16) //!< Toggle(7:0)
// Polarity is part of taking ADC measurement... but don't fully understand it
#define TPAD_POLARITY_HIHI_ADDR (0x17) //!< Polarity(31:24)
#define TPAD_POLARITY_HILO_ADDR (0x18) //!< Polarity(23:16)
#define TPAD_POLARITY_LOHI_ADDR (0x19) //!< Polarity(15:8)
#define TPAD_POLARITY_LOLO_ADDR (0x1A) //!< Polarity(7:0)

#endif // ANYMEAS_EN

// These registers do not change based on AnyMeas settings:
#define TPAD_ERA_VAL_ADDR 0x1B
#define TPAD_ERA_HIADDR_ADDR 0x1C
#define TPAD_ERA_LOADDR_ADDR 0x1D
#define TPAD_ERA_CTRL_ADDR 0x1E

#define TPAD_PRODID_ADDR 0x1F

/**
 * Write to a register on the Pinnacle ASIC (i.e. the Trackpad controller).
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param addr Register address to write to.
 * \param val Value to write to register.
 *
 * \return None.
 */
static void writeTpadReg(Trackpad trackpad, uint8_t addr, uint8_t val) {
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
static uint8_t readTpadReg(Trackpad trackpad, uint8_t addr) {
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
static void writeTpadExtRegs(Trackpad trackpad, uint16_t addr, uint8_t len, 
	const uint8_t* data) {

	// Write address 
	writeTpadReg(trackpad, TPAD_ERA_HIADDR_ADDR, 0xFF & (addr >> 8));
	writeTpadReg(trackpad, TPAD_ERA_LOADDR_ADDR, 0xFF & addr);

	for (int idx = 0; idx < len; idx++) {
		// Write value
		writeTpadReg(trackpad, TPAD_ERA_VAL_ADDR, data[idx]);

		// Write ERA auto-increment write to ERA Control
		writeTpadReg(trackpad, TPAD_ERA_CTRL_ADDR, 0x0A);

		// Read ERA Control until it contains 0x00
		while (readTpadReg(trackpad, TPAD_ERA_CTRL_ADDR)){
		}
	}
}

/**
 * Clear Software Command Complete and Software Data Ready flags for the
 *  specifica trackpad.
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * 
 * \return None.
 */
static inline void clearTpadFlags(Trackpad trackpad) {
	writeTpadReg(trackpad, TPAD_STATUS1_ADDR, 0x00);
}

/**
 * Setup interrupt handling for rising edge of DR event from specified trackpad.
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * 
 * \return None.
 */
static void setupTpadISR(Trackpad trackpad) {
	// Setting PINT so we can react to PINT rising edge
	if (trackpad == R_TRACKPAD) {
		Chip_SYSCTL_SetPinInterrupt(PINT_R_TRACKPAD, GPIO_R_TRACKPAD_DR);
		Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(PINT_R_TRACKPAD));
		Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH(PINT_R_TRACKPAD));
		NVIC_ClearPendingIRQ(PIN_INT3_IRQn);
		NVIC_EnableIRQ(PIN_INT3_IRQn);
		NVIC_SetPriority(PIN_INT3_IRQn, 3);
	} else if (trackpad == L_TRACKPAD) {
		Chip_SYSCTL_SetPinInterrupt(PINT_L_TRACKPAD, GPIO_L_TRACKPAD_DR);
		Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(PINT_L_TRACKPAD));
		Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH(PINT_L_TRACKPAD));
		NVIC_ClearPendingIRQ(PIN_INT4_IRQn);
		NVIC_EnableIRQ(PIN_INT4_IRQn);
		NVIC_SetPriority(PIN_INT4_IRQn, 3);
	}
}


#if (!ANYMEAS_EN)


/**
 * Setup Trackpad ASIC (i.e. configure registers, calibration, setup ISR).
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * 
 * \return None.
 */
static void setupTpad(Trackpad trackpad) {
	// Reset the TrackpadASIC:
	writeTpadReg(trackpad, TPAD_SYSCFG1_ADDR, TPAD_SYSCFG1_RESET_BIT);

	usleep(50 * 1000);

	while (!(TPAD_STATUS1_CC_BIT & readTpadReg(trackpad, 
		TPAD_STATUS1_ADDR))) {
	}

	clearTpadFlags(trackpad);

	usleep(10 * 1000);

	// Check Firmware ID
	uint8_t fw_id = readTpadReg(trackpad, TPAD_FW_ID_ADDR);
	if (fw_id != 0x07)
		return;

	// Check Firmware Version
	uint8_t fw_ver = readTpadReg(trackpad, TPAD_FW_VER_ADDR);
	if (fw_ver != 0x3a) 
		return;

	// Set ASIC to normal mode and active
	writeTpadReg(trackpad, TPAD_SYSCFG1_ADDR, 0);
	
	// Allow time for changes to take place
	usleep(10 * 1000);

	clearTpadFlags(trackpad);

	writeTpadReg(trackpad, TPAD_FEEDCFG2_ADDR, 0x1F);
	writeTpadReg(trackpad, TPAD_FEEDCFG1_ADDR, TPAD_FEEDCFG1_FEEDEN_BIT
		| TPAD_FEEDCFG1_ABSEN_BIT);
	writeTpadReg(trackpad, TPAD_ZIDLE_ADDR, 0x5);
	writeTpadReg(trackpad, TPAD_ZSCALER_ADDR, 16);

	setupTpadISR(trackpad);
}

/**
 * Get the latest Absolute Packet DAta and Clear Flags, all in a single burst
 *  of SPI data.
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param[out] absData Location of where to store absolute packet data.
 * 
 * \return None.
 */
void getAbsDataAndClr(Trackpad trackpad, volatile TrackpadAbsData* absData) {
	Chip_SSP_DATA_SETUP_T xf_setup;
	uint8_t tx_data[11];
	uint8_t rx_data[11];

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, false);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, false);
	}

	// Number of words to transfer
	xf_setup.length = 11;
	// Used to count how many words have been transmitted
	xf_setup.tx_cnt = 0;
	xf_setup.tx_data = tx_data;
	// Used to count how many words have been received
	xf_setup.rx_cnt = 0;
	xf_setup.rx_data = rx_data;

	// Auto-incremented read starting at register TPAD_MEASRESULT_HI_ADDR
	tx_data[0] = 0xA0 | TPAD_PACKETBTE0_ADDR; // Command Byte
	tx_data[1] = 0xFC; // Filler Byte
	tx_data[2] = 0xFC; // Filler Byte
	tx_data[3] = 0xFC; // PacketByte_0
	tx_data[4] = 0xFC; // PacketByte_1
	tx_data[5] = 0xFC; // PacketByte_2
	tx_data[6] = 0xFC; // PacketByte_3
	tx_data[7] = 0xFC; // PacketByte_4
	tx_data[8] = 0xFC; // PacketByte_5
	// Clear flags
	tx_data[9] = 0x80 | TPAD_STATUS1_ADDR;
	tx_data[10] = 0x00;

	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, true);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, true);
	}

	absData->xPos = ((0x0F & rx_data[7]) << 8) | rx_data[5];
	absData->yPos = ((0xF0 & rx_data[7]) << 4) | rx_data[6];
	absData->zPos = 0x3F & rx_data[8];
}

/**
 * Get the latest data from the Pinnacle ASIC and update global variables, etc.
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 *
 * \return The ADC value.
 */
static void getLatestTpadData(Trackpad trackpad) {
	getAbsDataAndClr(trackpad, tpadAbsDataIdxs[trackpad]);

	tpadAbsDataIdxs[trackpad]++;
	tpadAbsDataIdxs[trackpad] %= 2;
}


#else  // if (ANYMEAS_EN)


/**
 * Get the latest AnyMeas ADC reading and Clear Flags, all in a single burst
 *  of SPI data.
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 *
 * \return The ADC value.
 */
static int16_t getTpadAdcAndClr(Trackpad trackpad) {
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

	// Auto-incremented read starting at register TPAD_MEASRESULT_HI_ADDR
	tx_data[0] = 0xA0 | TPAD_MEASRESULT_HI_ADDR;
	tx_data[1] = 0xFC; // Filler Byte
	tx_data[2] = 0xFC; // Filler Byte
	tx_data[3] = 0xFC; // TPAD_MEASRESULT_HI_ADDR
	tx_data[4] = 0xFB; // TPAD_MEASRESULT_LO_ADDR
	// Clear flags
	tx_data[5] = 0x80 | TPAD_STATUS1_ADDR;
	tx_data[6] = 0x00;

	Chip_SSP_RWFrames_Blocking(spiRegs, &xf_setup);

	if (R_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_R_TRACKPAD_CS_N, true);
	} else if (L_TRACKPAD == trackpad) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, true);
	}

	// Concatenate TPAD_MEASRESULT_HI_ADDR and TPAD_MEASRESULT_HI_ADDR into 
	//  a single 16-bit word
	return (rx_data[3] << 8) | rx_data[4];
}

/**
 * Function to encompass all (relevant) settings related to configuring ADC
 *  in AnyMeas mode.
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param gain Defines gain for ADC... (i.e. not really sure...).
 * \param toggleFreq AKA ElecFrec... (i.e. not really sure... Related to 
 *	toggle registers?).
 * \param sampleLength Bit length... (i.e. not really sure...).
 * \param muxSel Sets the sense mux... (i.e. not really sure...).
 * \param cfg2 Misc config settings not likely to be used (i.e. set to 0).
 * \param aperture Apterture width... (i.e. not really sure...).
 * 
 * \return None.
 */
static void setTpadAdcCfg(Trackpad trackpad, TpadAdcGain gain, 
	TpadAdcToggleFreq toggleFreq, TpadAdcSampleLen sampleLength, 
	TpadAdcMuxSel muxSel , uint8_t cfg2, 
	TpadAdcAperture aperture) {
	// Shadow registers to reduce unnecessary SPI transactions
	static uint8_t cfg1_shadow[2] = {0, 0};
	static uint8_t ctrl_shadow[2] = {0, 0};
	static uint8_t mux_ctrl_shadow[2] = {0, 0};
	static uint8_t cfg2_shadow[2] = {0, 0};
	static uint8_t width_shadow[2] = {0, 0};

	uint8_t cfg1_update = gain | toggleFreq;
	uint8_t ctrl_update = sampleLength;
	uint8_t mux_ctrl_update = muxSel;
	uint8_t cfg2_update = cfg2;
	uint8_t width_update = aperture;
	
	if (cfg1_shadow[trackpad] != cfg1_update) {
		writeTpadReg(trackpad, TPAD_ADCCFG1_ADDR, cfg1_update);
		cfg1_shadow[trackpad] = cfg1_update;
	}
	if (ctrl_shadow[trackpad] != ctrl_update) {
		writeTpadReg(trackpad, TPAD_ADCCTRL_ADDR, ctrl_update);
		ctrl_shadow[trackpad] = ctrl_update;
	}
	if (mux_ctrl_shadow[trackpad] != mux_ctrl_update) {
		writeTpadReg(trackpad, TPAD_ADCMUXCTRL_ADDR, mux_ctrl_update);
		mux_ctrl_shadow[trackpad] = mux_ctrl_update;
	}
	if (cfg2_shadow[trackpad] != cfg2_update) {
		writeTpadReg(trackpad, TPAD_ADCCFG2_ADDR, cfg2_update);
		cfg2_shadow[trackpad] = cfg2_update;
	}
	if (width_shadow[trackpad] != width_update) {
		writeTpadReg(trackpad, TPAD_ADCWIDTH_ADDR, width_update);
		width_shadow[trackpad] = width_update;
	}
}

/**
 * Update registers used to set Toggle value. 
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param toggle Not entirely sure on its purpose. Somehow related to ADC
 *	readings in AnyMeas mode...
 *
 * \return None.
 */
static void setTpadToggle(Trackpad trackpad, uint32_t toggle) {
	// Shadow copies of Trackpad ASIC registers to minimize SPI transactions
	static uint8_t toggle_hihi_shadow[2] = {0, 0};
	static uint8_t toggle_hilo_shadow[2] = {0, 0};
	static uint8_t toggle_lohi_shadow[2] = {0, 0};
	static uint8_t toggle_lolo_shadow[2] = {0, 0};

	uint8_t toggle_hihi = 0xFF & (toggle >> 24);
	uint8_t toggle_hilo = 0xFF & (toggle >> 16);
	uint8_t toggle_lohi = 0xFF & (toggle >> 8);
	uint8_t toggle_lolo = 0xFF & (toggle >> 0);

	if (toggle_hihi_shadow[trackpad] != toggle_hihi) {
		writeTpadReg(trackpad, TPAD_TOGGLE_HIHI_ADDR, toggle_hihi);
		toggle_hihi_shadow[trackpad] = toggle_hihi;
	}	
	if (toggle_hilo_shadow[trackpad] != toggle_hilo) {
		writeTpadReg(trackpad, TPAD_TOGGLE_HILO_ADDR, toggle_hilo);
		toggle_hilo_shadow[trackpad] = toggle_hilo;
	}	
	if (toggle_lohi_shadow[trackpad] != toggle_lohi) {
		writeTpadReg(trackpad, TPAD_TOGGLE_LOHI_ADDR, toggle_lohi);
		toggle_lohi_shadow[trackpad] = toggle_lohi;
	}	
	if (toggle_lolo_shadow[trackpad] != toggle_lolo) {
		writeTpadReg(trackpad, TPAD_TOGGLE_LOLO_ADDR, toggle_lolo);
		toggle_lolo_shadow[trackpad] = toggle_lolo;
	}	
}

/**
 * Update registers used to set Poliarty value. 
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param polarity Not entirely sure on its purpose. Somehow related to ADC
 *	readings in AnyMeas mode...
 *
 * \return None.
 */
static void setTpadPolarity(Trackpad trackpad, uint32_t polarity) {
	// Shadow copies of Trackpad ASIC registers to minimize SPI transactions
	static uint8_t polarity_hihi_shadow[2] = {0, 0};
	static uint8_t polarity_hilo_shadow[2] = {0, 0};
	static uint8_t polarity_lohi_shadow[2] = {0, 0};
	static uint8_t polarity_lolo_shadow[2] = {0, 0};

	uint8_t polarity_hihi = 0xFF & (polarity >> 24);
	uint8_t polarity_hilo = 0xFF & (polarity >> 16);
	uint8_t polarity_lohi = 0xFF & (polarity >> 8);
	uint8_t polarity_lolo = 0xFF & (polarity >> 0);

	if (polarity_hihi_shadow[trackpad] != polarity_hihi) {
		writeTpadReg(trackpad, TPAD_POLARITY_HIHI_ADDR, polarity_hihi);
		polarity_hihi_shadow[trackpad] = polarity_hihi;
	}	
	if (polarity_hilo_shadow[trackpad] != polarity_hilo) {
		writeTpadReg(trackpad, TPAD_POLARITY_HILO_ADDR, polarity_hilo);
		polarity_hilo_shadow[trackpad] = polarity_hilo;
	}	
	if (polarity_lohi_shadow[trackpad] != polarity_lohi) {
		writeTpadReg(trackpad, TPAD_POLARITY_LOHI_ADDR, polarity_lohi);
		polarity_lohi_shadow[trackpad] = polarity_lohi;
	}	
	if (polarity_lolo_shadow[trackpad] != polarity_lolo) {
		writeTpadReg(trackpad, TPAD_POLARITY_LOLO_ADDR, polarity_lolo);
		polarity_lolo_shadow[trackpad] = polarity_lolo;
	}	
}

/**
 * Update registers used to set address where next ADC read starts. 
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param addr Defines where next ADC read starts.
 *
 * \return None.
 */
static void setTpadAdcStartAddr(Trackpad trackpad, uint16_t addr) {
	// Shadow copies to minimize SPI transactions
	uint8_t addr_hi_shadow[2] = {0, 0};
	uint8_t addr_lo_shadow[2] = {0, 0};

	uint8_t addr_hi = 0xFF & (addr >> 8);
	uint8_t addr_lo = 0xFF & addr;

	if (addr_hi_shadow[trackpad] != addr_hi) {
		writeTpadReg(trackpad, TPAD_ADC_START_ADDR_HI_ADDR, addr_hi);
		addr_hi_shadow[trackpad] = addr_hi;
	}
	if (addr_lo_shadow[trackpad] != addr_lo) {
		writeTpadReg(trackpad, TPAD_ADC_START_ADDR_LO_ADDR, addr_lo);
		addr_lo_shadow[trackpad] = addr_lo;
	}
}

/**
 * Specify how many measurements to take (before needing to be started again?)?
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param numMeas The number of measurements to take before...?
 *
 * \return None.
 */
static void setTpadNumMeas(Trackpad trackpad, uint8_t numMeas) {
	static uint8_t meas_ctrl_shadow[2] = {0, 0};

	// TODO: add flag for enabling low power mode via TPAD_MEASCTRL_POSTMEASPWR_BIT?
	//  For now always run fast as possible and do not worry about power
	uint8_t meas_ctrl = (TPAD_MEASCTRL_NUMMEAS_MASK & numMeas);

	if (meas_ctrl_shadow[trackpad] != meas_ctrl) {
		writeTpadReg(trackpad, TPAD_MEASCTRL_ADDR, meas_ctrl);
		meas_ctrl_shadow[trackpad] = meas_ctrl;
	}
}

/**
 * Take an ADC Measurement with Trackpad ASIC in AnyMeas mode.
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * \param toggle Not entirely sure on its purpose. Somehow related to ADC
 *	readings in AnyMeas mode...
 * \param polarity Not entirely sure on its purpose. Somehow related to ADC
 *	readings in AnyMeas mode...
 * \param adjust Right shift adjustment applied to ADC result.
 * 
 * \return ADC result (16-bit value cast to 32-bit...?)
 */
static int32_t takeTpadAdcMeas(Trackpad trackpad, uint32_t toggle, 
	uint32_t polarity, uint8_t adjust) {

	setTpadToggle(trackpad, toggle);
	setTpadPolarity(trackpad, polarity);
	// Put measurement data starting at Register 19???
	setTpadAdcStartAddr(trackpad, 0x0013);
	setTpadNumMeas(trackpad, 1);

	// Start the measurement
	writeTpadReg(trackpad, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

	usleep(2 * 1000);

	int16_t retval = getTpadAdcAndClr(trackpad);

	return (int32_t)(retval >> adjust);
}

/**
 * Request AnyMeas ADC results start being captured so that X/Y locations can
 *  be calculated. This function starts conversion process (which continues
 *  via ISR) and then returns. See trackpadGetLastXY() for waitinging for
 *  ADC results to be gathered and then converted to X/Y position. 
 *  
 * \param trackpad Specifies which trackpad to communicate with. 
 *
 * \return None.
 */
void trackpadLocUpdate(Trackpad trackpad) {
	tpadAdcIdxs[trackpad] = 0;

	// Start by requesting measurements for X axis location
	setTpadAdcStartAddr(trackpad, ANYMEAS_X_ADC_ADDR);
	setTpadNumMeas(trackpad, NUM_ANYMEAS_X_ADCS);

	// Start the measurements
	writeTpadReg(trackpad, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);
}

/**
 * Convert the last updated AnyMeas ADC values to X/Y location. If update to
 *  AnyMeas ADC values has been requested (i.e. via trackpadLocUpdate()), this
 *  function will wait until data has been updated.
 * 
 * \param trackpad Specifies which Trackpad to communicate with. 
 * \param[out] xLoc X location. 0-1200. 0 is left side of Trackpad. 1200/2 will
 *	be returned if finger is not down.
 * \param[out] yLoc y location. 0-700. 0 is bottom side of Trackpad. 700/2 will
 *	be returned if finger is not down.
 *
 * \return None.
 */
void trackpadGetLastXY(Trackpad trackpad, uint16_t* xLoc, uint16_t* yLoc) {

	// Set defaults in case finger is not down
	*xLoc = 1200/2;
	*yLoc = 700/2;

	// Wait for AnyMeas ADCs related to X position to be updated
	while (tpadAdcIdxs[trackpad] < NUM_ANYMEAS_X_ADCS) {
	}

	// Calculate xLoc
	int32_t adc_vals_x[12];

	// This is based on simulation of official firmware. Cannot say I
	//  understand it...
	int32_t compensated_val = tpadAdcDatas[trackpad][0] - tpadAdcComps[trackpad][0];
	adc_vals_x[0] = compensated_val;
	adc_vals_x[1] = compensated_val;
	adc_vals_x[2] = -compensated_val;
	adc_vals_x[3] = compensated_val;
	adc_vals_x[4] = compensated_val;
	adc_vals_x[5] = compensated_val;
	adc_vals_x[6] = -compensated_val;
	adc_vals_x[7] = -compensated_val;
	adc_vals_x[8] = -compensated_val;
	adc_vals_x[9] = compensated_val;
	adc_vals_x[10] = -compensated_val;
	adc_vals_x[11] = -compensated_val;

	compensated_val = tpadAdcDatas[trackpad][1] - tpadAdcComps[trackpad][1];
	adc_vals_x[0] -= compensated_val;
	adc_vals_x[1] += compensated_val;
	adc_vals_x[2] += compensated_val;
	adc_vals_x[3] -= compensated_val;
	adc_vals_x[4] += compensated_val;
	adc_vals_x[5] += compensated_val;
	adc_vals_x[6] += compensated_val;
	adc_vals_x[7] -= compensated_val;
	adc_vals_x[8] -= compensated_val;
	adc_vals_x[9] -= compensated_val;
	adc_vals_x[10] += compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][2] - tpadAdcComps[trackpad][2];
	adc_vals_x[0] += compensated_val;
	adc_vals_x[1] -= compensated_val;
	adc_vals_x[2] += compensated_val;
	adc_vals_x[3] += compensated_val;
	adc_vals_x[4] -= compensated_val;
	adc_vals_x[5] += compensated_val;
	adc_vals_x[6] += compensated_val;
	adc_vals_x[7] += compensated_val;
	adc_vals_x[8] -= compensated_val;
	adc_vals_x[9] -= compensated_val;
	adc_vals_x[10] -= compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][3] - tpadAdcComps[trackpad][3];
	adc_vals_x[0] -= compensated_val;
	adc_vals_x[1] += compensated_val;
	adc_vals_x[2] -= compensated_val;
	adc_vals_x[3] += compensated_val;
	adc_vals_x[4] += compensated_val;
	adc_vals_x[5] -= compensated_val;
	adc_vals_x[6] += compensated_val;
	adc_vals_x[7] += compensated_val;
	adc_vals_x[8] += compensated_val;
	adc_vals_x[9] -= compensated_val;
	adc_vals_x[10] -= compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][4] - tpadAdcComps[trackpad][4];
	adc_vals_x[0] -= compensated_val;
	adc_vals_x[1] -= compensated_val;
	adc_vals_x[2] += compensated_val;
	adc_vals_x[3] -= compensated_val;
	adc_vals_x[4] += compensated_val;
	adc_vals_x[5] += compensated_val;
	adc_vals_x[6] -= compensated_val;
	adc_vals_x[7] += compensated_val;
	adc_vals_x[8] += compensated_val;
	adc_vals_x[9] += compensated_val;
	adc_vals_x[10] -= compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][5] - tpadAdcComps[trackpad][5];
	adc_vals_x[0] -= compensated_val;
	adc_vals_x[1] -= compensated_val;
	adc_vals_x[2] -= compensated_val;
	adc_vals_x[3] += compensated_val;
	adc_vals_x[4] -= compensated_val;
	adc_vals_x[5] += compensated_val;
	adc_vals_x[6] += compensated_val;
	adc_vals_x[7] -= compensated_val;
	adc_vals_x[8] += compensated_val;
	adc_vals_x[9] += compensated_val;
	adc_vals_x[10] += compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][6] - tpadAdcComps[trackpad][6];
	adc_vals_x[0] += compensated_val;
	adc_vals_x[1] -= compensated_val;
	adc_vals_x[2] -= compensated_val;
	adc_vals_x[3] -= compensated_val;
	adc_vals_x[4] += compensated_val;
	adc_vals_x[5] -= compensated_val;
	adc_vals_x[6] += compensated_val;
	adc_vals_x[7] += compensated_val;
	adc_vals_x[8] -= compensated_val;
	adc_vals_x[9] += compensated_val;
	adc_vals_x[10] += compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][7] - tpadAdcComps[trackpad][7];
	adc_vals_x[0] += compensated_val;
	adc_vals_x[1] += compensated_val;
	adc_vals_x[2] -= compensated_val;
	adc_vals_x[3] -= compensated_val;
	adc_vals_x[4] -= compensated_val;
	adc_vals_x[5] += compensated_val;
	adc_vals_x[6] -= compensated_val;
	adc_vals_x[7] += compensated_val;
	adc_vals_x[8] += compensated_val;
	adc_vals_x[9] -= compensated_val;
	adc_vals_x[10] += compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][8] - tpadAdcComps[trackpad][8];
	adc_vals_x[0] += compensated_val;
	adc_vals_x[1] += compensated_val;
	adc_vals_x[2] += compensated_val;
	adc_vals_x[3] -= compensated_val;
	adc_vals_x[4] -= compensated_val;
	adc_vals_x[5] -= compensated_val;
	adc_vals_x[6] += compensated_val;
	adc_vals_x[7] -= compensated_val;
	adc_vals_x[8] += compensated_val;
	adc_vals_x[9] += compensated_val;
	adc_vals_x[10] -= compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][9] - tpadAdcComps[trackpad][9];
	adc_vals_x[0] -= compensated_val;
	adc_vals_x[1] += compensated_val;
	adc_vals_x[2] += compensated_val;
	adc_vals_x[3] += compensated_val;
	adc_vals_x[4] -= compensated_val;
	adc_vals_x[5] -= compensated_val;
	adc_vals_x[6] -= compensated_val;
	adc_vals_x[7] += compensated_val;
	adc_vals_x[8] -= compensated_val;
	adc_vals_x[9] += compensated_val;
	adc_vals_x[10] += compensated_val;
	adc_vals_x[11] -= compensated_val;

	compensated_val = tpadAdcDatas[trackpad][10] - tpadAdcComps[trackpad][10];
	adc_vals_x[0] += compensated_val;
	adc_vals_x[1] -= compensated_val;
	adc_vals_x[2] += compensated_val;
	adc_vals_x[3] += compensated_val;
	adc_vals_x[4] += compensated_val;
	adc_vals_x[5] -= compensated_val;
	adc_vals_x[6] -= compensated_val;
	adc_vals_x[7] -= compensated_val;
	adc_vals_x[8] += compensated_val;
	adc_vals_x[9] -= compensated_val;
	adc_vals_x[10] += compensated_val;
	adc_vals_x[11] -= compensated_val;

	for (int idx = 0; idx < 12; idx++) {
		if (adc_vals_x[idx] < 0)
			adc_vals_x[idx] = 0;
	}

	int32_t dividend = 0;
	int32_t divisor = 0;
	int32_t factor = 0;
	for (int idx = 0; idx < 12; idx++) {
		dividend += factor * adc_vals_x[idx];
		divisor += adc_vals_x[idx];
		factor += 100;
	}

	int32_t x_pos = -1;
	if (divisor) {
		x_pos = dividend / divisor;	
		x_pos = 1200 - x_pos;
	}

	// Wait for AnyMeas ADCs related to Y position to be updated
	while (tpadAdcIdxs[trackpad] < NUM_ANYMEAS_ADCS) {
	}

	// Calculate yLoc
	int32_t adc_vals_y[8];

	compensated_val = tpadAdcDatas[trackpad][11] - tpadAdcComps[trackpad][11];
	adc_vals_y[0] = -compensated_val;
	adc_vals_y[1] = compensated_val;
	adc_vals_y[2] = -compensated_val;
	adc_vals_y[3] = compensated_val;
	adc_vals_y[4] = -compensated_val;
	adc_vals_y[5] = compensated_val;
	adc_vals_y[6] = -compensated_val;
	adc_vals_y[7] = compensated_val;

	compensated_val = tpadAdcDatas[trackpad][12] - tpadAdcComps[trackpad][12];
	adc_vals_y[0] -= compensated_val;
	adc_vals_y[1] -= compensated_val;
	adc_vals_y[2] += compensated_val;
	adc_vals_y[3] += compensated_val;
	adc_vals_y[4] -= compensated_val;
	adc_vals_y[5] -= compensated_val;
	adc_vals_y[6] += compensated_val;
	adc_vals_y[7] += compensated_val;

	compensated_val = tpadAdcDatas[trackpad][13] - tpadAdcComps[trackpad][13];
	adc_vals_y[0] += compensated_val;
	adc_vals_y[1] -= compensated_val;
	adc_vals_y[2] -= compensated_val;
	adc_vals_y[3] += compensated_val;
	adc_vals_y[4] += compensated_val;
	adc_vals_y[5] -= compensated_val;
	adc_vals_y[6] -= compensated_val;
	adc_vals_y[7] += compensated_val;

	compensated_val = tpadAdcDatas[trackpad][14] - tpadAdcComps[trackpad][14];
	adc_vals_y[0] -= compensated_val;
	adc_vals_y[1] -= compensated_val;
	adc_vals_y[2] -= compensated_val;
	adc_vals_y[3] -= compensated_val;
	adc_vals_y[4] += compensated_val;
	adc_vals_y[5] += compensated_val;
	adc_vals_y[6] += compensated_val;
	adc_vals_y[7] += compensated_val;

	compensated_val = tpadAdcDatas[trackpad][15] - tpadAdcComps[trackpad][15];
	adc_vals_y[0] += compensated_val;
	adc_vals_y[1] -= compensated_val;
	adc_vals_y[2] += compensated_val;
	adc_vals_y[3] -= compensated_val;
	adc_vals_y[4] -= compensated_val;
	adc_vals_y[5] += compensated_val;
	adc_vals_y[6] -= compensated_val;
	adc_vals_y[7] += compensated_val;

	compensated_val = tpadAdcDatas[trackpad][16] - tpadAdcComps[trackpad][16];
	adc_vals_y[0] += compensated_val;
	adc_vals_y[1] += compensated_val;
	adc_vals_y[2] -= compensated_val;
	adc_vals_y[3] -= compensated_val;
	adc_vals_y[4] -= compensated_val;
	adc_vals_y[5] -= compensated_val;
	adc_vals_y[6] += compensated_val;
	adc_vals_y[7] += compensated_val;

	compensated_val = tpadAdcDatas[trackpad][17] - tpadAdcComps[trackpad][17];
	adc_vals_y[0] -= compensated_val;
	adc_vals_y[1] += compensated_val;
	adc_vals_y[2] += compensated_val;
	adc_vals_y[3] -= compensated_val;
	adc_vals_y[4] += compensated_val;
	adc_vals_y[5] -= compensated_val;
	adc_vals_y[6] -= compensated_val;
	adc_vals_y[7] += compensated_val;

	for (int idx = 0; idx < 8; idx++) {
		if (adc_vals_y[idx] < 0)
			adc_vals_y[idx] = 0;
		adc_vals_y[idx] /= 1000;
	}

	dividend = 0;
	divisor = 0;
	factor = 0;
	for (int idx = 0; idx < 8; idx++) {
		dividend += factor * adc_vals_y[idx];
		divisor += adc_vals_y[idx];
		factor += 100;
	}

	int32_t y_pos = -1;
	if (divisor) {
		y_pos = dividend / divisor;	
	}

	// Update outputs if finger was down (i.e. x_pos and y_pos are both valid)
	if (x_pos > 0 && y_pos > 0)  {
		*xLoc = x_pos;
		*yLoc = y_pos;
	}
}

/**
 * Setup Trackpad ASIC (i.e. configure registers, calibration, setup ISR).
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * 
 * \return None.
 */
static void setupTpad(Trackpad trackpad) {
	// Reset the TrackpadASIC:
	writeTpadReg(trackpad, TPAD_SYSCFG1_ADDR, TPAD_SYSCFG1_RESET_BIT);

	usleep(50 * 1000);

	while (!(TPAD_STATUS1_CC_BIT & readTpadReg(trackpad, TPAD_STATUS1_ADDR))) {
	}

	clearTpadFlags(trackpad);

	usleep(10 * 1000);

	// Check Firmware ID
	uint8_t fw_id = readTpadReg(trackpad, TPAD_FW_ID_ADDR);
	if (fw_id != 0x07)
		return;

	// Check Firmware Version
	uint8_t fw_ver = readTpadReg(trackpad, TPAD_FW_VER_ADDR);
	if (fw_ver != 0x3a) 
		return;

	// Stop Trackpad ASIC internal calculations
	writeTpadReg(trackpad, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_TRACKDIS_BIT);
	
	// Delay after track disable to allow for tracking operations to finish 
	usleep(10 * 1000);

	clearTpadFlags(trackpad);

	// Set default states for all registers:
	writeTpadReg(trackpad, TPAD_ADCCFG1_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_ADCCTRL_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_ADCMUXCTRL_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_ADCCFG2_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_ADCWIDTH_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_TOGGLE_HIHI_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_TOGGLE_HILO_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_TOGGLE_LOHI_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_TOGGLE_LOLO_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_POLARITY_HIHI_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_POLARITY_HILO_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_POLARITY_LOHI_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_POLARITY_LOLO_ADDR, 0x00);

	setTpadAdcStartAddr(trackpad, 0x0013);

	setTpadNumMeas(trackpad, 1);
	writeTpadReg(trackpad, TPAD_MEASCTRL_ADDR, 0x41);
	writeTpadReg(trackpad, TPAD_MEASINDEX_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_ANYMEASSTATE_ADDR, 0x00);
	writeTpadReg(trackpad, TPAD_ADCCFG2_ADDR, 0x00);

	// Load Compensation Matrix Data (I think...):
	//  According to datasheet: A compensation matrix of 92 values (each 
	//  value is 16 bits signed) is stored sequentially in Pinnacle RAM, 
	//  with the first value being stored at 0x01DF. 
	uint8_t era_data[8];

	// Comensation Matrix Data for AnyMeas ADCs for Y axis location?
	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x05;
	era_data[7] = 0x50;
	writeTpadExtRegs(trackpad, 0x015b, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x06;
	era_data[7] = 0x60;
	writeTpadExtRegs(trackpad, 0x0163, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x04;
	era_data[7] = 0xc8;
	writeTpadExtRegs(trackpad, 0x016b, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x07;
	era_data[7] = 0x80;
	writeTpadExtRegs(trackpad, 0x0173, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x05;
	era_data[7] = 0x28;
	writeTpadExtRegs(trackpad, 0x017b, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x06;
	era_data[7] = 0x18;
	writeTpadExtRegs(trackpad, 0x0183, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x04;
	era_data[7] = 0xb0;
	writeTpadExtRegs(trackpad, 0x018b, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x0193, 8, era_data);

	// Comensation Matrix Data for AnyMeas ADCs for X axis location?
	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x02;
	era_data[5] = 0x3b;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x01df, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x04;
	era_data[5] = 0x76;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x01e7, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x00;
	era_data[5] = 0xed;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x01ef, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x01;
	era_data[5] = 0xda;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x01f7, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x03;
	era_data[5] = 0xb4;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x01ff, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x07;
	era_data[5] = 0x68;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x0207, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x06;
	era_data[5] = 0xd1;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x020f, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x05;
	era_data[5] = 0xa3;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x0217, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x03;
	era_data[5] = 0x47;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x021f, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x06;
	era_data[5] = 0x8e;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x0227, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x05;
	era_data[5] = 0x1d;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writeTpadExtRegs(trackpad, 0x022f, 8, era_data);

	setTpadAdcCfg(trackpad, TPAD_ADC_GAIN0, TPAD_ADC_TOGGLE_FREQ_0,
		TPAD_ADC_SAMPLEN_256, TPAD_ADC_MUXSEL_SENSEP1GATE, 0, 
		TPAD_ADC_APETURE_500NS);

	era_data[0] = 0x64;
	era_data[1] = 0x03;
	writeTpadExtRegs(trackpad, 0x00d8, 2, era_data);

	clearTpadFlags(trackpad);

	takeTpadAdcMeas(trackpad, 0x00000000, 0x00000000, 0);
	takeTpadAdcMeas(trackpad, 0x000007f8, 0x00000550, 0);
	takeTpadAdcMeas(trackpad, 0x0fff0000, 0x023b0000, 0);
	
	clearTpadFlags(trackpad);

	// Setting PINT so we can react to PINT rising edge
	setupTpadISR(trackpad);

	// Compute compensation values (i.e. average value of ADCs, assuming
	//  no input during initialization).
	int comp_accums[NUM_ANYMEAS_ADCS];
	memset(comp_accums, 0, sizeof(int) * NUM_ANYMEAS_ADCS);

	static int NUM_COMP_AVGS = 16;
	for (int comp_cnt = 0; comp_cnt < NUM_COMP_AVGS; comp_cnt++) {
		// Request X and Y AnyMeas ADC measurements
		trackpadLocUpdate(trackpad);

		// Wait for AnyMeas ADCs related to X position to be updated
		while (tpadAdcIdxs[trackpad] < NUM_ANYMEAS_ADCS) {
			__WFI();
		}

		for (int comp_idx = 0; comp_idx < NUM_ANYMEAS_ADCS; comp_idx++) {
			comp_accums[comp_idx] += tpadAdcDatas[trackpad][comp_idx];
		}
	}

	for (int comp_idx = 0; comp_idx < NUM_ANYMEAS_ADCS; comp_idx++) {
		tpadAdcComps[trackpad][comp_idx] = comp_accums[comp_idx] 
			/ NUM_COMP_AVGS;
	}
}


#endif // ANYMEAS_EN


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
	writeTpadReg(R_TRACKPAD, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_SHUTDOWN_BIT);

	// Left Trackpad comms setup
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_L_TRACKPAD_CS_N);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_CS_N, IOCON_DIGMODE_EN |
		IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_DR, IOCON_DIGMODE_EN | 
		IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Place Left Trackpad in shutdown mode
	writeTpadReg(L_TRACKPAD, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_SHUTDOWN_BIT);

	setupTpad(R_TRACKPAD);
	setupTpad(L_TRACKPAD);
}


#if (ANYMEAS_EN)


/**
 * Function to be called by ISR to handle next ADC value.
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * 
 * \return None.
 */
void getNextTpadAdcValIsr(Trackpad trackpad) {
	volatile int16_t* tpad_adc_datas = tpadAdcDatas[trackpad];
	int tpad_adc_idx = tpadAdcIdxs[trackpad];

	tpad_adc_datas[tpad_adc_idx] = getTpadAdcAndClr(trackpad);
	tpad_adc_idx++;

	if (tpad_adc_idx == NUM_ANYMEAS_X_ADCS) {
		// Request measurements used for position on Y axis
		setTpadAdcStartAddr(trackpad, ANYMEAS_Y_ADC_ADDR);
		setTpadNumMeas(trackpad, NUM_ANYMEAS_Y_ADCS);

		// Start the measurement
		writeTpadReg(trackpad, TPAD_SYSCFG1_ADDR, 
			TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);
	}

	tpadAdcIdxs[trackpad] = tpad_adc_idx;
}


#endif // ANYMEAS_EN


/**
 * ISR for 3 - GPIO pin interrupt 3, which occurs on rising edge of Right 
 *  Haptic DR.
 * 
 * \return None.
 */
void FLEX_INT3_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(PINT_R_TRACKPAD));

#if (!ANYMEAS_EN)
	getLatestTpadData(R_TRACKPAD);
#else  // if (ANYMEAS_EN)
	getNextTpadAdcValIsr(R_TRACKPAD);
#endif // ANYMEAS_EN
}

/**
 * ISR for 4 - GPIO pin interrupt 4, which occurs on rising edge of Left 
 *  Haptic DR.
 * 
 * \return None.
 */
void FLEX_INT4_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(PINT_L_TRACKPAD));

#if (!ANYMEAS_EN)
	getLatestTpadData(L_TRACKPAD);
#else  // if (ANYMEAS_EN)
	getNextTpadAdcValIsr(L_TRACKPAD);
#endif // ANYMEAS_EN
}

/**
 * Print command usage details to console.
 *
 * \return None.
 */
void trackpadCmdUsage(void) {
	printf(

#if (!ANYMEAS_EN)

		"usage: trackpad ??? (WIP)\n"

#else // if (ANYMEAS_EN)

		"usage: trackpad monitor\n"
		"       trackpad getRaw\n"
		"       trackpad readReg left/right addr\n"
		"       trackpad writeReg left/right addr val\n"
		"\n"
		"monitor: Monitor X/Y position calculated for each Trackpad\n"
		"getRaw: print single set of raw ADC readings and compensation\n" 
		"	data (ideal for inserting into simulations)\n"
		"readReg/writeReg: Access Trackpad ASIC Regiters\n"
#endif
	);
}

/**
 * Function that will entire loop which prints out X/Y position for each 
 *  Trackpad until a key is pressed to exit.
 *
 * \return None.
 */
void tpadMonitor(void) {
	printf("Trackpad X/Y Location (Press any key to exit):\n");
	printf("\n");
	printf("Time             Left X Left Y Right X Right Y\n");
	printf("----------------------------------------------\n");

	while (!usb_tstc()) {
		uint16_t x_loc = 0;
		uint16_t y_loc = 0;

		trackpadLocUpdate(L_TRACKPAD);
		trackpadLocUpdate(R_TRACKPAD);

		printf("0x%08x       ", getUsTickCnt());

		trackpadGetLastXY(L_TRACKPAD, &x_loc, &y_loc);

		printf("  %4d ", x_loc);
		printf("  %4d ", y_loc);

		trackpadGetLastXY(R_TRACKPAD, &x_loc, &y_loc);

		printf("   %4d ", x_loc);
		printf("   %4d ", y_loc);

		printf("\r");
		usb_flush();

		usleep(10 * 1000);
	}
}

/**
 * Print single set of raw ADC readings and compensation data (ideal for 
 *  inserting into simulations)"
 *
 * \return None.
 */
void tpadGetRaw(void) {
	int16_t eeprom_comps[NUM_ANYMEAS_ADCS];
	eepromRead(0x628, eeprom_comps, sizeof(eeprom_comps));
	for (int idx = 0; idx < NUM_ANYMEAS_ADCS; idx++) {
		printf("Left Compensation Vals[%d] = %d %d\n", idx, 
			tpadAdcComps[L_TRACKPAD][idx], eeprom_comps[idx]);
	}
	printf("\n");

	eepromRead(0x602, eeprom_comps, sizeof(eeprom_comps));
	for (int idx = 0; idx < NUM_ANYMEAS_ADCS; idx++) {
		printf("Right Compensation Vals[%d] = %d %d\n", idx, 
			tpadAdcComps[R_TRACKPAD][idx], eeprom_comps[idx]);
	}
	printf("\n");

	uint16_t x_loc = 0;
	uint16_t y_loc = 0;

	trackpadLocUpdate(L_TRACKPAD);
	trackpadLocUpdate(R_TRACKPAD);

	printf("# Left Trackpad AnyMeas ADC Vals:\n");

	trackpadGetLastXY(L_TRACKPAD, &x_loc, &y_loc);

	for (int idx = 0; idx < NUM_ANYMEAS_ADCS; idx++) {
		uint32_t base_addr = 0x10000a5e;
		printf("set {short}0x%08x = %d\n", base_addr + 2 * idx, 
			tpadAdcDatas[L_TRACKPAD][idx]);
		printf("set {short}0x%08x = %d\n", 0x4c + base_addr + 2 * idx, 
			tpadAdcDatas[L_TRACKPAD][idx]);
	}
	printf("\n");

	printf("# Right Trackpad AnyMeas ADC Vals:\n");

	trackpadGetLastXY(R_TRACKPAD, &x_loc, &y_loc);

	for (int idx = 0; idx < NUM_ANYMEAS_ADCS; idx++) {
		uint32_t base_addr = 0x10000a38;
		printf("set {short}0x%08x = %d\n", base_addr + 2 * idx, 
			tpadAdcDatas[R_TRACKPAD][idx]);
		printf("set {short}0x%08x = %d\n", 0x4c + base_addr + 2 * idx, 
			tpadAdcDatas[R_TRACKPAD][idx]);
	}
	printf("\n");
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

#if (!ANYMEAS_EN)

	Trackpad trackpad = R_TRACKPAD;

	printf("Firmware ID = 0x%02x\n", readTpadReg(trackpad, TPAD_FW_ID_ADDR));
	printf("Firmware Version = 0x%02x\n", readTpadReg(trackpad, TPAD_FW_VER_ADDR));

	printf("Status 1 = 0x%02x\n", readTpadReg(trackpad, 0x02));
	printf("Sys Config 1 = 0x%02x\n", readTpadReg(trackpad, 0x03));
	printf("Feed Config 1 = 0x%02x\n", readTpadReg(trackpad, 0x04));
	printf("Feed Config 2 = 0x%02x\n", readTpadReg(trackpad, 0x05));
	printf("Feed Config 3 = 0x%02x\n", readTpadReg(trackpad, 0x06));
	printf("Cal Config 1 = 0x%02x\n", readTpadReg(trackpad, 0x07));
	printf("PS/2 Aux Control = 0x%02x\n", readTpadReg(trackpad, 0x08));
	printf("Sample Rate = 0x%02x\n", readTpadReg(trackpad, 0x09));
	printf("Z Idle = 0x%02x\n", readTpadReg(trackpad, TPAD_ZIDLE_ADDR));
	printf("Z Scaler = 0x%02x\n", readTpadReg(trackpad, TPAD_ZSCALER_ADDR));
	printf("Sleep Interval = 0x%02x\n", readTpadReg(trackpad, TPAD_SLEEPINTERVAL_ADDR));
	printf("Sleep Timer = 0x%02x\n", readTpadReg(trackpad, TPAD_SLEEPTIMER_ADDR));
	printf("Dynamic EMI Adjust = 0x%02x\n", readTpadReg(trackpad, 0x0E));

	while (!usb_tstc()) {

		int idx = 0;//tpadAbsDataIdxs[trackpad];
		printf("[%d]: X = %4d, Y = %4d, Z = %4d\r", idx, 
			tpadAbsDatas[trackpad][idx].xPos, 
			tpadAbsDatas[trackpad][idx].yPos, 
			tpadAbsDatas[trackpad][idx].zPos);

		usleep(10 * 1000);
	}

#else // if (ANYMEAS_EN)

	if (argc < 2) {
		trackpadCmdUsage();
		return -1;
	}

	if (!strcmp("monitor", argv[1])) {
		tpadMonitor();
	} else if (!strcmp("getRaw", argv[1])) {
		tpadGetRaw();
	} else if (!strcmp("readReg", argv[1])) {
		if (argc != 4) {
			trackpadCmdUsage();
			return -1;
		}
		Trackpad trackpad = R_TRACKPAD;
		if (!strcmp("left", argv[2])) {
			trackpad = L_TRACKPAD;
		}
		uint32_t addr = strtol(argv[3], NULL, 0);

		printf("Read 0x%02x from register 0x%02x of %s Trackpad.\n",
			readTpadReg(trackpad, addr), addr, 
			trackpad == R_TRACKPAD ? "right":"left");
	} else if (!strcmp("writeReg", argv[1])) {
		if (argc != 5) {
			trackpadCmdUsage();
			return -1;
		}
		Trackpad trackpad = R_TRACKPAD;
		if (!strcmp("left", argv[2])) {
			trackpad = L_TRACKPAD;
		}
		uint32_t addr = strtol(argv[3], NULL, 0);
		uint32_t val = strtol(argv[4], NULL, 0);

		printf("Writing 0x%02x to register 0x%02x of %s Trackpad.\n",
			val, addr, trackpad == R_TRACKPAD ? "right":"left");
		
		writeTpadReg(trackpad, addr, val);
	} else {
		trackpadCmdUsage();
		return -1;
	}

#endif // ANYMEAS_EN

	return 0;
}
