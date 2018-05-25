/**
 * This file attempts to track function usage and purpose for 
 *  vcf_wired_controller_d0g_57bf5c10.bin. This data was obtained via exeLog 
 *  outputs from pinkySim.
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

/**
 * Early initialization for selecting and enabling most basic clock sources 
 *  (i.e. System clock). May not return if PLLs do not lock.
 *
 * \return None.
 */
void fnc0x00000fe0();

/**
 * Main entry function for Steam Controller Firmware? Have not reached return
 *  in simulation yet...
 *
 * \return Unknown.
 */
?? fnc0x0000154c();

/**
 * Get/Calculate the system clock frequency and save to a global variable.
 * 
 * \return The system System Clock Frequency in Hz.
 */
int fnc0x00000fd0();

/**
 * Main funtion for getting the system clock frequency. 
 *
 * \return The system System Clock Frequency in Hz.
 */
int fnc0x00000494();

/**
 * Related to getting the system clock frequency.
 *
 * \return The system System Clock Frequency in Hz.
 */
int fnc0x00000450();

/**
 * Related to getting the system clock frequency.
 *
 * Get the clock frequency (having already checked that the clock source is
 *  PLL output).
 *
 * \return The system System Clock Frequency in Hz.
 */
int fnc0x000004d0();

/**
 * Related to getting the system clock frequency. Performs checks on the 
 *  input parameters...
 *
 * \param arg0x00000300_0 The system clock frequency in Hz returned by 
 *  	fnc0x00000454().
 * \param arg0x00000300_1 SYSAHBCLKDIV register value.
 *
 * \return The system System Clock Frequency in Hz.
 */
int fnc0x00000300(arg0x00000300_0, arg0x00000300_1);

/**
 * Check if EEPROM contents are valid. If they are not, code goes down some
 *  other (currently unknown) path... TODO: I think this path just writes magic
 *  word to EEPROM...
 *
 * \return None.
 */
void fnc0x00000d04();

/**
 * EEPROM Read
 *
 * \param arg0x00000bdc_0 - EEPROM address to read
 * \param arg0x00000bdc_1 - RAM address to write EEPROM data to
 * \param arg0x00000bdc_2 - Number of bytes to read from EEPROM
 *
 * \return None.
 */
void fnc0x00000bdc(arg0x00000bdc_0, arg0x00000bdc_1, arg0x00000bdc_2);

/**
 * iap_command() wrapper (assuming command_param[4] is already on stack...).
 *
 * \param arg0x00000bb4_0 - command_param[0] : IAP Command Code
 * \param arg0x00000bb4_1 - command_param[1] : IAP Command Specific
 * \param arg0x00000bb4_2 - command_param[2] : IAP Command Specific
 * \param arg0x00000bb4_3 - command_param[3] : IAP Command Specific
 *
 * \return None.
 */
void fnc0x00000bb4(arg0x00000bb4_0, arg0x00000bb4_1, arg0x00000bb4_2, arg0x00000bb4_3);

/**
 * iap_entry function. See 20.14 IAP commands in UM10462 for details.
 *
 * \param arg0x1fff1ff0_0 - unsigned int[] command_param
 * \param arg0x1fff1ff0_1 - unsigned int[] status_result
 */
void fnc0x1fff1ff0(arg0x1fff1ff0_0, arg0x1fff1ff0_1);

/**
 * Drive Battery/Power Related GPIO. This function checks the hardware version
 *  and drives the appropriate GPIO to do something related to whether to use
 *  battery power or not (I think).
 *
 * \param arg0x00000f90_0 - Determines whether to drive GPIO high or low 
 *	(Set to 1 if VBUS is high).
 *
 * \return None.
 */
void fnc0x00000f90(arg0x00000f90_0);

/**
 * Memory initialization function. Memory is filled byte by byte. Sometimes 
 *  memory is filled via constant data from value stored in code, other times
 *  it is zeroed.
 *
 * \param arg0x00002a2a_0 Base address of non-zero constant values to write.
 * \param arg0x00002a2a_1 Base addess of RAM to initialize.
 * \param arg0x00002a2a_2 Number of bytes to write.
 * 
 * \return None.
 */
void fnc0x00002a2a(arg0x00002a2a_0, arg0x00002a2a_1, arg0x00002a2a_2);

/**
 * Calculate System Clock Frequency and save to (global?) variable
 * 
 * \return None.
 */
void fnc0x00009cfc();

/**
 * AKA uint32_t Chip_Clock_GetSystemClockRate(void) from
 *  lpc_chip_11uxx_lib: clock_11xx.c
 *
 * Return system clock rate (in Hz)
 */
uint32_t fnc0x00004174();

/**
 * AKA uint32_t Chip_Clock_GetMainClockRate(void) from
 *  lpc_chip_11uxx_lib: clock_11xx.c
 *
 * \return main clock rate (in Hz)
 */
uint32_t fnc0x00004130();

/**
 * AKA uint32_t chip_clock_getsystemplloutclockrate(void) from 
 *  lpc_chip_11uxx_lib: clock_11xx.c
 * 
 * \return system pll output clock rate (in Hz)
 */
uint32_t fnc0x000041b0();

/**
 * Compute arg0x000020ec_0 / arg0x000020ec_1. Note this processor has no FPU.
 *
 * \param arg0x000020ec_0 Dividend
 * \param arg0x000020ec_1 Divisor
 *
 * \return arg0x000020ec_0 / arg0x000020ec_1.
 */
int fnc0x000020ec(arg0x000020ec_0, arg0x000020ec_1);

/**
 * Set interrupt priorities.
 *
 * \return None.
 */
void fnc0x00009c50();

/**
 * Clear some RAM values.
 *
 * \return None.
 */
void fnc0x0000a0d8();

/**
 * Enable some (more) system clocks.
 *
 * \return None.
 */
void fnc0x00005644();

/**
 * Configure ADC
 *
 * \return None.
 */
void fnc0x00002ba0();

/** 
 * AKA void Chip_ADC_Init(LPC_ADC_T *pADC, ADC_CLOCK_SETUP_T *ADCSetup) from
 *  lpc_chip_11uxx_lib: adc_11xx.c.
 * Initialize the ADC peripheral and the ADC setup structure to default value
 *
 * \param arg0x00003fd0_0 pADC The base of ADC peripheral on the chip
 * \param arg0x00003fd0_1 ADCSetup ADC setup structure to be set
 *
 * \return None.
 */
void fnc0x00003fd0(arg0x00003fd0_0, arg0x00003fd0_1);

/**
 * AKA STATIC uint8_t getClkDiv(LPC_ADC_T *pADC, bool burstMode, uint32_t adcRate, uint8_t clks) from
 *  lpc_chip_11uxx_lib: adc_11xx.c.
 * 
 * \param arg0x0000d0e8_0 pADC Pointer to base control register for ADC being configured (not used in this function though...).
 * \param arg0x0000d0e8_1 burstMode Determines how full ADC rate is calculated.
 * \param arg0x0000d0e8_2 adcRate Desired ADC rate.
 * \param arg0x0000d0e8_3 clks If burstMode is true this is used instead of hardcode "number of clock for a full conversion"
 * 
 * \return Clock divider for ADC
 */
uint8_t fnc0x0000d0e8(arg0x0000d0e8_0, arg0x0000d0e8_1, arg0x0000d0e8_2, arg0x0000d0e8_3);

/**
 * AKA void Chip_ADC_SetSampleRate(LPC_ADC_T *pADC, ADC_CLOCK_SETUP_T *ADCSetup, uint32_t rate) from
 *  lpc_chip_11uxx_lib: adc_11xx.c.
 *
 * \param arg0x0000404c_0 pADC The base of ADC peripheral on the chip
 * \param arg0x0000404c_1 ADCSetup ADC setup structure to be set
 * \param arg0x0000404c_2 rate Sample rate, should be set so the clock for A/D converter is less than or equal to 4.5MHz.
 *
 * \return None.
 */
void fnc0x0000404c(arg0x0000404c_0, arg0x0000404c_1, arg0x0000404c_2);

/**
 * AKA void Chip_ADC_SetStartMode(LPC_ADC_T *pADC, ADC_START_MODE_T mode, ADC_EDGE_CFG_T EdgeOption) from
 *  lpc_chip_11uxx_lib: adc_11xx.c.
 *
 * \param arg0x00004080_0 pADC The base of ADC peripheral on the chip
 * \param arg0x00004080_1 mode Stating mode, should be :
 *							- ADC_NO_START				: Must be set for Burst mode
 *							- ADC_START_NOW				: Start conversion now
 *							- ADC_START_ON_CTOUT15		: Start conversion when the edge selected by bit 27 occurs on CTOUT_15
 *							- ADC_START_ON_CTOUT8		: Start conversion when the edge selected by bit 27 occurs on CTOUT_8
 *							- ADC_START_ON_ADCTRIG0		: Start conversion when the edge selected by bit 27 occurs on ADCTRIG0
 *							- ADC_START_ON_ADCTRIG1		: Start conversion when the edge selected by bit 27 occurs on ADCTRIG1
 *							- ADC_START_ON_MCOA2		: Start conversion when the edge selected by bit 27 occurs on Motocon PWM output MCOA2
 * \param arg0x00004080_2 EdgeOption Stating Edge Condition, should be :
 *							- ADC_TRIGGERMODE_RISING	: Trigger event on rising edge
 *							- ADC_TRIGGERMODE_FALLING	: Trigger event on falling edge
 *
 * \return None.
 */
void fnc0x00004080(arg0x00004080_0, arg0x00004080_1, arg0x00004080_2);
