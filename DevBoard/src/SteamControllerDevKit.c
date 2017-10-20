/*
===============================================================================
 Name        : SteamControllerDevKit.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif

#include <cr_section_macros.h>

#include <stdint.h>

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
void pwrAnalogBlock(uint32_t reg0) {
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

// !!!!!! IMPORTANT NOTE !!!!!
//	Currently this will only work with a particular rev of hw (tested on Luna_mainboard V000456-00 rev3)
//	Also, this (may) only work when powering up by Steam Controller Button press when USB cable is not connected
//	TODO: add in paths for handling case where USB cable is connected
//	TODO: add in paths for handling different hw versions?

int main(void) {
	volatile uint32_t* reg32 = (volatile uint32_t)0;
	uint32_t val = 0;

	// Make sure crystal oscillator is powered                                      
	pwrAnalogBlock(0x00000020);

	// Some sort of delay required after last system control register mod?          
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

	// Enables clock for GPIO port registers via system clock control register
	reg32 = (volatile uint32_t*)0x40048080;
	val = *reg32;
	val |= 0x40;
	*reg32 = val;

	// Check for brown out detect
	reg32 = (volatile uint32_t*)0x40048030;
	if (*reg32 & 0x8) {
		// Clear BOD
		*reg32 = 0x8;
		// Set PIO1_10 to output bit
		*((uint8_t*)0x5000002a) = 1;
	} else {
		// Set PIO1_10 to output bit
		*((uint8_t*)0x5000002a) = 0;
	}

	// Set PIO1_10 to output via GPIO direction port 1 register
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

	// Set PIO1_10 output bit
	*((uint8_t*)0x5000002a) = 0;

	// Set PIO1_10 to output via GPIO direction port 1 register
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

	// Enables USB SRAM block at address 0x2000 4000 via System clock control register 
	reg32 = (volatile uint32_t*)0x40048080;
	val = *reg32;
	val |= 0x08000000;
	*reg32 = val;

//	USBD_HW_API->Init(USBD_HANDLE_T* phUsb = 0x1000022c, USB_CORE_DESCS_T* pDesc = 0x10001be4, USBD_API_INIT_PARAM_T* param = 0x10001ba0)

	// PIO0_21 is connected to button LED

	// Set PIO0_21 output bit
	*((uint8_t*)0x50000015) = 0;

	// Set PIO0_21 to output via GPIO direction port 1 register
	reg32 = (volatile uint32_t*)0x50002000;
	val = *reg32;
	val |= 0x00200000;
	*reg32 = val;

	// Force the counter to be placed into memory
	volatile static int i = 0 ;
	// Enter an infinite loop, just incrementing a counter
	while(1) {
		i++ ;
		// When Y button is pressed Steam Button LED will light
		*((uint8_t*)0x50000015) = !(*((uint8_t*)0x5000002B));
	}
	return 0 ;
}
