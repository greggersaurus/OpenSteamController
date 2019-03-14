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
#include <string.h>

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

#define PINT_R_TRACKPAD 3
#define PINT_L_TRACKPAD 4

#define NUM_ANYMEAS_ADCS (19) //!< Number of ADC channels read in AnyMeas
	//!< mode

static int16_t tpadAdcComps[2][NUM_ANYMEAS_ADCS]; //!< Compensation values 
	//!< for ADC channels read from trackpad in AnyMeas mode. There are 19
	//!< channels read for each of the two trackpads.
static volatile int16_t tpadAdcDatas[2][NUM_ANYMEAS_ADCS]; //!< The ADC 
	//!< values most recently read via ISR.
static volatile int tpadAdcIdxs[2]; //!< Used to track where next ADC value
	//!< read from ISR should be written in tpad_adc_datas.

// Trackpad ASIC Registers:
//TODO: Specify AnyMeas v.s. Normal?
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

// Note: The following register descriptions only apply to AnyMeas mode. If
//	 the AnyMeas bit is not set, these registers have different purposes.
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
	writePinnacleReg(trackpad, TPAD_ERA_HIADDR_ADDR, 0xFF & (addr >> 8));
	writePinnacleReg(trackpad, TPAD_ERA_LOADDR_ADDR, 0xFF & addr);

	for (int idx = 0; idx < len; idx++) {
		// Write value
		writePinnacleReg(trackpad, TPAD_ERA_VAL_ADDR, data[idx]);

		// Write ERA auto-increment write to ERA Control
		writePinnacleReg(trackpad, TPAD_ERA_CTRL_ADDR, 0x0A);

		// Read ERA Control until it contains 0x00
		while (readPinnacleReg(trackpad, TPAD_ERA_CTRL_ADDR)){
		}
	}
}

/**
 * Get the latest AnyMeas ADC reading and Clear Flags, all in a single burst
 *  of SPI data.
 *
 * \param trackpad Specifies which trackpad to communicate with. 
 *
 * \return The ADC value.
 */
static int16_t getPinnacleAdcAndClr(Trackpad trackpad) {
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
	tx_data[1] = 0xFC;
	tx_data[2] = 0xFC;
	tx_data[3] = 0xFC;
	tx_data[4] = 0xFB;
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
 * Clear Software Command Complete and Software Data Ready flags for the
 *  specifica trackpad.
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * 
 * \return None.
 */
static inline void clearFlagsPinnacle(Trackpad trackpad) {
	writePinnacleReg(trackpad, TPAD_STATUS1_ADDR, 0x00);
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
static void setAdcCfgPinnacle(Trackpad trackpad, TpadAdcGain gain, 
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
		writePinnacleReg(trackpad, TPAD_ADCCFG1_ADDR, cfg1_update);
		cfg1_shadow[trackpad] = cfg1_update;
	}
	if (ctrl_shadow[trackpad] != ctrl_update) {
		writePinnacleReg(trackpad, TPAD_ADCCTRL_ADDR, ctrl_update);
		ctrl_shadow[trackpad] = ctrl_update;
	}
	if (mux_ctrl_shadow[trackpad] != mux_ctrl_update) {
		writePinnacleReg(trackpad, TPAD_ADCMUXCTRL_ADDR, mux_ctrl_update);
		mux_ctrl_shadow[trackpad] = mux_ctrl_update;
	}
	if (cfg2_shadow[trackpad] != cfg2_update) {
		writePinnacleReg(trackpad, TPAD_ADCCFG2_ADDR, cfg2_update);
		cfg2_shadow[trackpad] = cfg2_update;
	}
	if (width_shadow[trackpad] != width_update) {
		writePinnacleReg(trackpad, TPAD_ADCWIDTH_ADDR, width_update);
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
static void setTogglePinnacle(Trackpad trackpad, uint32_t toggle) {
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
		writePinnacleReg(trackpad, TPAD_TOGGLE_HIHI_ADDR, toggle_hihi);
		toggle_hihi_shadow[trackpad] = toggle_hihi;
	}	
	if (toggle_hilo_shadow[trackpad] != toggle_hilo) {
		writePinnacleReg(trackpad, TPAD_TOGGLE_HILO_ADDR, toggle_hilo);
		toggle_hilo_shadow[trackpad] = toggle_hilo;
	}	
	if (toggle_lohi_shadow[trackpad] != toggle_lohi) {
		writePinnacleReg(trackpad, TPAD_TOGGLE_LOHI_ADDR, toggle_lohi);
		toggle_lohi_shadow[trackpad] = toggle_lohi;
	}	
	if (toggle_lolo_shadow[trackpad] != toggle_lolo) {
		writePinnacleReg(trackpad, TPAD_TOGGLE_LOLO_ADDR, toggle_lolo);
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
static void setPolarityPinnacle(Trackpad trackpad, uint32_t polarity) {
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
		writePinnacleReg(trackpad, TPAD_POLARITY_HIHI_ADDR, polarity_hihi);
		polarity_hihi_shadow[trackpad] = polarity_hihi;
	}	
	if (polarity_hilo_shadow[trackpad] != polarity_hilo) {
		writePinnacleReg(trackpad, TPAD_POLARITY_HILO_ADDR, polarity_hilo);
		polarity_hilo_shadow[trackpad] = polarity_hilo;
	}	
	if (polarity_lohi_shadow[trackpad] != polarity_lohi) {
		writePinnacleReg(trackpad, TPAD_POLARITY_LOHI_ADDR, polarity_lohi);
		polarity_lohi_shadow[trackpad] = polarity_lohi;
	}	
	if (polarity_lolo_shadow[trackpad] != polarity_lolo) {
		writePinnacleReg(trackpad, TPAD_POLARITY_LOLO_ADDR, polarity_lolo);
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
static void setAdcStartAddrPinnacle(Trackpad trackpad, uint16_t addr) {
	// Shadow copies to minimize SPI transactions
	uint8_t addr_hi_shadow[2] = {0, 0};
	uint8_t addr_lo_shadow[2] = {0, 0};

	uint8_t addr_hi = 0xFF & (addr >> 8);
	uint8_t addr_lo = 0xFF & addr;

	if (addr_hi_shadow[trackpad] != addr_hi) {
		writePinnacleReg(trackpad, TPAD_ADC_START_ADDR_HI_ADDR, addr_hi);
		addr_hi_shadow[trackpad] = addr_hi;
	}
	if (addr_lo_shadow[trackpad] != addr_lo) {
		writePinnacleReg(trackpad, TPAD_ADC_START_ADDR_LO_ADDR, addr_lo);
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
static void setNumMeasPinnacle(Trackpad trackpad, uint8_t numMeas) {
	static uint8_t meas_ctrl_shadow[2] = {0, 0};

	uint8_t meas_ctrl = TPAD_MEASCTRL_POSTMEASPWR_BIT | 
		(TPAD_MEASCTRL_NUMMEAS_MASK & numMeas);

	if (meas_ctrl_shadow[trackpad] != meas_ctrl) {
		writePinnacleReg(trackpad, TPAD_MEASCTRL_ADDR, meas_ctrl);
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
static int32_t takeAdcMeasPinnacle(Trackpad trackpad, uint32_t toggle, 
	uint32_t polarity, uint8_t adjust) {

	setTogglePinnacle(trackpad, toggle);
	setPolarityPinnacle(trackpad, polarity);
	// Put measurement data starting at Register 19???
	setAdcStartAddrPinnacle(trackpad, 0x0013);
	setNumMeasPinnacle(trackpad, 1);

	// Start the measurement
	writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

	usleep(2 * 1000);

	int16_t retval = getPinnacleAdcAndClr(trackpad);

	return (int32_t)(retval >> adjust);
}

//TODO: rename function? too similar to register access one?
/**
 * Request all NUM_ANYMEAS_ADCS ADC values be read. Return once tpadAdcDatas
 *  has the latest data.
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 *
 * \return None.
 */
void getPinnacleAdcVals(Trackpad trackpad) {
	tpadAdcIdxs[trackpad] = 0;

	// Request first 11 measurements.
	setAdcStartAddrPinnacle(trackpad, 0x01df);
	setNumMeasPinnacle(trackpad, 11);

	// Start the measurement
	writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

	// Wait for ADC reads to complete
	while (tpadAdcIdxs[trackpad] < NUM_ANYMEAS_ADCS) {
		__WFI();
	}
}

/**
 * Setup Trackpad ASIC (i.e. configure registers, calibration, setup ISR).
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * 
 * \return None.
 */
static void setupTrackpad(Trackpad trackpad) {
	// Reset the TrackpadASIC:
	writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, TPAD_SYSCFG1_RESET_BIT);

	usleep(50 * 1000);

	while (!(TPAD_STATUS1_CC_BIT & readPinnacleReg(trackpad, TPAD_STATUS1_ADDR))) {
	}

	clearFlagsPinnacle(trackpad);

	usleep(10 * 1000);

	// Check Firmware ID
	uint8_t fw_id = readPinnacleReg(trackpad, TPAD_FW_ID_ADDR);
	if (fw_id != 0x07)
		return;

	// Check Firmware Version
	uint8_t fw_ver = readPinnacleReg(trackpad, TPAD_FW_VER_ADDR);
	if (fw_ver != 0x3a) 
		return;

	// Stop Trackpad ASIC internal calculations
	writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_TRACKDIS_BIT);
	
	// Delay after track disable to allow for tracking operations to finish 
	usleep(10 * 1000);

	clearFlagsPinnacle(trackpad);

	// Set default states for all registers:
	writePinnacleReg(trackpad, TPAD_ADCCFG1_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_ADCCTRL_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_ADCMUXCTRL_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_ADCCFG2_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_ADCWIDTH_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_TOGGLE_HIHI_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_TOGGLE_HILO_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_TOGGLE_LOHI_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_TOGGLE_LOLO_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_POLARITY_HIHI_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_POLARITY_HILO_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_POLARITY_LOHI_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_POLARITY_LOLO_ADDR, 0x00);

	setAdcStartAddrPinnacle(trackpad, 0x0013);

	setNumMeasPinnacle(trackpad, 1);
	writePinnacleReg(trackpad, TPAD_MEASCTRL_ADDR, 0x41);
	writePinnacleReg(trackpad, TPAD_MEASINDEX_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_ANYMEASSTATE_ADDR, 0x00);
	writePinnacleReg(trackpad, TPAD_ADCCFG2_ADDR, 0x00);

	// Load Compensation Matrix Data (I think...):
	uint8_t era_data[8];

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x05;
	era_data[7] = 0x50;
	writePinnacleExtRegs(trackpad, 0x015b, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x06;
	era_data[7] = 0x60;
	writePinnacleExtRegs(trackpad, 0x0163, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x04;
	era_data[7] = 0xc8;
	writePinnacleExtRegs(trackpad, 0x016b, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x07;
	era_data[7] = 0x80;
	writePinnacleExtRegs(trackpad, 0x0173, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x05;
	era_data[7] = 0x28;
	writePinnacleExtRegs(trackpad, 0x017b, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x06;
	era_data[7] = 0x18;
	writePinnacleExtRegs(trackpad, 0x0183, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x07;
	era_data[3] = 0xf8;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x04;
	era_data[7] = 0xb0;
	writePinnacleExtRegs(trackpad, 0x018b, 8, era_data);

	era_data[0] = 0x00;
	era_data[1] = 0x00;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x00;
	era_data[5] = 0x00;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x0193, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x02;
	era_data[5] = 0x3b;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x01df, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x04;
	era_data[5] = 0x76;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x01e7, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x00;
	era_data[5] = 0xed;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x01ef, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x01;
	era_data[5] = 0xda;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x01f7, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x03;
	era_data[5] = 0xb4;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x01ff, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x07;
	era_data[5] = 0x68;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x0207, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x06;
	era_data[5] = 0xd1;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x020f, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x05;
	era_data[5] = 0xa3;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x0217, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x03;
	era_data[5] = 0x47;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x021f, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x06;
	era_data[5] = 0x8e;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x0227, 8, era_data);

	era_data[0] = 0x0f;
	era_data[1] = 0xff;
	era_data[2] = 0x00;
	era_data[3] = 0x00;
	era_data[4] = 0x05;
	era_data[5] = 0x1d;
	era_data[6] = 0x00;
	era_data[7] = 0x00;
	writePinnacleExtRegs(trackpad, 0x022f, 8, era_data);

	setAdcCfgPinnacle(trackpad, TPAD_ADC_GAIN0, TPAD_ADC_TOGGLE_FREQ_0,
		TPAD_ADC_SAMPLEN_256, TPAD_ADC_MUXSEL_SENSEP1GATE, 0, 
		TPAD_ADC_APETURE_500NS);

	era_data[0] = 0x64;
	era_data[1] = 0x03;
	writePinnacleExtRegs(trackpad, 0x00d8, 2, era_data);

	clearFlagsPinnacle(trackpad);

	takeAdcMeasPinnacle(trackpad, 0x00000000, 0x00000000, 0);
	takeAdcMeasPinnacle(trackpad, 0x000007f8, 0x00000550, 0);
	takeAdcMeasPinnacle(trackpad, 0x0fff0000, 0x023b0000, 0);
	
	clearFlagsPinnacle(trackpad);

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

	// Compute compensation values (i.e. average value of ADCs, assuming
	//  no input during initialization).
	int comp_accums[NUM_ANYMEAS_ADCS];
	memset(comp_accums, 0, sizeof(int) * NUM_ANYMEAS_ADCS);

	for (int comp_cnt = 0; comp_cnt < 16; comp_cnt++) {
		getPinnacleAdcVals(trackpad);

		for (int comp_idx = 0; comp_idx < NUM_ANYMEAS_ADCS; comp_idx++) {
			comp_accums[comp_idx] += tpadAdcDatas[trackpad][comp_idx];
		}
	}

	for (int comp_idx = 0; comp_idx < NUM_ANYMEAS_ADCS; comp_idx++) {
		tpadAdcComps[trackpad][comp_idx] = comp_accums[comp_idx] / 16;
	}
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
	writePinnacleReg(R_TRACKPAD, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_SHUTDOWN_BIT);

	// Left Trackpad comms setup
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_L_TRACKPAD_CS_N, true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_L_TRACKPAD_CS_N);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_CS_N, IOCON_DIGMODE_EN |
		IOCON_MODE_INACT, IOCON_FUNC0);

	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD_DR, IOCON_DIGMODE_EN | 
		IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Place Left Trackpad in shutdown mode
	writePinnacleReg(L_TRACKPAD, TPAD_SYSCFG1_ADDR, 
		TPAD_SYSCFG1_SHUTDOWN_BIT);

	setupTrackpad(R_TRACKPAD);
	setupTrackpad(L_TRACKPAD);
}

/**
 * Function to be called by ISR to handle next ADC value.
 * 
 * \param trackpad Specifies which trackpad to communicate with. 
 * 
 * \return None.
 */
void getNextPinnacleAdcValIsr(Trackpad trackpad) {
	volatile int16_t* tpad_adc_datas = tpadAdcDatas[trackpad];
	int tpad_adc_idx = tpadAdcIdxs[trackpad];

	tpad_adc_datas[tpad_adc_idx] = getPinnacleAdcAndClr(trackpad);
	tpad_adc_idx++;

	if (tpad_adc_idx == 11) {
		// Request next 8 measurements...
		setAdcStartAddrPinnacle(trackpad, 0x015b);
		setNumMeasPinnacle(trackpad, 8);

		// Start the measurement
		writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
			TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);
	}

	tpadAdcIdxs[trackpad] = tpad_adc_idx;
}

/**
 * ISR for 3 - GPIO pin interrupt 3, which occurs on rising edge of Right 
 *  Haptic DR.
 * 
 * \return None.
 */
void FLEX_INT3_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(PINT_R_TRACKPAD));

	getNextPinnacleAdcValIsr(R_TRACKPAD);
}

/**
 * ISR for 4 - GPIO pin interrupt 4, which occurs on rising edge of Left 
 *  Haptic DR.
 * 
 * \return None.
 */
void FLEX_INT4_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH(PINT_L_TRACKPAD));

	getNextPinnacleAdcValIsr(L_TRACKPAD);
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

	Trackpad trackpad = L_TRACKPAD;

	printf("Firmware ID = 0x%02x\n", readPinnacleReg(trackpad, TPAD_FW_ID_ADDR));
	printf("Firmware Version = 0x%02x\n", readPinnacleReg(trackpad, TPAD_FW_VER_ADDR));

	int16_t eeprom_comps[NUM_ANYMEAS_ADCS];
	if (trackpad == R_TRACKPAD) {
		eepromRead(0x602, eeprom_comps, sizeof(eeprom_comps));
	} else {
		eepromRead(0x628, eeprom_comps, sizeof(eeprom_comps));
	}

	for (int idx = 0; idx < NUM_ANYMEAS_ADCS; idx++) {
		printf("comps[%d] = %d %d\n", idx, tpadAdcComps[trackpad][idx], eeprom_comps[idx]);
	}
	
	while (!usb_tstc()) {
		getPinnacleAdcVals(trackpad);
		for (int idx = 0; idx < NUM_ANYMEAS_ADCS; idx++) {
			printf("% 5d ", tpadAdcDatas[trackpad][idx] - tpadAdcComps[trackpad][idx]);
		}
		printf("\r");

		usleep(50 * 1000);
	}

/*
	while (!usb_tstc()) {
		// Get first 11 measurements...
		setAdcStartAddrPinnacle(trackpad, 0x01df);
		setNumMeasPinnacle(trackpad, 11);

		// Start the measurement
		writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
			TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

		for (int cnt = 0; cnt < 11; cnt++) {
			// Wait for trackpad to say it has new data                             
			while (!Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD_DR)) {          
				//printf("DR Low for Right Trackpad\n");                        
			}

			printf("% 5d ", (int16_t)getPinnacleAdcAndClr(trackpad));
		}

		// Get next 8 measurements...
		setAdcStartAddrPinnacle(trackpad, 0x015b);
		setNumMeasPinnacle(trackpad, 8);

		// Start the measurement
		writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
			TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

		for (int cnt = 0; cnt < 8; cnt++) {
			// Wait for trackpad to say it has new data                             
			while (!Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD_DR)) {          
				//printf("DR Low for Right Trackpad\n");                        
			}

			printf("% 5d ", (int16_t)getPinnacleAdcAndClr(trackpad));
		}
		printf("\r");

		usleep(50 * 1000);
	}
*/

	
/*
	int comps[19];
	int comp_idx = 0;
	memset(comps, 0, 19*sizeof(int));

	for (int avg_cnt = 0; avg_cnt < 16; avg_cnt++) {
		comp_idx = 0;

		// Get first 11 measurements...
		setAdcStartAddrPinnacle(trackpad, 0x01df);
		setNumMeasPinnacle(trackpad, 11);

		// Start the measurement
		writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
			TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

		for (int cnt = 0; cnt < 11; cnt++) {
			// Wait for trackpad to say it has new data                             
			while (!Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD_DR)) {          
				//printf("DR Low for Right Trackpad\n");                        
			}

			comps[comp_idx] += (int16_t)getPinnacleAdcAndClr(trackpad);
			comp_idx++;
		}

		// Get next 8 measurements...
		setAdcStartAddrPinnacle(trackpad, 0x015b);
		setNumMeasPinnacle(trackpad, 8);

		// Start the measurement
		writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
			TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

		for (int cnt = 0; cnt < 8; cnt++) {
			// Wait for trackpad to say it has new data                             
			while (!Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD_DR)) {          
				//printf("DR Low for Right Trackpad\n");                        
			}

			comps[comp_idx] += (int16_t)getPinnacleAdcAndClr(trackpad);
			comp_idx++;
		}
	}

	for (int idx = 0; idx < 19; idx++) {
		comps[idx] /= 16;
		printf("0x%04x ", comps[idx]);
	}
	printf("\n");

	int vals_idx = 0;
	while (!usb_tstc()) {
		vals_idx = 0;

		// Get first 11 measurements...
		setAdcStartAddrPinnacle(trackpad, 0x01df);
		setNumMeasPinnacle(trackpad, 11);

		// Start the measurement
		writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
			TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

		for (int cnt = 0; cnt < 11; cnt++) {
			// Wait for trackpad to say it has new data                             
			while (!Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD_DR)) {          
				//printf("DR Low for Right Trackpad\n");                        
			}

			int val = (int16_t)getPinnacleAdcAndClr(trackpad);
			printf("% 5d ", val - comps[vals_idx]);
			vals_idx++;
		}

		// Get next 8 measurements...
		setAdcStartAddrPinnacle(trackpad, 0x015b);
		setNumMeasPinnacle(trackpad, 8);

		// Start the measurement
		writePinnacleReg(trackpad, TPAD_SYSCFG1_ADDR, 
			TPAD_SYSCFG1_ANYMEASEN_BIT | TPAD_SYSCFG1_TRACKDIS_BIT);

		for (int cnt = 0; cnt < 8; cnt++) {
			// Wait for trackpad to say it has new data                             
			while (!Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD_DR)) {          
				//printf("DR Low for Right Trackpad\n");                        
			}

			int val = (int16_t)getPinnacleAdcAndClr(trackpad);
			printf("% 5d ", val - comps[vals_idx]);
			vals_idx++;
		}
		printf("\r");

		usleep(50 * 1000);
	}
*/

	return 0;
}
