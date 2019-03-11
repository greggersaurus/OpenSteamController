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
 * TODO: This might be where default jingles are read from flash into RAM.
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
 * \param arg0x000020ec_0 (uint32_t) Dividend
 * \param arg0x000020ec_1 (uint32_t) Divisor
 *
 * \return (uint32_t)arg0x000020ec_0 / (uint32_t)arg0x000020ec_1.
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

/**
 * Peripheral initialization (i.e. BOD, peripheral control related GPIOs, 
 *	Steam Button LED Control, CT16B0, Trackpad ASICs (via SPI), Haptics, 
 *	MPU-6500 gyo/accelerometer, ADC channel for power related stuff, 
 *	ADC channels for analog triggers (maybe?), ADC channels for analog 
 *	Joystick
 *
 * \return None.
 */
void fnc0x00005d90();

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
 *	PIO0_19 (Analog Joystick Enable)
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
 * Setup Joystick Power GPIO (PIO0_19)
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
 * Enable ADC conversion (and handling of data via interrupts) for specified 
 *  channel.
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
 * Choose between enabling JTAG or Analog triggers (as they share the same
 * 	pins).
 * 
 * \param arg0x00005ba0_0 If 0 JTAG will be enabled. If 1 JTAG will be disabled
 *	and TDO and TDI will be setup as AD0 and AD2 which measure Left and 
 *	Right analog trigger positions respectively. PIO1_1 is active low
 *	enable of triggers outputting analog voltage.
 *
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
 * Convert integer to float.
 *
 * \param arg0x00002672_0 Integer value to convert to floating point 
 *	representation.
 *
 * \return arg0x00002672_0 in float format
 */
float fnc0x00002672(arg0x00002672_0);

/**
 * Called by fnc0x00002672() to convert integer to floating point representation.
 *
 * \param arg0x0000281a_0 Integer to convert to float format.
 * \param arg0x0000281a_1 TODO Some sort of flag?
 * \param arg0x0000281a_2 TODO ??
 * \param arg0x0000281a_3 TODO ??
 *
 * \return ??
 */
float fnc0x0000281a(arg0x0000281a_0, arg0x0000281a_1, arg0x0000281a_2, arg0x0000281a_3);

/**
 * Floating point division
 *
 * \param arg0x00002394_0 Dividend
 * \param arg0x00002394_1 Divisor
 *
 * \return (float)arg0x00002394_0 / (float)arg0x00002394_1
 */
float fnc0x00002394(arg0x00002394_0, arg0x00002394_1);

/**
 * Floating point multiplication.
 *
 * \param arg0x0000231c_0 Multiplier
 * \param arg0x0000231c_1 Multiplicand
 * \param arg0x0000231c_2 Copy of Multiplier (this a compiler artifact that
 *	this looks like a third argument?
 *
 * \return arg0x0000231c_0 * arg0x0000231c_1 (see note on arg0x0000231c_2).
 */
float fnc0x0000231c(arg0x0000231c_0, arg0x0000231c_1, arg0x0000231c_2);

/**
 * TODO: Update some variables related to CT16B1?
 *
 * Called as:
 *	Part of Steam Controller button control initialization
 * \return None.
 */
void fnc0x00007404(arg0x00007404_0, arg0x00007404_1);

/**
 * Sleep fucntion. Configure CT16B0 and delay for specified amount.
 * 
 * \param arg0x0000a934_0 Defines length of delay before interrupt fires TODO:
 *	understand and have this make more sense (i.e. units on delay)?
 *
 * \return None.
 */
void fnc0x0000a934(arg0x0000a934_0);

/**
 * Set CT16B0 and wait until MR0 causes causes interrupt to fire and be handled.
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
 * Disable clock for a given counter/timer
 * 
 * \param arg0x00004574_0 Base address of counter/timer to have clock disabled for.
 * 
 * \return None.
 */
void fnc0x00004574(arg0x00004574_0);

/**
 * Initialize comms to Trackpad ASICs and place in shutdown mode
 * 
 * \return None.
 */
void fnc0x0000a388();

/**
 * Initialize comms to specified number of Trackpad ASICs and place all in 
 * shutdown mode.
 */
void fnc0x000048bc(arg0x000048bc_0);

/**
 * Configure SSP0 for Trackpad ASIC communications and enable for polling mode 
 *  (i.e. interrupts not enabled)
 *
 * \return None.
 */
void fnc0x00008f50();

/**
 * AKA void Chip_SSP_Init(LPC_SSP_T *pSSP)
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
 * Configure Chip Select and Data Ready pins for specified Trackpad ASIC.
 *
 * \param arg0x00007498_0 Specifies which Trackpad ASIC to configure 0 = Right,
 *	1 = Left.
 *
 * \return None.
 */
void fnc0x00007498(arg0x00007498_0);

/**
 * Write to Trackpad ASIC register
 *
 * \param arg0x00004c7e_0 Specifies which Trackpad ASIC to configure 0 = Right,
 *	1 = Left.
 * \param arg0x00004c7e_1 Cirque Pinnacle ASIC register to write to.
 * \param arg0x00004c7e_2 Cirque Pinnacle ASIC register value to be written.
 *
 * \return None.
 */
void fnc0x00004c7e(arg0x00004c7e_0, arg0x00004c7e_1, arg0x00004c7e_2);

/**
 * Enable communications to given Trackpad ASIC (i.e. pull chip select low, disable
 *  interrupts, enable clock for SSP0 and increment 0x10000340 (i.e. mutex or
 *  counting semaphore).
 *
 * \param arg0x00009840_0 Specifies which Trackpad ASIC to configure 0 = Right,
 *	1 = Left.
 *
 * \return None.
 */
void fnc0x00009840(arg0x00009840_0);

/**
 * SPI blocking polling write. Similar to 
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
 * Disable communications to given Trackpad ASIC (i.e. pull chip select high, enable 
 *  interrupts, disable clock for SSP0 and decrement 0x10000340 (i.e. mutex or
 *  counting semaphore). Effectively inverse of fnc0x00009840().
 *
 * \param arg0x0000991c_0 Specifies which Trackpad ASIC to configure 0 = Right,
 *	1 = Left.
 *
 * \return None.
 */
void fnc0x0000991c(arg0x0000991c_0);

/**
 * Inialize comms to and put MPU-6500 in sleep mode (accelerometer/gyro).
 *
 * \param arg0x00005fbc_0 Unknown. Checked early in  function call.
 *
 * \return 0 on success?
 */
int fnc0x00005fbc(arg0x00005fbc_0);

/**
 * Initialize comms to and put MPU-6500 in sleep mode.
 * 
 * \param arg0x00006a10_0 I2C slave address of MPU-6500.
 * 
 * \return 0 on success?
 */
int fnc0x00006a10(arg0x00006a10_0);

/**
 * Initialize I2C 0 and setup transfers struct, interrupts, etc. needed for 
 *  transfers to occur
 *
 * \param arg0x00005c8c_0 I2C Slave Address of device that will be communciated
 *	with.
 *
 * \return None.
 */
void fnc0x00005c8c(arg0x00005c8c_0);

/**
 * AKA void Chip_I2C_Init(I2C_ID_T id)
 * Initializes the LPC_I2C peripheral with specified parameter.
 *
 * \param arg0x00004268_0 id I2C peripheral ID (I2C0, I2C1 ... etc)
 *
 * \return None.
 */
void fnc0x00004268(arg0x00004268_0);

/**
 * AKA void Chip_I2C_SetClockRate(I2C_ID_T id, uint32_t clockrate)
 * Set up clock rate for LPC_I2C peripheral.
 *
 * \param arg0x00004354_0 id I2C peripheral ID (I2C0, I2C1 ... etc)
 * \param arg0x00004354_0 clockrate Target clock rate value to initialized I2C peripheral (Hz)
 *
 * \note
 * Parameter @a clockrate for I2C0 should be from 1000 up to 1000000
 * (1 KHz to 1 MHz), as I2C0 support Fast Mode Plus. If the @a clockrate
 * is more than 400 KHz (Fast Plus Mode) Board_I2C_EnableFastPlus()
 * must be called prior to calling this function.
 *
 * \return None.
 */
void fnc0x00004354(arg0x00004354_0, arg0x00004354_1);

/**
 * Write data to MPU-65000 (gyro/accelerometer)
 *
 * \param arg0x0000d6c4_0 Uknown. TODO
 * \param arg0x0000d6c4_1 MPU 6500 Register Address
 * \param arg0x0000d6c4_2 Number of data bytes in transfer sequence?
 * \param arg0x0000d6c4_3 Address of data to transfer to MPU-6500?
 *
 * \return 0 on success?
 */
int fnc0x0000d6c4(arg0x0000d6c4_0, arg0x0000d6c4_1, arg0x0000d6c4_2, arg0x0000d6c4_3);

/**
 * AKA int Chip_I2C_MasterTransfer(I2C_ID_T id, I2C_XFER_T *xfer)
 * Transmit and Receive data in master mode
 *
 * \param arg0x000042f0_0 id : I2C peripheral selected (I2C0, I2C1 etc)
 * \param arg0x000042f0_0 xfer : Pointer to a I2C_XFER_T structure see notes below
 *
 * \return
 * Any of #I2C_STATUS_T values, xfer->txSz will have number of bytes
 * not sent due to error, xfer->rxSz will have the number of bytes yet
 * to be received.
 * @note
 * The parameter @a xfer should have its member @a slaveAddr initialized
 * to the 7-Bit slave address to which the master will do the xfer, Bit0
 * to bit6 should have the address and Bit8 is ignored. During the transfer
 * no code (like event handler) must change the content of the memory
 * pointed to by @a xfer. The member of @a xfer, @a txBuff and @a txSz be
 * initialized to the memory from which the I2C must pick the data to be
 * transfered to slave and the number of bytes to send respectively, similarly
 * @a rxBuff and @a rxSz must have pointer to memroy where data received
 * from slave be stored and the number of data to get from slave respectilvely.
 */
int fnc0x000042f0(arg0x000042f0_0, arg0x000042f0_1);

/**
 * Change Steam Controller LED blink rate/intensity?
 * 
 * \param arg0x000098f0_0 Used to specify intensity? Some sort of index relating
 * 	to what value to set MR0 to for CT16B1?
 * \param arg0x000098f0_1 Some sort of a flag?
 * 
 * \return None.
 */
void fnc0x000098f0(arg0x000098f0_0, arg0x000098f0_1);

/**
 * Setup ADC to convert sample via ISR and get DR6 results related to (battery) power?
 *
 * \return None.
 */
void fnc0x00002cf0()

/**
 * Configure/setup for ISR to run and save accumulated readings for AD6 in
 *  global variable.
 *
 * \return None.
 */
void fnc0x00002d4c();

/**
 * Check values of P1_12 and (potentially) P0_18. Update variable 0x100002b3
 *  based on results. Related to battery power or cap charging or something?
 *
 * \return None.
 */
void fnc0x00003bf4();

/**
 * ADC ISR: 
 *
 * 	For DR6: The ISR seems to initially wait, and then capture and 
 *	 accumulate 8 readings of ADC channel 6 voltage to 0x10000f8c. Upon
 *	 capturing last sample ADC clock may be disabled.
 *
 *	TODO: add details on what we know this does for other channels
 *
 * \return None.
 */
void fnc0x00002b20();

/**
 * Clear all ADC Data Registers (clears status bits on registers, by reading
 *  ADC Data Registers and doing nothing with read data.)
 *
 * \return None.
 */
void fnc0x00004cbc();

/**
 * AKA void Chip_ADC_SetBurstCmd(LPC_ADC_T *pADC, FunctionalState NewState)
 *  Enable burst mode.
 * 
 * \param arg0x0000402c_0 pADC The base of ADC peripheral on the chip
 * \param arg0x0000402c_1 NewState New state, should be ENABLE or DISABLE
 * 
 * \return None.
 */
void fnc0x0000402c(arg0x0000402c_0, arg0x0000402c_1);

/**
 * Check/process results for ADC read results for active channels.
 *
 * \return None.
 */
void fnc0x00004d3c();

/**
 * Related to ADC Channel 6 voltage being measured. Check if we need to, and 
 *  if needed convert ADC channel 6 ADC counts to voltage level in mV and store
 *  to a global variable (0x100002b8).
 *
 * param arg0x00003b48_0 ADC Channel number? (Unused in current sim data)
 * param arg0x00003b48_1 Average value of ADC channel 6
 *
 * \return None.
 */
void fnc0x00003b48(arg0x00003b48_0, arg0x00003b48_1);

/**
 * Setup AD2 and AD0 for Analog trigger position tracking (AD0 = Left, 
 *	AD2 = Right).
 * 
 * \param arg0x00002f4c_0 ?? Function pointer maybe?
 *
 * \return None.
 */
void fnc0x00002f4c(arg0x00002f4c_0);

/**
 * Enable two ADC channels for Joystick X/Y position measurement funcitonality.
 * 
 * \param arg0x00006384_0 Relates to address that some value (callback?) is
 *	written to.
 * \param arg0x00006384_1 Relates to address that some value (callback?) is
 *	written to.
 *
 * \return None.
 */
void fnc0x00006384(arg0x00006384_0, arg0x00006384_1);

/**
 * Related to Analog position? Calibrating extreme values (L/R/U/D)??
 *
 * \param stack[0] Value written to (uint32_t*)arg0x0000963c_0[0]
 * \param arg0x0000963c_0 Base address of where results are to be stored.
 * \param arg0x0000963c_1 Some integer value... 
 * \param arg0x0000963c_2 Another integer value (defines extreme maybe?)
 * \param arg0x0000963c_3 Value set in results registers defined by 
 *	arg0x0000963c_0.
 *
 * \return 0 or 1 (maybe related to setting upper or lower limit maybe?)
 */
int fnc0x0000963c(arg0x0000963c_0, arg0x0000963c_1, arg0x0000963c_2, arg0x0000963c_3);

/**
 * Add args and convert result to floating point.
 *
 * \param arg0x00002662_0 Summation operand 1 (in standard binary).
 * \param arg0x00002662_1 Summation operand 2 (in standard binary).
 * 
 * \return Sum in floating point representation. 
 */
float fnc0x00002662(arg0x00002662_0, arg0x00002662_1);

/**
 * Enable/disable analog joystick power.
 * 
 * \param arg0x00006504_0 Logic high enable for analog joystick (i.e. causes 
 * 	AD1 reads to track X direction and AD3 to track Y direction).
 *
 * \return None.
 */
void fnc0x00006504(arg0x00006504_0);

/**
 * Set some variables (callback?) related to Joystick X/Y measurements? 
 *	And on next call relates to Trackpads?
 *
 * \return None.
 */
void fnc0x00004e84(arg0x00004e84_0);

/**
 * Initialize GPIOs for monitoring button states.
 * 
 * \return None.
 */
void fnc0x00007934();

/**
 * Configure TBD PIO1_24 and TBD PIO1_8. Radio/USART related?
 * 
 * \return None.
 */
void fnc0x0000ba44();

/**
 * Radio Chip Init? Setup USART and send some messages to Radio Chip.
 *  TODO: what do these messages do and are any responses expected checked?
 *
 * \return None.
 */
void fnc0x0000bd3c();

/**
 * Configure communications with Radio Chip. Setup USART and GPIO1_5 to monitor
 *  as interrupt.
 *
 * \return None.
 */
void fnc0x0000ba9c();

/**
 * Configure USART0. 
 *
 * \param arg0x0000a648_0 Baud rate? Potentially unused.
 * \param arg0x0000a648_1 RXD GPIO Bank/Group.
 * \param arg0x0000a648_2 RXD GPIO Number.
 * \param arg0x0000a648_3 TXD GPIO Bank/Group.
 * Note: TXD GPIO Number was pushed onto stack before function call.
 * 
 * \return None.
 */
void fnc0x0000a648(arg0x0000a648_0, arg0x0000a648_1, arg0x0000a648_2, arg0x0000a648_3);

/**
 * Send TODO (init? powerdown?) message [0x2, 0x53, 0x03] to Radio (via UART)??
 *
 * \return None.
 */
void fnc0x0000bcfc();

/**
 * Transmit Message to Radio (via UART). Note this literally just wraps
 *	fnc0x0000a7e0()... Not sure why...
 *
 * \param arg0x0000bad8_0 (uint8_t*) Address of byte to be written out via UART.
 * \param arg0x0000bad8_1 Buffer size (i.e. number of bytes to write)?
 *
 * \return None.
 */
void fnc0x0000bad8(arg0x0000bad8_0, arg0x0000bad8_1);

/**
 * Transmit Message to Radio (via UART).
 *
 * \param arg0x0000a7e0_0 (uint8_t*) Address of byte to be written out via UART.
 * \param arg0x0000a7e0_1 Buffer size (i.e. number of bytes to write)?
 *
 * \return None.
 */
void fnc0x0000a7e0(arg0x0000a7e0_0, arg0x0000a7e0_1);

/**
 * Transmit 0x02 to Radio Chip (via USART).
 * TODO: How does make Radio chip react? Start of message marker maybe?
 *
 * \return None.
 */
void fnc0x0000a86c();

/**
 * Transmit byte via USART. If USART TX FIFO is full, byte will be buffered in
 *  RAM FIFO.
 *
 * \param arg0x0000a77c_0 (uint8_t*) Address of byte to be written out via UART.
 * \param arg0x0000a77c_1 Some sort of flag... Always simulated with '1' so far...
 *
 * \return None.
 */
void fnc0x0000a77c(arg0x0000a77c_0, arg0x0000a77c_1);

/**
 * Transmit byte(s?) (message payload?) to Radio Chip (via USART).
 *
 * \param arg0x0000a77c_0 (uint8_t*) Address of byte to be written out via UART.
 * \param arg0x0000a77c_1 Buffer size (i.e. number of bytes to write)?
 *
 * \return None.
 */
void fnc0x0000a800(arg0x0000a800_0, arg0x0000a800_1);

/**
 * Transmit 0x03 to Radio Chip (via USART).
 * TODO: How does make Radio chip react? End of message marker maybe?
 *
 * \return None.
 */
void fnc0x0000a85c();

/**
 * Send TODO (??) message [0x2, 0x58, 0x03] to Radio (via UART)??
 *
 * \return None.
 */
void fnc0x0000c000();

/**
 * Setup SysTick to fire every millisecond
 *
 * \return None.
 */
void fnc0x00009b28();

/**
 * Setup SysTick to fire at a particular rate and enable
 *
 * \param arg0x00009a72_0 The number of system clocks for after which the 
 *	SysTick IRQ will periodically fire.
 * \param arg0x00009a72_1 Defines the largest value that arg0x00009a72_0 can be.
 *
 * \return None.
 */
void fnc0x00009a72(arg0x00009a72_0, arg0x00009a72_1);

/**
 * Setup the watchdog timer
 *
 * \param arg0x0000b9f4_0 Watchdog timeout value (in units of 12M clocks).
 *
 * \return None.
 */
void fnc0x0000b9f4(arg0x0000b9f4_0);

/**
 * Enable/connect via USB. Make sure USB is setup and then indicate that USB
 *  device must signal a connect. The pull-up resistor on USB_DP will be enabled
 *  when this bit is set and the VbusDebounced bit is one.
 *
 * \param arg0x00002fd8_0 Flag. Checked at beginning of function. Unknown if
 * 	not set.
 *
 * \return None.
 */
void fnc0x00002fd8(arg0x00002fd8_0);

/**
 * Check some (USB related?) variable. Not sure what happens if variable is
 *  not 0...
 *
 * \return None.
 */
void fnc0x00007728();

/**
 * Check some (USB related?) internal variable twice.
 *
 * \return 0 if variable was not set...
 */
int fnc0x00007734();

/**
 * Check if USB_Configure_Event() has fired.
 * 
 * \return 1 USB_Configure_Event() has not fired, 0 if it has.
 */
int fnc0x0000625c();

/**
 * PendSV main handler.
 *
 * \return None.
 */
void fnc0x00007674();

/**
 * PendSV callback (called by main handler).
 *
 * \param arg0x0000bd88_0 Unknown. Pointer to something?
 * \param arg0x0000bd88_1 Flag. If 0 do nothing.
 *
 * \return None.
 */
void fnc0x0000bd88(arg0x0000bd88_0, arg0x0000bd88_1);

/**
 * SysTick main handler.
 *
 * \return None.
 */
void fnc0x00009ad0();

/**
 * PIN_INT2/PINT2 main handler
 *
 * \return None.
 */
void fnc0x000053f8();

/**
 * Related to ADC readings and CT16B1. Maybe something to do with DR6 readings
 *	and power to board and Steam Button LED?
 *
 * \return None.
 */
void fnc0x00006e14();

/**
 * Call function to choose between enabling JTAG or Analog Triggers 
 *  (fnc0x00005ba0), then check if 0x100002b4 is 0.
 *
 * \param arg0x00005204_0 If 0 JTAG will be enabled. If 1 JTAG will be disabled
 *	and TDO and TDI will be setup as AD0 and AD2 which measure Left and 
 *	Right analog trigger positions respectively. PIO1_1 is active low
 *	enable of triggers outputting analog voltage.
 *
 * \return None.
 */
void fnc0x00005204(arg0x00005204_0)

/**
 * Function for handling AD0/AD2 read accumulations.
 * 
 * \param arg0x00002ebc_0 ADC Number. 0 for Right Trigger. 2 for Left Trigger.
 *	Others are invalid?
 * \param arg0x00002ebc_1 Accumulated ADC reading.
 *
 * \return None.
 */
void fnc0x00002ebc(arg0x00002ebc_0, arg0x00002ebc_1);

/**
 * Calculate AD0/AD2 calibration extreme(s)?
 *	This function calls fnc0x0000963c().
 *
 * \param arg0x00005fe8_0 Addresss where calculated results (calibration 
 *	results?) are to be stored.
 * \param arg0x00005fe8_1 Some sort of index/offset?
 *
 * \return None.
 */
void fnc0x00005fe8(arg0x00005fe8_0, arg0x00005fe8_1);

/**
 * Math related to struct AnalogReadingRelated.
 *  Update second float in struct AnalogReadingRelated 
 *	(i.e. AnalogReadingRelated + 0x4).
 *
 * \param arg0x00009690_0 Pointer to struct AnalogReadingRelated
 * \param arg0x00009690_1 Only simulated with value 0...
 *
 * \return A float... TODO: purpose?
 */
float fnc0x00009690(arg0x00009690_0, arg0x00009690_1);

/**
 * Floating point addition (but maybe actually subtraction since one operand
 *	or the other always seems to be negative).
 * 
 * \param arg0x00002268_0 Operand 1.
 * \param arg0x00002268_1 Operand 2.
 * 
 * \return
 */
float fnc0x00002268(arg0x00002268_0, arg0x00002268_1);

/**
 * Check field/flags (byte offset 0x18) in struct AnalogReadingRelated.
 *
 * \param arg0x0000356c_0 Pointer to struct AnalogReadingRelated. Not sure what
 *	happens if 32-bit field at byte offset 0x18 has any of bits 31:24 set.
 *
 * \return None.
 */
void fnc0x0000356c(arg0x0000356c_0);

/**
 * Check field/flags (byte offset 0x20) in struct AnalogReadingRelated.
 *
 * \param arg0x0000356c_0 Pointer to struct AnalogReadingRelated. Not sure what
 *	happens if 32-bit field at byte offset 0x20 has any of bits 31:24 set.
 *
 * \return None.
 */
void fnc0x00003500(arg0x00003500_0);

/**
 * Function for handling AD1/AD3 read accumulations (Joystick).
 *
 * \param arg0x0000627c_0 1 for AD1, 3 for AD3.
 * \param arg0x0000627c_0 Accumulated ADC reading.
 *
 * \return None.
 */
void fnc0x0000627c(arg0x0000627c_0, arg0x0000627c_1);

/**
 * Check flag 0x20000008. If non-zero then...?
 *
 * \return None.
 */
void fnc0x0000a3b4();

/**
 * Check flag 0x10000281. If non-zer then...?
 *
 * \return None.
 */
void fnc0x00006a58();

/**
 * Call fnc0x00006d64() to check 0x10001700 to 0x1000172e.
 *  If fnc0x00006d64() returns non-zero...?
 *
 * \retturn None.
 */
void fnc0x00006dd4();

/**
 * Check flags from 0x10001700 to 0x1000172e.
 *
 * \return 0 if all flags are 0.
 */
uint8_t fnc0x00006d64();

/**
 * Related to CT16B1?
 *
 * \return None.
 */
void fnc0x00009914();

/**
 * Related to CT16B1?
 *
 * \return None.
 */
void fnc0x0000743c();

/**
 * Some (floating point) compuatations related to CT16B1
 *
 * \param arg0x0000d71c_0 uint32_t Unknown purpose
 * \param arg0x0000d71c_1 uint32_t Unknown purpose
 * \param arg0x0000d71c_2 uint32_t Unknown purpose
 *
 * \return None.
 */
uint16_t fnc0x0000d71c(arg0x0000d71c_0, arg0x0000d71c_1, arg0x0000d71c_2);

/**
 * CT16B1 Related? Compare 0x1000005c >= 0x10000330 then...?
 *
 * \return 0 if 0x1000005c < 0x10000330
 */
int fnc0x000073dc();

/** AKA \fn void ISR(USBD_HANDLE_T hUsb)                                          
 *  Function to USB device controller interrupt events.                       
 *                                                                            
 *  When the user application is active the interrupt handlers are mapped in the user flash
 *  space. The user application must provide an interrupt handler for the USB interrupt and
 *  call this function in the interrupt handler routine. The driver interrupt handler takes
 *  appropriate action according to the data received on the USB bus.         
 *                                                                            
 *  \param[in]  hUsb Handle to the USB device stack.                          
 *  \return Nothing.                                                          
 */                                                                           
void fnc0x1fff3654(arg0x1fff3654_0);

/**
 * Call fnc0x00007df8 to check states of a series of buttons. EP1 related?
 *
 * \return None.
 */
void fnc0x00006584();

/**
 * Check states of a series of buttons (S19 - FRONT_R, S17 - FRONT_L, 
 *	S1 - A Button, S4 - B Button, and not-enabled buttons). EP1 related?
 *
 * \return 0 on sucecss??
 */
int fnc0x00007df8()

/**
 * Check state of a controller button (by calling fnc0x00009e08()).
 * 
 * \param arg0x00007904_0 See fnc0x00009e08() for further details.
 *
 * \return See fnc0x00009e08() for further details.
 */
int fnc0x00007904(arg0x00007904_0);

/**
 * Check state of a controller button.
 *
 * \param arg0x00009e08_0 Index of button to query where:
 *	 [0] = S6 RT Button
 *	 [1] = S3 LT Button
 *	 [2] = S9 Y Button
 *	 [3] = S4 B Button
 *	 [4] = S7 X Button
 *	 [5] = S1 A Button
 *	 [6] = S10 Right Bumper Button
 *	 [7] = S8 Left Bumper Button
 *	 [8] = S2 Left Trackpad Click Button
 *	 [9] = S19 Front Right Arrow Button
 *	[10] = S18 Steam Button
 *	[11] = S17 Front Left Arrow Button
 *	[12] = ?? Button
 *	[13] = ?? Button
 *	[14] = ?? Button
 *	[15] = ?? Button
 *	[16] = ?? Button
 *	[17] = ?? Button
 *	[18] = ?? Button
 *	[19] = ?? Button
 *	[20] = S14 Left Inner Grip Button
 *	[21] = S16 Right Inner Grip Button
 *	[22] = ?? Button
 *	[23] = ?? Button
 *	[24] = ?? Button
 *	[25] = ?? Button
 *	[26] = ?? Button
 *	[27] = ?? Button
 *	[28] = ?? Button
 *	[29] = ?? Button
 *	[30] = ?? Button
 *	[31] = ?? Button
 *	[32] = ?? Button
 *	[33] = ?? Button
 *	[34] = ?? Button
 *	[35] = ?? Button
 *	[36] = ?? Button
 *	[37] = ?? Button
 *	[38] = ?? Button
 *	[39] = ?? Button
 *	[40] = ?? Button
 *	[41] = ?? Button
 *	[42] = S? Analog Joystick Click Button
 *	[43] = S5 Right Trackpad Click Button
 *	[44] = ?? Button
 *	[45] = ?? Button
 *	[46] = ?? Button
 *
 * \return True if button is being depressed, false otherwise.
 */
int fnc0x00009e08(arg0x00009e08_0);

/**
 * Compare two uint8_t arrays (memcmp?).
 *
 * \param arg0x000021ee_0 Address of array 0.
 * \param arg0x000021ee_1 Address of array 1.
 * \param arg0x000021ee_2 Length of arrays.
 *
 * \return 0 if the arrays match.
 */
int fnc0x000021ee(arg0x000021ee_0, arg0x000021ee_1, arg0x000021ee_2);

/**
 * Call fnc0x00007f04() to check states on a variety of inputs. EP2 related?
 *
 * \return None.
 */
void fnc0x000072b8();

/**
 * Check states of inputs (S6 - RT (Right trigger digital), S3 - LT (Left trigger
 *	 digital), not enabled other (analog?)). EP2 related?
 * 
 * \return 0 on success??
 */
int fnc0x00007f04();

/**
 * Execute callback for given peripheral/behavior index.
 *
 * \param arg0x000078e6_0 Same as arg0x000078e6_1. Never actually referenced in 
 *	function.
 * \param arg0x000078e6_1 Index used to determine what callback to attempt.
 *	See callbacks array starting at 0x10000858 for further details.
 *
 * \return Return value produced by callback?
 */
uint8_t fnc0x000078e6(arg0x000078e6_0, arg0x000078e6_1);

/**
 * EP3 related checking of inputs/interfaces?? Primarily calls fnc0x0000807c().
 *
 * \return None.
 */
void fnc0x0000b84c()

/**
 *
 * Check results for a variety of inputs (EP3 related?)
 * 	S6 - RT (Right trigger digital), S3 - LT (Left trigger digital), S8 - LB (Left bumper), S10 - RB (Right bumper), 
 *	S1 - A Button, S4 - B Button, S7 - X Button, S9 - Y Button, Not enabled/configured buttons,
 *	S19 - FRONT_R (Front right arrow button), S18 - Steam button, S17 - FRONT_L (Front left arrow button), 
 *	S14 - Left inner grip button, S16 - Right inner grip button, S5 - Right trackpad click, 
 *	Joystick X (AD1), Joystick Click, S2 - Left trackpad click, Left Analog Trigger (AD0),
 *	Right Analog Trigger (AD2), 
 *
 * \return 0 on success..??
 */
int fnc0x0000807c()

/**
 * Compute values related to AD. Calls fnc0x0000b0f0().
 * 
 * \param arg0x00006308_0 See arg0x0000b0f0_0 for details.
 *
 * \return None.
 */
void fnc0x00006308(arg0x00006308_0);

/**
 * Compute values related to AD
 * 
 * \param arg0x0000b0f0_0 Using 0x10000026 as base this determine 16-bit offset
 *	of values to be used (and updated?) in computation.
 *
 * \return None.
 */
void fnc0x0000b0f0(arg0x0000b0f0_0);

/**
 * Unknown floating point math. Calls fnc0x0000288e().
 * 
 * \parma arg0x0000e8e0_0 A floating point number...
 * 
 * \return ??
 */
float fnc0x0000e8e0(arg0x0000e8e0_0);

/**
 * Unknown floating point math.
 * 
 * \parma arg0x0000288e_0 A floating point number...
 * 
 * \return ??
 */
float fnc0x0000288e(arg0x0000288e_0);

/**
 * A bunch of floating point math.
 * 
 * \param arg0x00009074_0 A floating point number...
 * \param arg0x00009074_1 A floating point number...
 *
 * \return ??
 */
float fnc0x00009074(arg0x00009074_0, arg0x00009074_1);

/**
 * Related to EP3 and 0x20000034 as base address?
 * 
 * \param arg0x0000a28c_0 Offset from 0x20000034...
 * \param arg0x0000a28c_1 Address where something is written...
 * \param arg0x0000a28c_2 Unused?
 * \param arg0x0000a28c_3 ADdress where something is written...
 *
 * \return None.
 */
void fnc0x0000a28c(arg0x0000a28c_0, arg0x0000a28c_1, arg0x0000a28c_2, arg0x0000a28c_3);

/**
 * EP3 related?
 *
 * \param arg0x0000221e_0 Address?
 * \param arg0x0000221e_1 ??
 * 
 * \return ??
 */
int fnc0x0000221e(arg0x0000221e_0, arg0x0000221e_1);

/**
 * Check 0x10000280 flag...
 * 
 * \return State of flag?
 */
int fnc0x00006880();

/**
 * Set some variables... EP3 related?
 *
 * \return None.
 */ 
void fnc0x0000b338();

/**
 * Check and set some variable. EP3 related??
 *
 * \return 0 on success??
 */
int fnc0x0000b5a0();

/**
 * Check 0x10000344 for ?? purpose...
 * 
 * \return None.
 */
void fnc0x00003138();

/**
 * Check some flags and SysTick related vairable, then wfi... then set
 *	*(uint8_t*)0x100002b5 = *(uint8_t*)0x100002b4 and exit.
 * 
 * \return None.
 */
void fnc0x00007ca0();

/**
 * Compute the different between SysTick (and maybe EP3?) related variables...
 *
 * \return ((*(uint32_t*)0x10000338) - (*(uint32_t*)0x100002c8))
 */
int fnc0x00007c7c();

/**
 * Handle USB_Configure_Event()
 *
 * \return None.
 */
void fnc0x00004e58();

/**
 * USB_Configure_Event related. Check bounds on arguments, then set some 
 * 	variables....
 *
 * \param arg0x00004f60_0 TBD use. A number?
 * \param arg0x00004f60_1 TBD use. A number?
 * 
 * \return None.
 */
void fnc0x00004f60(arg0x00004f60_0, arg0x00004f60_1);

/**
 * Check if input argument is equal to a value... Not sure what happens if it
 *	is. Do nothing return if not.
 *
 * \param arg0x000095b8_0 Number to check.
 *
 * \return None.
 */
void fnc0x000095b8(arg0x000095b8_0);

/**
 * USB_Configure_Event() related. Called via callback 0x10000348. Calls 
 *	fnc0x000076d4() with a particular value.
 *
 * \return None.
 */
void fnc0x0000a894();

/**
 * USB_Configure_Event()/USB_Suspend_Event() related. Do some maths, check 
 *	some variables and increment a counter.
 *
 * \param arg0x000076d4_0 Value to set variable. Callback address maybe?
 *
 * \return None.
 */
void fnc0x000076d4(arg0x000076d4_0);

/** AKA uint32_t WriteEP(USBD_HANDLE_T hUsb, uint32_t EPNum, uint8_t *pData, uint32_t cnt)
*  Function to write data to be sent on the requested endpoint.              
*                                                                            
*  This function is called by USB stack and the application layer to send data
*  on the requested endpoint.                                                
*                                                                            
*  \param[in] hUsb Handle to the USB device stack.                           
*  \param[in] EPNum  Endpoint number as per USB specification.               
*                    ie. An EP1_IN is represented by 0x81 number.            
*  \param[in] pData Pointer to the data buffer from where data is to be copied. 
*  \param[in] cnt  Number of bytes to write.                                 
*  \return Returns the number of bytes written.                              
*/                                                                           
uint32_t fnc0x1fff3386(arg0x1fff3386_0, arg0x1fff3386_1, arg0x1fff3386_2, arg0x1fff3386_3);

/**
 * Handle USB_Suspend_Event().
 *
 * \return None.
 */
void fnc0x0000995c();

/**
 * Handle USB_Resume_Event().
 *
 * \return None.
 */
void fnc0x00008de4();

// Function called from main loop if USB_Configure_Event has occurred? (Setup trackpad ASIC, kickoff jingle, setup gyro/accelerometer?, and more...?)
?? fnc0x0000a8ac()

/**
 * Power down watchdog.
 *
 * \return None.
 */
void fnc0x0000b9d0();

/**
 * Setup Haptics.
 * 
 * \return None.
 */
void fnc0x00006f8c();

/**
 * Drive Haptics Enable GPIO (i.e. PIO1_7).
 * 
 * \param arg0x00005c40_0 Inverse of value to drive PIO1_7.
 * 
 * \return None.
 */
void fnc0x00005c40(arg0x00005c40_0);

/**
 * Setup callback for TODO purpose
 * 
 * \return None.
 */
void fnc0x000037c0();

/**
 * Fill SRAM Jingle Data location with Jingle Data (either from EEPROM or 
 *  default data read from flash)
 * 
 * \return None.
 */
void fnc0x000038dc();

/**
 * Read Jingle Data from EEPROM.
 * 
 * \param arg0x00006604_0 Index of Jingle data...? This value is shifted left
 *	by 11 bits and used as EEPROM address to read from.
 * \param arg0x00006604_1 Number of bytes to read from EEPROM.
 * \param arg0x00006604_2 Unused...?
 * \param arg0x00006604_3 Address to write EEPROM data (i.e. possible Jingle
 *	Data) to.
 *
 * \return None.
 */
void fnc0x00006604(arg0x00006604_0, arg0x00006604_1, arg0x00006604_2, arg0x00006604_3);

/**
 * Fill SRAM Jingle Data location with default Jingle Data (i.e. that read and
 *  unpacked from flash).
 * 
 * \return None.
 */
void fnc0x000065c0();

/**
 * Initialize both trackpads (i.e. calibration?)
 *
 * \return None.
 */
void fnc0x0000a308();

/**
 * Initialize (i.e. clear) some SRAM1 variables (related to Trackpad ASIC?)
 * 
 * \param arg0x00005f90_0 Defines which set of SRAM1 variable to clear, maybe
 *	related to Right Trackpad and Left Trackpad?
 *
 * \return None.
 */
void fnc0x00005f90(arg0x00005f90_0);

/**
 * Initialize (write non-zero values) SRAM1 variables (related to Trackpad ASIC?)
 * 
 * \return None.
 */
void fnc0x00005eb8();

/**
 * Initialize Trackpad ASICs
 *
 * \param arg0x000047a4_0 The number of Trackpad ASICs to init?
 * 
 * \return None.
 */
void fnc0x000047a4(arg0x000047a4_0);

/**
 * CT16B0 sleep related (usleep or msleep type function maybe?).
 *
 * \param arg0x00009604_0 Related to time to sleep (us or ms maybe?)
 *
 * \return None.
 */
void fnc0x00009604(arg0x00009604_0);

/**
 * Setup Trackpad ASIC (i.e. Configure registers, calibration, setup ISR).
 *
 * \param arg0x000074e8_0 Defines which Trackpad to initialize. 0 = Right,
 *	1 = Left.
 *
 * \return None.
 */
void fnc0x000074e8(arg0x000074e8_0);

/**
 * Trackpad ASIC Register Read
 *
 * \param arg0x0000491c_0 Specifies which Trackpad ASIC to configure 0 = Right,
 *	1 = Left.
 * \param arg0x0000491c_1 Register address to read from in Trackpad ASIC.
 *
 * \return Content of specified Trackpad ASIC register.
 */
uint8_t fnc0x0000491c(arg0x0000491c_0, arg0x0000491c_1);

/**
 * Trackpad ASIC Extended Register Access (ERA) Write with Address Increment
 *
 * \param arg0x00004c14_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * \param arg0x00004c14_1 16-bit extended register address.
 * \param arg0x00004c14_2 Number of bytes to be sequentially written.
 * \param arg0x00004c14_3 Pointer to data to be written.
 *
 * \return 0 on succes??
 */
uint8_t fnc0x00004c14(arg0x00004c14_0, arg0x00004c14_1, arg0x00004c14_2, arg0x00004c14_3);

/**
 * Clear SW_DR and HW_DR by writing 0x00 to Status1 trackpad ASIC register
 *
 * \param arg0x00004cb0_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * 
 * \return None.
 */
void fnc0x00004cb0(arg0x00004cb0_0)

/**
 * Initialize registers (and shadow copies) for Trackpad ASIC (i.e. write ERAs,
 *	 etc.)
 * 
 * \param arg0x0000777c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 *
 * \return None.
 */
void fnc0x0000777c(arg0x0000777c_0);

/**
 * Update Trackpad ASIC AnyMeas Registers Related to ADC configuration:
 *	0x05 = AnyMeas_AccumBits_ElecFreq, 
 *	0x06 = AnyMeas_BitLength
 *	0x07 = AnyMeas_ADC_MuxControl
 *	0x08 = AnyMeas_ADC_Config2
 *	0x09 = AnyMeas_ADC_AWidth
 * 
 * \param arg0x00002be0_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * \param arg0x00002be0_1 AnyMeas Accumulation Bits (i.e. Gain). 
 *	0xC0 = lowest, 0x80 = 1.3x, 0x40 = 1.6x, 0x00 = 2x
 * \param arg0x00002be0_2 AnyMeas Toggle Frequence (ElecFreq). 
 *	0x02 = 500,000 Hz, 0x03 = 444,444 Hz, 0x04 = 400,000 Hz,
 *	0x05 = 363,636 Hz, 0x06 = 333,333 Hz, 0x07 = 307,692 Hz,
 *	0x09 = 267,000 Hz, 0x0B = 235,000 Hz
 * \param arg0x00002be0_3 AnyMeas BitLength
 * \param Stack[0] AnyMeas ADC MuxControl
 * \param Stack[1] AnyMeas ADC Config2
 * \param Stack[2] AnyMeas ADC AWidth
 *	
 * \param bunch of stuff on stack used to update other registers...
 * 
 * \return None.
 */
void fnc0x00002be0(arg0x00002be0_0, arg0x00002be0_1, arg0x00002be0_2, arg0x00002be0_3);

/**
 * Update ASIC Trackpad register (check if value differs from shadow copy and 
 *	send update via SPI if necessary)
 * 
 * \param arg0x0000c094_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * \param arg0x0000c094_1 Local special to Valve FW index of register (i.e.
 *	index relates to regiters that they care about modifying). See 
 *	details on 0x100000c4 - 0x100000d3 for indices map to Trackpad ASIC
 *	register addresses.
 * \param arg0x0000c094_2 Trackpad ASIC Register Address.
 * \param arg0x0000c094_3 Value that register should be.
 *
 * \return None.
 */
void fnc0x0000c094(arg0x0000c094_0, arg0x0000c094_1, arg0x0000c094_2, arg0x0000c094_3);

/**
 * Similar to ADC_TakeMeasurement (https://github.com/cirque-corp/Cirque_Pinnacle_1CA027/blob/master/Additional_Examples/AnyMeas_Example/AnyMeas_Example.ino)
 * 
 * \param arg0x00002d9c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * \param arg0x00002d9c_1 Toggle
 * \param arg0x00002d9c_2 Polarity
 * \param arg0x00002d9c_3 Right shift adjustment to be applied to 16-bit ADC value.
 * 
 * \return Values of Trackpad ASIC registers 0x10 and 0x11 concatenated. 
 */
int32_t fnc0x00002d9c(arg0x00002d9c_0, arg0x00002d9c_1, arg0x00002d9c_2, arg0x00002d9c_3);

/**
 * Setup PINT3/4 to gather compensation data via AnyMeas ADC reads.
 * 
 * \param arg0x0000975c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * 
 * \return None.
 */
void fnc0x0000975c(arg0x0000975c_0);

/**
 * Setup ADC reads for compensation calculations(?) (11 reads starting at ADC 
 *	Address 0x01df).
 * 
 * \param arg0x00002c4c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * 
 * \return None.
 */
void fnc0x00002c4c(arg0x00002c4c_0);

/**
 * Update Trackpad Toggle Frequency (ElecFreq) based on global variable 
 *	(i.e. 0x10000006/0x10000007).
 *
 * \param arg0x00003f3c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 *
 * \return None.
 */
void fnc0x00003f3c(arg0x00003f3c_0);

/**
 * Trackpad ASIC compensation calculation routine. Setup Trackpad ASIC in a 
 *  particular way then monitor results captured via ISR. Results may be 
 *  replaced by values from 0x600 or 0x626 in EEPROM.
 *
 * \parma arg0x00007d6c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 *
 * \return None.
 */
void fnc0x00007d6c(arg0x00007d6c_0)

/**
 * Attempt to accumulate compensation data via AnyMeas ADC reads.
 *
 * \param arg0x00006cb8_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * 
 * \return 1 on succes. 0 on failure (i.e. could not get desired data via ISR)
 */
int fnc0x00006cb8(arg0x00006cb8_0);

/**
 * Wait for a sequence of AnyMeas ADC reads to take place via PINT3/4.
 *
 * \param arg0x00002c4c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * \param arg0x0000b97c_1 Related to Number of times to wait Interrupt to fire 
 *	before declaring failure (but don't quite understand the math...).
 *
 * \return 1 on success. 0 on failure (i.e. ISR did not fire after 
 *	arg0x0000b97c_1 sleeps).
 */
int fnc0x0000b97c(arg0x0000b97c_0, arg0x0000b97c_1);

/**
 * Get value related to PINT3/4 ISR reads/accumulations/averages of Trackpad 
 *  ASIC registers 0x11 and 0x12. Seems to be value at 0x10000aa8 for Right 
 *  Trackpad and value at 0x10000ace for Left Trackpad. 
 *  and 
 * 
 * \return See description above. 
 */
int32_t fnc0x00005a24(arg0x00005a24_0);

/**
 * Read from EEPROM, related to Trackpad ASIC compensation.
 *
 * \param arg0x0000863c_0 Offset from base address where to read EEPROM 0x00 
 *	for Right Trackpad. 0x26 for Left Trackpad.
 * \param arg0x0000863c_1 Memory where read EEPROM data is to be saved.
 * \param arg0x0000863c_2 EEPROM base address where Trackpad ASIC (calibration?)
 *	data has been stored.
 *
 * \return None.
 */
void fnc0x0000863c(arg0x0000863c_0, arg0x0000863c_1, arg0x0000863c_2);

/**
 * Setup Trackpad ASIC to capture AnyMeas ADC results for normal operation?
 *
 * \param arg0x0000977c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 *
 * \return None.
 */
void fnc0x0000977c(arg0x0000977c_0);

/**
 * Update some Trackpad ASIC registers related to normal AnyMeas operations?
 *
 * \param arg0x00002d14_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 *
 * \return None.
 */
void fnc0x00002d14(arg0x00002d14_0);

/**
 * PINT3 (i.e. DR for Right Trackpad ASIC) ISR.
 * 
 * \return None.
 */
void fnc0x00005402();

/**
 * PINT3 related callback.
 * 
 * \return None.
 */
void fnc0x0000763c();

/**
 * Read Trackpad ASIC AnyMeas ADC value (registers 0x11 and 0x12), save results 
 *	(to proper index in global array), and clear Trackpad ASIC flags 
 *
 * \param arg0x0000573c_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * 
 * \return None.
 */
void fnc0x0000573c(arg0x0000573c_0);

/**
 * Called in PINT3. Exit related code and setting of variables based on
 *  conditions (i.e. ISR has run enough times that global data is valid).
 * Changes 
 *
 * \param arg0x00009798_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 * 
 * \return None.
 */
void fnc0x00009798(arg0x00009798_0);

/**
 * Part 2 of ADC reads for compensation calculations(?) (8 reads starting at ADC 
 *	Address 0x015b). Called in PINT3/4 after ADC reads setup by 
 *	fnc0x00002c4c() have completed.
 *
 * \param arg0x00002c82_0 Specifies which Trackpad ASIC to communicate with.
 *	0 = Right, 1 = Left.
 *
 * \return None.
 */
void fnc0x00002c82(arg0x00002c82_0);

/**
 * Play jingle for a particular event.
 *
 * \param arg0x00003934_0 Event index (0 = power-up, 1 = power-down, others = ??)
 *
 * \return 1 on success??
 */
int fnc0x00003934(arg0x00003934_0);

/**
 * Start next Note (i.e. repeated pulse) in sequence setup for specified haptic.
 *
 * \param arg0x000079b0_0 Indicates which haptic (0 = Right, 1 = Left)
 * 
 * \return TODO status code?
 */
int fnc0x000079b0(arg0x000079b0_0);

/**
 * Play a Note (i.e. repeated pulse) via a haptic.
 *
 * \param arg0x000037fc_0 Indicates which haptic (0 = Right, 1 = Left)
 * \param arg0x000037fc_1 byte[0] of Note to be played
 * \param arg0x000037fc_2 byte[3] and byte[2] of Note to be played
 * \param arg0x000037fc_3 byte[5] and byte[4] of Note to be played
 * 
 * \return TODO status code?
 */
int fnc0x000037fc(arg0x000037fc_0, arg0x000037fc_1, arg0x000037fc_2, arg0x000037fc_3);

/**
 * Setup variables to play Note via a Haptic.
 *
 * \param arg0x0000708c_0 Indicates which haptic (0 = Right, 1 = Left)
 * \param arg0x0000708c_1 Number of microseconds high
 * \param arg0x0000708c_2 Number of microseconds low
 * \param arg0x0000708c_3 Number of times to repeat high then low for this Note
 * 
 * \return None
 */
void fnc0x0000708c(arg0x0000708c_0, arg0x0000708c_1, arg0x0000708c_2, arg0x0000708c_3);

/**
 * Set state of Haptic GPIO. 
 *
 * \param arg0x000091e8_0 Indicates which haptic (0 = Right, 1 = Left).
 * \param arg0x000091e8_1 How to drive GPIO (0 or 1).
 *
 * \return None.
 */
void fnc0x000091e8(arg0x000091e8_0, arg0x000091e8_1);

/**
 * Function called on connect (i.e. USB connection established or Radio Chip).
 *  Stop LED from blinking and play startup Jingle.
 * 
 * \return None.
 */
void fnc0x00007a10();

// CT32B0 Interrupt Handler?
?? fnc0x00009d2c( arg0x00009d2c_0, arg0x00009d2c_1, arg0x00009d2c_2, arg0x00009d2c_3, arg0x00009d2c_4, arg0x00009d2c_5, arg0x00009d2c_6, arg0x00009d2c_12, )

// CT32B0 Interrupt Handler related (callback called by interrupt handler?)?
?? fnc0x00006130( arg0x00006130_0, arg0x00006130_1, arg0x00006130_2, arg0x00006130_3, arg0x00006130_4, arg0x00006130_5, arg0x00006130_6, arg0x00006130_12, )

// Related to right haptics interrupt handling (i.e. CT32B0 MR1)
?? fnc0x00006eb2( arg0x00006eb2_0, arg0x00006eb2_1, arg0x00006eb2_2, arg0x00006eb2_3, arg0x00006eb2_4, arg0x00006eb2_5, arg0x00006eb2_6, arg0x00006eb2_12, )

// Related to left haptics interrupt handling (i.e. CT32B0 MR2)
?? fnc0x00006ebe( arg0x00006ebe_0, arg0x00006ebe_1, arg0x00006ebe_2, arg0x00006ebe_3, arg0x00006ebe_4, arg0x00006ebe_5, arg0x00006ebe_6, arg0x00006ebe_12, )

// Generic function for toggling GPIO for haptics
?? fnc0x00006ecc( arg0x00006ecc_0, arg0x00006ecc_1, arg0x00006ecc_2, arg0x00006ecc_3, arg0x00006ecc_4, arg0x00006ecc_5, arg0x00006ecc_6, arg0x00006ecc_12, )

// Related to Haptics/CT32B0 Interrupt handler and switching/ending of pulse being sent to haptic via GPIO
?? fnc0x000079f0( arg0x000079f0_0, arg0x000079f0_1, arg0x000079f0_2, arg0x000079f0_3, arg0x000079f0_4, arg0x000079f0_5, arg0x000079f0_6, arg0x000079f0_7, arg0x000079f0_12, )

// Related to Haptics/CT32B0 Interrupt handler and switching/ending of pulse being sent to haptic via GPIO
// Called when Note is done being played. Calls functions to setup next Note in Jingle sequence to play.
?? fnc0x00003894( arg0x00003894_0, arg0x00003894_1, arg0x00003894_2, arg0x00003894_3, arg0x00003894_4, arg0x00003894_5, arg0x00003894_6, arg0x00003894_7, arg0x00003894_12, )
