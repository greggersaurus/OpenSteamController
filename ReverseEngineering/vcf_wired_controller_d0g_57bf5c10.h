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
 * Only used in early init?
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
 * Only used in early init?
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

/**
 * Retrieve first 132 bytes of EEPROM data and save to RAM. Also check magic
 *  number from retrieved data.
 *
 * \return None.
 */
void fnc0x00006644();

/**
 * EEPROM Read
 * Only used after early init?
 *
 * \param arg0x000051c4_0 - EEPROM address to read
 * \param arg0x000051c4_1 - RAM address to write EEPROM data to
 * \param arg0x000051c4_2 - Number of bytes to read from EEPROM
 *
 * \return Status Code from IAP command (i.e. CMD_SUCCESS).
 */
uint32_t fnc0x000051c4(arg0x000051c4_0, arg0x000051c4_1, arg0x000051c4_2)

/**
 * iap_command() wrapper (assuming command_param[4] is already on stack...).
 * Only used after early init?
 *
 * \param arg0x00005d10_0 - command_param[0] : IAP Command Code
 * \param arg0x00005d10_1 - command_param[1] : IAP Command Specific
 * \param arg0x00005d10_2 - command_param[2] : IAP Command Specific
 * \param arg0x00005d10_3 - command_param[3] : IAP Command Specific
 *
 * \return Status Code from IAP command (i.e. CMD_SUCCESS).
 */
uint32_t fnc0x00005d10(arg0x00005d10_0, arg0x00005d10_1, arg0x00005d10_2, arg0x00005d10_3);

/**
 * Call fnc0x00004cd8() to clear a bunch fo RAM values.
 * Seems redundant as this is all function does and fnc0x00009184() calls
 *  same subfunction to clear same SRAM0 values...
 *
 * \return None.
 */
void fnc0x00006cb0();

/**
 * Clear a bunch of SRAM0 values.
 *
 * \return None.
 */
void fnc0x00004cd8();

/**
 * Clear and initialize a bunch of SRAM0 values. 
 * Calls:
 *	fnc0x00004cd8()
 *	fnc0x000091b0(arg0x000091b0_0, arg0x000091b0_1)
 *	fnc0x00006d54()
 *	fnc0x00006d94(arg0x00006d94_0, arg0x00006d94_1)
 *
 * \return None.
 */
void fnc0x00009184();

/**
 * Initialize some SRAM0 values.
 * 
 * \param arg0x000091b0_0 Used for pathing and values to set SRAM0 values.
 * \param arg0x000091b0_1 Used for pathing and values to set SRAM0 values.
 *
 * \return None.
 */
void fnc0x000091b0(arg0x000091b0_0, arg0x000091b0_1);

/**
 * Clear some SRAM0 Values
 *
 * \return None.
 */
void fnc0x00006d54();

/**
 * Check some path options... but input arguments are constants...
 *
 * \param arg0x00006d94_0 Unknown usage. Not tested if arg0x00006d94_1 is 0xE
 * \param arg0x00006d94_1 Checked against all integer values at least up until
 *	0xE...
 * 
 * \return None.
 */
void fnc0x00006d94(arg0x00006d94_0, arg0x00006d94_1);

/**
 * Initial SRAM1 (calls fnc0x00004f30()).
 *
 * \param arg0x00004f20_0 Value written to 0x20000078. Purpose unknown.
 *
 * \return None.
 */
void fnc0x00004f20(arg0x00004f20_0);

/**
 * Initialize SRAM1 values.
 *
 * \return None.
 */
void fnc0x00004f30();

/**
 * USB configuration/initialization. Call fnc0x0000b7c8() to configure
 *  clocks, etc. Call USBD_HW_API->Init() to initialize core USB device. 
 *  And finally, setup interrupt in case USB_VBUS GPIO changes state.
 *
 * \return None.
 */
void fnc0x00003060();

/**
 * Configure USB peripheral (i.e. setup clocking, power up needed portions, 
 *  set needed GPIO pin functions).
 *
 * \param arg0x0000b7c8_0 TODO: Unknown. Branch check at beginning of function.
 *
 * \return None.
 */
void fnc0x0000b7c8(arg0x0000b7c8_0);

/** \fn ErrorCode_t Init(USBD_HANDLE_T* phUsb, USB_CORE_DESCS_T* pDesc, USBD_API_INIT_PARAM_T* param)
 *  Function to initialize USB device stack's DCD and core layers.            
 *                                                                            
 *  This function is called by application layer to initialize USB hardware and core layers. 
 *  On successful initialization the function returns a handle to USB device stack which should
 *  be passed to the rest of the functions.                                   
 *                                                                            
 *  \param[in,out] phUsb Pointer to the USB device stack handle of type USBD_HANDLE_T. 
 *  \param[in]  pDesc Structure containing pointers to various descriptor arrays needed by the stack.
 *                    These descriptors are reported to USB host as part of enumerations process.
 *  \param[in]  param Structure containing USB device stack initialization parameters.
 *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
 *          \retval LPC_OK(0) On success                                      
 *          \retval ERR_USBD_BAD_MEM_BUF(0x0004000b) When insufficient memory buffer is passed or memory
 *                                             is not aligned on 2048 boundary.
 */                                                                           
ErrorCode_t fnc0x1fff351c(arg0x1fff351c_0, arg0x1fff351c_1, arg0x1fff351c_2);

/**
 * Setup interrupt callback for specific GPIO changing state.
 *
 * \param arg0x000055e8_0 GPIO Group/Port Number of pin to be related to interrupt.
 * \param arg0x000055e8_1 GPIO Pin Number of pin to be related to interrupt.
 * \param arg0x000055e8_2 Callback function?? Related to interrupt. Will be
 *	set to 0x100010b4 (if 0x100010b4 is non-zero).
 * \param arg0x000055e8_3 Some sort of boolean? TODO: what if this is 0?
 * 
 * \return None.
 */
void fnc0x000055e8(arg0x000055e8_0, arg0x000055e8_1, arg0x000055e8_2, arg0x000055e8_3);

/**
 * Initialize USB HID EP1
 * 
 *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
 *          \retval LPC_OK On success                                         
 *          \retval ERR_USBD_BAD_MEM_BUF  Memory buffer passed is not 4-byte  
 *              aligned or smaller than required.                             
 *          \retval ERR_API_INVALID_PARAM2 Either HID_GetReport() or HID_SetReport()
 *              callback are not defined.                                     
 *          \retval ERR_USBD_BAD_DESC  HID_HID_DESCRIPTOR_TYPE is not defined 
 *              immediately after interface descriptor.                       
 *          \retval ERR_USBD_BAD_INTF_DESC  Wrong interface descriptor is passed. 
 *          \retval ERR_USBD_BAD_EP_DESC  Wrong endpoint descriptor is passed. 
 */
ErrorCode_t fnc0x00006534();

/**
 * Initialize HID driver for specified EP 
 *  (Generic wrapper for calling USBD_HID_API_T->init())
 * 
 * \param arg0x0000300c_0 USB EP Number
 * \param arg0x0000300c_1 USBD_HID_INIT_PARAM_T* param	
 *
 *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
 *          \retval LPC_OK On success                                         
 *          \retval ERR_USBD_BAD_MEM_BUF  Memory buffer passed is not 4-byte  
 *              aligned or smaller than required.                             
 *          \retval ERR_API_INVALID_PARAM2 Either HID_GetReport() or HID_SetReport()
 *              callback are not defined.                                     
 *          \retval ERR_USBD_BAD_DESC  HID_HID_DESCRIPTOR_TYPE is not defined 
 *              immediately after interface descriptor.                       
 *          \retval ERR_USBD_BAD_INTF_DESC  Wrong interface descriptor is passed. 
 *          \retval ERR_USBD_BAD_EP_DESC  Wrong endpoint descriptor is passed. 
 */
ErrorCode_t fnc0x0000300c(arg0x0000300c_0, arg0x0000300c_1);

/** \fn ErrorCode_t init(USBD_HANDLE_T hUsb, USBD_HID_INIT_PARAM_T* param)    
 *  Function to initialize HID function driver module.                        
 *                                                                            
 *  This function is called by application layer to initialize HID function driver  
 *  module. On successful initialization the function returns a handle to HID 
 *  function driver module in passed param structure.                         
 *                                                                            
 *  \param[in] hUsb Handle to the USB device stack.                           
 *  \param[in, out] param Structure containing HID function driver module     
 *      initialization parameters.                                            
 *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
 *          \retval LPC_OK On success                                         
 *          \retval ERR_USBD_BAD_MEM_BUF  Memory buffer passed is not 4-byte  
 *              aligned or smaller than required.                             
 *          \retval ERR_API_INVALID_PARAM2 Either HID_GetReport() or HID_SetReport()
 *              callback are not defined.                                     
 *          \retval ERR_USBD_BAD_DESC  HID_HID_DESCRIPTOR_TYPE is not defined 
 *              immediately after interface descriptor.                       
 *          \retval ERR_USBD_BAD_INTF_DESC  Wrong interface descriptor is passed. 
 *          \retval ERR_USBD_BAD_EP_DESC  Wrong endpoint descriptor is passed. 
 */                                                                           
ErrorCode_t fnc0x1fff2cfc(arg0x1fff2cfc_0, arg0x1fff2cfc_1);

/**
 * Initialize USB HID EP2
 * 
 *  \return Returns \ref ErrorCode_t type to indicate success or error condition.
 *          \retval LPC_OK On success                                         
 *          \retval ERR_USBD_BAD_MEM_BUF  Memory buffer passed is not 4-byte  
 *              aligned or smaller than required.                             
 *          \retval ERR_API_INVALID_PARAM2 Either HID_GetReport() or HID_SetReport()
 *              callback are not defined.                                     
 *          \retval ERR_USBD_BAD_DESC  HID_HID_DESCRIPTOR_TYPE is not defined 
 *              immediately after interface descriptor.                       
 *          \retval ERR_USBD_BAD_INTF_DESC  Wrong interface descriptor is passed. 
 *          \retval ERR_USBD_BAD_EP_DESC  Wrong endpoint descriptor is passed. 
 */
ErrorCode_t fnc0x00007268();

/**
 * Initialize USB EP3
 *
 * \param arg0x00006058_0 Value to set variable 0x10000380 to. Unknown purpose.
 *
 * \return None.
 */
void fnc0x00006058(arg0x00006058_0);

/**
 * Copy some variable values?? Called as part of USB HID3 init...
 *
 * \return None.
 */
void fnc0x00007c90();

// TODO: Lots of code in here... Overarching system/perpipherals initiliazation?
?? fnc0x00005d90( arg0x00005d90_0, arg0x00005d90_1, arg0x00005d90_2, arg0x00005d90_3, arg0x00005d90_4, arg0x00005d90_5, arg0x00005d90_6, arg0x00005d90_12, )

/**
 * Configure Brown Out Detection and Reset. That is specify voltage levels for
 *  generating interrupts and enable reset function.
 *
 * \return None.
 */
void fnc0x0000521c();

/**
 * Groups together a whole bunch of GPIO and memory initialization:
 *	PIO1_7
 *	PIO0_19
 *	PIO0_2 (PINT1)
 *	PIO1_28
 *	PIO0_7
 *  These all related?
 *
 * \return None.
 */
void fnc0x00009204();

/**
 * Write some RAM values. 
 *  Specifically 
 *	((uint8_t*)(0x10000953 + arg0x00007980_0))[0] = arg0x00007980_1
 *	((uint8_t*)(0x10000953 + arg0x00007980_0))[1] = arg0x00007980_2
 *	((uint8_t*)(0x10000924 + arg0x00007980_0))[0] = 1
 *
 * \return None.
 */
void fnc0x00007980(arg0x00007980_0, arg0x00007980_1, arg0x00007980_2);

/**
 * Setup TODO purpose GPIO (PIO1_7)
 *
 * \param arg0x000071e8_0 GPIO Bank/Group
 * \param arg0x000071e8_1 GPIO Number/Offset
 * 
 * \return None.
 */
void fnc0x000071e8(arg0x000071e8_0, arg0x000071e8_1);

/**
 * Set I/O Configuration specified GPIO (while making sure to keep reserved
 *  bits properly set).
 * 
 * \param arg0x00005508_0 GPIO Bank/Group
 * \param arg0x00005508_1 GPIO Number/Offset
 * \param arg0x00005508_2 GPIO settings (i.e. FUNC, MODE, HYS, INV, OD).
 *
 * \return None.
 */
void fnc0x00005508(arg0x00005508_0, arg0x00005508_1, arg0x00005508_2);

/**
 * Setup TODO purpose GPIO (PIO0_19)
 *
 * \param arg0x000064c4_0 GPIO Bank/Group
 * \param arg0x000064c4_1 GPIO Number/Offset
 * 
 * \return None.
 */
void fnc0x000064c4(arg0x000064c4_0, arg0x000064c4_1);

/**
 * Setup next PINT (PINT1) for TODO purpose
 *
 * \param arg0x00007c38_0 GPIO Bank/Group
 * \param arg0x00007c38_1 GPIO Number/Offset
 * \param arg0x00007c38_2 A callback function?
 * 
 * \return None.
 */
void fnc0x00007c38(arg0x00007c38_0, arg0x00007c38_1, arg0x00007c38_2);

/**
 * Setup next (i.e. next free) GPIO interrupt
 *
 * \param arg0x00005544_0 GPIO Bank/Group
 * \param arg0x00005544_1 GPIO Number/Offset
 * \param arg0x00005544_2 A callback function called by interrupt handler?
 * \param arg0x00005544_3 Boolean of unknown purpose? TODO: only simulated with value of 0...
 *
 * \return None.
 */
void fnc0x00005544(arg0x00005544_0, arg0x00005544_1, arg0x00005544_2, arg0x00005544_3);

/**
 * Initialize fields in UNKNOWN STRUCT A. (Note some values for struct init are
 *  are stack).
 *
 * \param arg0x0000bb38_0 Value for UKNOWN STRUCT A UNKNOWN FIELD A
 * \param arg0x0000bb38_1 Value for UKNOWN STRUCT A UNKNOWN FIELD C
 * \param arg0x0000bb38_2 Value for UKNOWN STRUCT A UNKNOWN FIELD B
 * \param arg0x0000bb38_3 Value for UKNOWN STRUCT A UNKNOWN FIELD E
 *
 * \return None.
 */
void fnc0x0000bb38(arg0x0000bb38_0, arg0x0000bb38_1, arg0x0000bb38_2, arg0x0000bb38_3);

/**
 * Setup a GPIO for TODO prupose. arg0x00004fdc_0 is an offset that seems to
 *  imply this function can be used to specify a  series of related or similar
 *  GPIOs. 
 *
 * \param arg0x00004fdc_0 Offset/index to define GPIO purpose?
 * \param arg0x00004fdc_1 Group/bank of GPIO to configure
 * \param arg0x00004fdc_2 Num/offset of GPIO to configure
 *
 * \return None.
 */
void fnc0x00004fdc(arg0x00004fdc_0, arg0x00004fdc_1, arg0x00004fdc_2);

/**
 * Configure GPIO1_10 (related to board powering up via battery?) and AD6
 *  (related to TODO).
 *
 * \return None.
 */
void fnc0x00007af0();

/**
 * Enable ADC conversion (and interrupts) for specified channel.
 *
 * \param arg0x00002a90_0 0-based AD channel to setup.
 * \param arg0x00002a90_1 Callback function related to conversion complete?
 *
 * \return None.
 */
void fnc0x00002a90(arg0x00002a90_0, arg0x00002a90_1);

/**
 * Check state of PIO0_3 (USB_VBUS). This causes major divergence in code 
 *  (i.e. if comms expected via USB or RF chip... I think).
 * 
 * \return 1 if USB_VBUS reads high, 0 otherwise. 
 */
int fnc0x00007b70();

/**
 * Drive TODO GPIO (GPIO1_10).
 *
 * \param arg0x00007b98_0 Enable/disable what is driven by GPIO1_10? Actual
 *  	value written to GPIO may be inverted based on settings.
 *
 * \return None.
 */
void fnc0x00007b98(arg0x00007b98_0);

/**
 * Configure GPIO1_1 and TDI/TDO mode... But why?
 *
 * \return None.
 */
void fnc0x00005b44();

/**
 * Drive GPIO1_1 and Configure TDI/TDO MODE... But why? Are TDI/TDO used for
 *  multiple functions (i.e. not just JTAG?)?
 *
 * \param arg0x00005ba0_0 Unknown. Maybe related to how to drive GPIO1_1?
 * \return None.
 */
void fnc0x00005ba0(arg0x00005ba0_0);

/**
 * Initialize Steam Controller LED control.
 *
 * \param arg0x000098b4_0 Timer/Counter number (i.e. n in CT16Bn) to be used as
 *	PWM for LED.
 * 
 * \return None.
 */
void fnc0x000098b4(arg0x000098b4_0);

/**
 * Set next function pointer in array starting at 0x10000dd8.
 *
 * \param arg0x00007bbc_0 Function pointer to save in next open array slot.
 *
 * \return None.
 */
void fnc0x00007bbc(arg0x00007bbc_0);

/**
 * Configure counter/timer to act as PWM for Steam Controller LED (i.e. reset 
 *	timer and setup appropriate MAT output pin).
 *
 * \param arg0x000098b4_0 Timer/Counter number (i.e. n in CT16Bn) to be used as
 *	PWM for LED.
 *
 * \return None. 
 */
void fnc0x00007388(arg0x00007388_0);

/**
 * Enable clock for a given timer/counter.
 *
 * \param arg0x000045bc_0 Base control register address for counter whose clock
 *	is to be enabled.
 *
 * \return None.
 */
void fnc0x000045bc(arg0x000045bc_0);

/**
 * Enable match pin function for given timer/counter.
 *
 * \param arg0x0000a010_0 Base control register for timer.
 * \param arg0x0000a010_1 Which MAT is enabled for PWM mode.
 * \param arg0x0000a010_2 Boolean flag? TODO purpose?
 *
 * \return None.
 */
void fnc0x0000a010(arg0x0000a010_0, arg0x0000a010_1, arg0x0000a010_2);

/**
 * Update some CT16B1 related variables and calcualte and set MR0 for CT16B1
 *
 * \return None.
 */
void fnc0x0000a434();

/**
 * Calculate and set MR0 for CT16B1.
 *
 * \param arg0x00007368_0 TODO Used to updates some CT16B1 related variables.
 *
 * \return None.
 */
void fnc0x00007368(arg0x00007368_0);

/**
 * Caluclate MR0 for CT16B1.
 *
 * /return Vale to set MR0 register for CT16B1.
 */
uint32_t fnc0x0000e65c();

/**
 * TODO: Some math related function?
 *
 * Called as:
 * 	part of MR0 calculated for CT16B1
 *	TODO: Add other functions
 *
 * Calls :
 *	fnc0x0000281a
 *
 * \return ?? 
 */
uint32_t fnc0x00002672(arg0x00002672_0);

/**
 * TODO: Some math related function?
 *
 * Called as:
 * 	part of MR0 calculated for CT16B1
 *	TODO: Add other functions
 *
 * \return ??
 */
uint32_t fnc0x0000281a(arg0x0000281a_0, arg0x0000281a_1, arg0x0000281a_2, arg0x0000281a_3);

/**
 * TODO: Some math/path related function?
 *
 * Called as:
 * 	part of MR0 calculated for CT16B1
 *	TODO: Add other functions
 *
 * \return ??
 */
void fnc0x00002394(arg0x00002394_0, arg0x00002394_1);

/**
 * TODO: Some math/path related function?
 *
 * Called as:
 * 	part of MR0 calculated for CT16B1
 *	TODO: Add other functions
 *
 * \return ??
 */
uint32_t fnc0x0000231c(arg0x0000231c_0, arg0x0000231c_1, arg0x0000231c_2);

/**
 * TODO: Update some variables related to CT16B1?
 *
 * Called as:
 *	Part of Steam Controller button control initialization
 * \return None.
 */
void fnc0x00007404(arg0x00007404_0, arg0x00007404_1);

/**
 * Configure CT16B0 and delay for specified amount.
 * 
 * \param arg0x0000a934_0 Defines length of delay before interrupt fires TODO:
 *	understand and have this make more sense (i.e. units on delay)?
 *
 * \return None.
 */
void fnc0x0000a934(arg0x0000a934_0);

/**
 * Set CT16B1 and wait until MR0 causes causes interrup to fire and be handled.
 * 
 * \param arg0x00005174_0 Unused (n in CT16Bn maybe, but optimzied out?)
 * \param arg0x00005174_1 Match Register 0 value.
 *
 * \return None.
 */
void fnc0x00005174(arg0x00005174_0, arg0x00005174_1);

/**
 * Configure interrupts, etc. for CT16B0.
 *
 * \return None.
 */
void fnc0x0000600c();

/**
 * Set callback function and another variable related to counter/timer?
 * 
 * \param arg0x0000a0fc_0 Base address of counter/timer to have clock enabled for.
 * \param arg0x0000a0fc_1 Offset/index or number related to counter/timer? (i.e.
 *	n in CT16Bn).
 * \param arg0x0000a0fc_2 Function pointer to be saved?
 * \param arg0x0000a0fc_3 Some other parameter to be saved?
 *
 * \return None.
 */
void fnc0x0000a0fc(arg0x0000a0fc_0, arg0x0000a0fc_1, arg0x0000a0fc_2, arg0x0000a0fc_3);

/**
 * Enable clock for a given counter/timer
 * 
 * \param arg0x00004574_0 Base address of counter/timer to have clock enabled for.
 * 
 * \return None.
 */
void fnc0x00004574(arg0x00004574_0);

/**
 * Initialize comms to trackpad/haptics and place in shutdown mode
 * 
 * \return None.
 */
void fnc0x0000a388();

/**
 * Initialize comms to specified number of haptics/trackpads and place all in shutdown mode
 */
void fnc0x000048bc(arg0x000048bc_0);

/**
 * Configure SSP0 for haptics communications and enable for polling mode 
 *  (i.e. interrupts not enabled)
 *
 * \return None.
 */
void fnc0x00008f50();

/**
 * AKA void Chip_SSP_Init(LPC_SSP_T *pSSP)??
 * Initialize the SSP
 *
 * \param arg0x000043f4_0 pSSP The base of SSP peripheral on the chip
 *
 * \return None.
 */
void fnc0x000043f4(arg0x000043f4_0);

/**
 * AKA void Chip_SSP_SetBitRate(LPC_SSP_T *pSSP, uint32_t bitRate)
 * Set the clock frequency for SSP interface
 *
 * \param arg0x0000445c_0 pSSP The base of SSP peripheral on the chip
 * \param arg0x0000445c_1 bitRate The SSP bit rate
 *
 * \return None.
 */
void fnc0x0000445c(arg0x0000445c_0, arg0x0000445c_1);

/**
 * AKA void Chip_SSP_SetClockRate(LPC_SSP_T *pSSP, uint32_t clk_rate, uint32_t prescale)
 * Set up output clocks per bit for SSP bus
 *
 * \param arg0x000044b4_0 pSSP The base of SSP peripheral on the chip
 * \param arg0x000044b4_1 clk_rate The number of prescaler-output clocks per bit on the bus, minus one
 * \param arg0x000044b4_2 prescale The factor by which the Prescaler divides the SSP peripheral clock PCLK
 *
 * \return None.
 */
void fnc0x000044b4(arg0x000044b4_0, arg0x000044b4_1, arg0x000044b4_2);

/**
 * Configure Chip Select and Data Ready pins for specified haptic.
 *
 * \param arg0x00007498_0 Specifies which haptic to configure 0 = Right,
 *	1 = Left.
 *
 * \return None.
 */
void fnc0x00007498(arg0x00007498_0);

/**
 * Write to trackpad/haptics controller (i.e. Cirque Pinnacle ASIC) register
 *
 * \param arg0x00004c7e_0 Specifies which haptic to configure 0 = Right,
 *	1 = Left.
 * \param arg0x00004c7e_1 Cirque Pinnacle ASIC register to write to.
 * \param arg0x00004c7e_2 Cirque Pinnacle ASIC register value to be written.
 *
 * \return None.
 */
void fnc0x00004c7e(arg0x00004c7e_0, arg0x00004c7e_1, arg0x00004c7e_2);

/**
 * Enable communications to given haptic (i.e. pull chip select low, disable
 *  interrupts, enable clock for SSP0 and increment 0x10000340 (i.e. mutex or
 *  counting semaphore).
 *
 * \param arg0x00009840_0 Specifies which haptic to configure 0 = Right,
 *	1 = Left.
 *
 * \return None.
 */
void fnc0x00009840(arg0x00009840_0);

/**
 * SPI blocking polling read/write. Similar to 
 *  uint32_t Chip_SSP_RWFrames_Blocking(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup),
 *  except it looks like a lot is trimmed and some instructions re-ordered (i.e. 
 *  clearing interrupt status register and checking for overrun errors.
 *
 * \param arg0x00008efc_0 Pointer to struct containing all info needed for
 * 	transaction. Similar (possibly equivalent to) Chip_SSP_DATA_SETUP_T.
 *
 * \return Number of bytes transmitted/received. 
 */
uint32_t fnc0x00008efc(arg0x00008efc_0);

/**
 * Disable communications to given haptic (i.e. pull chip select high, enable 
 *  interrupts, disable clock for SSP0 and decrement 0x10000340 (i.e. mutex or
 *  counting semaphore). Effectively inverse of fnc0x00009840().
 *
 * \param arg0x0000991c_0 Specifies which haptic to configure 0 = Right,
 *	1 = Left.
 *
 * \return None.
 */
void fnc0x0000991c(arg0x0000991c_0);
