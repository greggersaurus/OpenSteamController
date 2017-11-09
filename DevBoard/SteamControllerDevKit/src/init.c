/**
 * \file init.c
 * \brief This is a place to organize all initialization related functions for 
 *	the Steam Controller. 
 *  The functions found here are all based on the results of simulating the
 *	vcf_wired_controller_d0g_57bf5c10.bin firmware. 
 *  Note that init is being locked down to verify we are dealing with 
 *	Luna_mainboard V000456-00 rev3. I do not have access to other hardware
 *	versions and do not want release somethig I could not test. If you have
 *	a different version of hardware please refer to the notes below and
 *	original reverse engineering of the firmware. Contributions are always
 *	welcome.
 *
 * MIT License
 *
 * Copyright (c) 2017 Gregory Gluszek
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

#include "init.h"

/* System oscillator rate and clock rate on the CLKIN pin */                    
//TODO: is this correct?
const uint32_t OscRateIn = 12000000;                                            
const uint32_t ExtRateIn = 0;

/**
 * Function to enable power to a specified analog block.
 *
 * Firmware Offset(s): 
 *	0x000005a4 - 0x000005b8
 * 
 * \param reg0 Set bit(s) specify which blocks to power. See 3.5.41 
 *	Power configuration register in UM10462 for details.
 * 
 * \return None.
 */
void pwrAnalogBlock(uint32_t reg0){
	// Power Configuration Register
	volatile uint32_t* reg32 = (volatile uint32_t*)0x40048238;
	uint32_t val = 0;

	// Read current register value
	val = *reg32;
	// Clear reserved bit that must stay cleared
	val &= 0x000005ff;
	// Clear desired bit (clearing enables desired block(s))
	val &= ~(reg0 & 0x000005ff);
	// Reserved bits that must always be set
	val |= 0xe800;

	// Write result to register
	*reg32 = val;
}

/**
 * First stage initialization. Mostly deals with proper clocking and power
 *  configurations that need to get set ASAP.
 * 
 * \return None. Though function will lock up and not return in case of failure
 *	(as there is no way to communicate failure if this setup fails).
 */
void stage1Init(void){
	volatile uint32_t* reg32 = (volatile uint32_t)0;
	uint32_t val = 0;

//TODO: convert these all to lpc_chip_11uxx_lib calls?
	// Make sure crystal oscillator is powered                                      
	pwrAnalogBlock(0x00000020);

	// Delay required after last system control register mod?          
	for (uint32_t cnt = 0; cnt < 0x1600; cnt++);

	// Select Crystal Oscillator (SYSOSC)                                           
	reg32 = (volatile uint32_t*)0x40048040;                                                               
	*reg32 = 1;                                                                       

	// Enable system PLL clock source update                                        
	reg32 = (volatile uint32_t*)0x40048044;                                                               
	*reg32 = 0;                                                                       
	*reg32 = 1;

	// Power Configuration Register                                                 
	reg32 = (volatile uint32_t*)0x40048238;                                                               
	// Read current value                                                           
	val = *reg32;                                                                     
	// Clear reserved bit that must stay cleared                                    
	val &= 0x5ff;                                                                   
	// Make sure system PLL is powered down                                         
	val |= 0x80;                                                                    
	// Reserved bits that must always be set                                        
	val |= 0xe800;                                                                  
	*reg32 = val;                    

	// System PLL control register                                                  
	reg32 = (volatile uint32_t*)0x40048008;                                                               
	// Division ratio = 2 x 4. Feedback divider value = 3 + 1.                      
	*reg32 = 0x23;

	// Make sure system PLL is powered
	pwrAnalogBlock(0x00000080);

	// Wait until PLL is locked                                                     
	do{
		// System PLL status register                                                   
		reg32 = (volatile uint32_t*)0x4004800c;                                                               
	} while(((*reg32) & 1) == 0);

	// System clock divider register                                                
	reg32 = (volatile uint32_t*)0x40048078;                                                               
	// Set system AHB clock divider to 1.                                           
	*reg32 = 1;

	// Flash configuration register                                                 
	reg32 = (volatile uint32_t*)0x4003c010;                                                               
	val = *reg32;                                                                     
	// Bits 31:2 must be written back exactly as read                               
	val &= 0xFFFFFFC0;                                                              
	// Set flash access time to 3 system clocks (for system clock up to 50 MHz)     
	val |=  2;                                                                      
	*reg32 = val;

	// Main clock source select register                                            
	reg32 = (volatile uint32_t*)0x40048070;                                                               
	// Select PLL output                                                            
	*reg32 = 3;                                                                       

	// Main clock source update enable register                                     
	reg32 = (volatile uint32_t*)0x40048074;                                                               
	// No change                                                                    
	*reg32 = 0;                                                                       
	// Update clock source                                                          
	*reg32 = 1; 

	// USB PLL clock source select register                                         
	reg32 = (volatile uint32_t*)0x40048048;                                                               
	// Select system oscillator                                                     
	*reg32 = 1;                                                                       

	// USB PLL clock source update enable register                                  
	reg32 = (volatile uint32_t*)0x4004804c;                                                               
	// No change                                                                    
	*reg32 = 0;                                                                       
	// Update clock source.                                                         
	*reg32 = 1; 

	// USB PLL control register                                                     
	reg32 = (volatile uint32_t*)0x40048010;                                                               
	// Division ration is 2 x 4. Feedback divider value is 3 + 1.                   
	*reg32 = 0x23;

	// Set USB PLL and USB transceiver to powered                                   
	pwrAnalogBlock(0x00000500);

	// Wait for PLL locked                                                          
	do{
		// USB PLL status register                                                      
		reg32 = (volatile uint32_t*)0x40048014;                                                               
	} while (((*reg32) & 1) == 0);

	// System clock control register                                                
	reg32 = (volatile uint32_t*)0x40048080;                                                               
	val = *reg32;                                                                     
	// Enable I/O configuration block                                               
	val |= 0x10000;                                                                 
	*reg32 = val;
}

/**
 * Second stage initialization. Here we get into Steam Controller hardware 
 *  specific operations (i.e. using some battery power to keep us booted if 
 *  necessary) and setting up clocking and power for USB.
 *
 * \param hwVersion Version number read from EEPROM.
 * 
 * \return None. Though function will lock up and not return in case of failure
 *	(as there is no way to communicate failure if this setup fails).
 */
void stage2Init(uint32_t hwVersion){
	volatile uint32_t* reg32 = (volatile uint32_t)0;
	uint32_t val = 0;

	// Check board version read from EEPROM
	if (hwVersion < 8){
		// Hard lock if version is not what we have tested to. HW version
		//  changes pins, such as battery power enable.
		volatile int lock = 1;
		while (lock) {}
	}

	// Enables clock for GPIO port registers via system clock control register
	reg32 = (volatile uint32_t*)0x40048080;
	val = *reg32;
	val |= 0x40;
	*reg32 = val;

	// Check state of PIO0_3 (USB voltage detected) 
	uint8_t usb_volt_detect = *((uint8_t*)0x50000003);

	if (!usb_volt_detect) {
		// Check for brown out detect
		reg32 = (volatile uint32_t*)0x40048030;
		if (*reg32 & 0x8) {
			// Clear BOD
			*reg32 = 0x8;
			// Set PIO1_10 (~Battery Power Enable?) output bit
			*((uint8_t*)0x5000002a) = 1;
		} else {
			// Set PIO1_10 (~Battery Power Enable?) output bit
			*((uint8_t*)0x5000002a) = 0;
		}
	} else {
		// Set PIO1_10 to output bit
		*((uint8_t*)0x5000002a) = 0;
	}

	// Set PIO1_10 (~Battery Power Enable?) to output via GPIO direction port 1 register
	reg32 = (volatile uint32_t*)0x50002004;
	val = *reg32;
	val |= 0x00000400;
	*reg32 = val;

	// Set GPREG1 to 0
	reg32 = (volatile uint32_t*)0x40038008;
	*reg32 = 0;

	// Enable pull down resistor on PIO0_3 register
	*(uint32_t*)0x4004400c = 0x00000008;

	// Set PIO0_6 to function as ~USB_CONNECT
	*(uint32_t*)0x40044018 = 0x00000001;

	// Set PIO1_17 to function as RXD - Receiver input for USART
	*(uint32_t*)0x400440a4 = 0x00000002;

	// Set PIO1_18 to function as TXD - Transmitter output for USART
	*(uint32_t*)0x400440a8 = 0x00000002;

	// Check state of PIO0_3 (USB voltage detected) 
	usb_volt_detect = *((uint8_t*)0x50000003);

	// Set PIO1_10 (~Battery Power Enable?) output bit
	if (!usb_volt_detect){
		// Must keep using battery power if there is no USB power
		*((uint8_t*)0x5000002a) = 0;
	} else {
		// Disable battery power if USB voltage is present
		*((uint8_t*)0x5000002a) = 1;
	}

	// Set PIO1_10 (~Battery Power Enable?) to output via GPIO direction port 1 register
	reg32 = (volatile uint32_t*)0x50002004;
	val = *reg32;
	val |= 0x00000400;
	*reg32 = val;

	// Select USB PLL out via USB clock source select register
	reg32 = (volatile uint32_t*)0x400480c0;
	*reg32 = 0;
	// Clear USB clock source update enable register
	reg32 = (volatile uint32_t*)0x400480c4;
	*reg32 = 0;
	// Update clock source via USB clock source update enable register
	reg32 = (volatile uint32_t*)0x400480c4;
	*reg32 = 1;
	// Set divide by 1 for USB clock divider register
	reg32 = (volatile uint32_t*)0x400480c8;
	*reg32 = 1;

	// Enables clock to the USB register interface via System clock control register
	reg32 = (volatile uint32_t*)0x40048080;
	val = *reg32;
	val |= 0x00004000;
	*reg32 = val;

	// Enables USB SRAM block at address 0x20004000 via System clock control register 
	reg32 = (volatile uint32_t*)0x40048080;
	val = *reg32;
	val |= 0x08000000;
	*reg32 = val;
}
