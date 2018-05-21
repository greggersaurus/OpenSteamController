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
void fnc0x1fff1ff0( arg0x1fff1ff0_0, arg0x1fff1ff0_1);

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

