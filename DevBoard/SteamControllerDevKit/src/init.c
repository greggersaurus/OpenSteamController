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

#include "chip.h"
#include "gpio_11xx_1.h"

#include "console.h"
#include "led_ctrl.h"
#include "adc_read.h"

/* System oscillator rate and clock rate on the CLKIN pin */                    
//TODO: is this correct?
const uint32_t OscRateIn = 12000000;                                            
const uint32_t ExtRateIn = 0;

/**
 * First stage initialization. Mostly deals with proper clocking and power
 *  configurations that need to get set ASAP.
 * 
 * \return None. Though function will lock up and not return in case of failure
 *	(as there is no way to communicate failure if this setup fails).
 */
void stage1Init(void){
	// Make sure crystal oscillator is powered
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_SYSOSC_PD);

	// Delay required after last system control register mod?          
	for (uint32_t cnt = 0; cnt < 0x1600; cnt++);

	// Select Crystal Oscillator (SYSOSC)
	Chip_Clock_SetSystemPLLSource(SYSCTL_PLLCLKSRC_MAINOSC);

	// Power down PLL to change the PLL divider ratio
	Chip_SYSCTL_PowerDown(SYSCTL_POWERDOWN_SYSPLL_PD);

	// Setup System PLL
	// Division ration is 2 x 4. Feedback divider value is 3 + 1.
	Chip_Clock_SetupSystemPLL(3, 1);

	// Powerup system PLL
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_SYSPLL_PD);

	// Wait until PLL is locked
	while (!Chip_Clock_IsSystemPLLLocked()) {}

	// Set system AHB clock divider to 1.                                           
	Chip_Clock_SetSysClockDiv(1);

	// Set flash access time to 3 system clocks (for system clock up to 50 MHz)     
	Chip_FMC_SetFLASHAccess(FLASHTIM_50MHZ_CPU);

	// Select PLL output
	Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_PLLOUT);

	// USB PLL clock source select system oscillator
	Chip_Clock_SetUSBPLLSource(SYSCTL_PLLCLKSRC_MAINOSC);

	// Setup USB PLL
	// Division ration is 2 x 4. Feedback divider value is 3 + 1.
	Chip_Clock_SetupUSBPLL(3, 1);


	// Set USB PLL and USB transceiver to powered
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_USBPLL_PD | SYSCTL_POWERDOWN_USBPAD_PD);

	// Wait for PLL to lock
	while (!Chip_Clock_IsUSBPLLLocked()) {}

	// Enable IOCON clock
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);
}

// Local variables to values of GPIOs, etc. early in boot process
static uint8_t pio0_3_start_val = 0;
static uint16_t ad6_start_val = 0;
static uint8_t pio0_2_start_val = 0;
static uint8_t pio1_12_start_val = 0;
static uint8_t pio0_18_start_val = 0;

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

	pio0_3_start_val = usb_volt_detect;

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

	// Enables SRAM1 block at address 0x2000 0000
	reg32 = (volatile uint32_t*)0x40048080;
	val = *reg32;
	val |= 0x04000000;
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

	// Drive some GPIOs according to simulation results (not sure why exactly...)
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 7, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 7, true);
	Chip_IOCON_PinMux(LPC_IOCON, 1, 7, IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 19, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 19, true);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	pio0_2_start_val = Chip_GPIO_GetPinState(LPC_GPIO, 0, 2);

	Chip_IOCON_PinMux(LPC_IOCON, 1, 28, IOCON_MODE_PULLDOWN, IOCON_FUNC0);
	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 28, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 28, true);

	Chip_IOCON_PinMux(LPC_IOCON, 0, 7, IOCON_MODE_PULLDOWN, IOCON_FUNC0);
	Chip_GPIO_WritePortBit(LPC_GPIO, 0, 7, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 7, true);

	initAdc();
	//ad6_start_val = adcReadChan(6);

	Chip_GPIO_WritePortBit(LPC_GPIO, 1, 1, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 1, true);
	Chip_IOCON_PinMux(LPC_IOCON, 1, 1, IOCON_MODE_PULLDOWN, IOCON_FUNC0);

	// Call initialization routines for specific peripherals, etc.
	initLedCtrl();

	pio1_12_start_val = Chip_GPIO_GetPinState(LPC_GPIO, 1, 12);
	pio0_18_start_val = Chip_GPIO_GetPinState(LPC_GPIO, 0, 18);
}


/**
 * Prints out statistics captured during init. Useful for reverse engineering
 *  to know the state of different inputs, etc. during different stages of boot.
 *
 * \return 0 on success.
 */
int initStatsCmdFnc(int argc, const char* argv[]) { 
	consolePrint("PIO0_3 was %d on startup. Is %d now.\n", 
		pio0_3_start_val, Chip_GPIO_GetPinState(LPC_GPIO, 0, 3));
	consolePrint("AD6 was %d on startup. Is %d now.\n", 
		ad6_start_val, adcReadChan(6));
	consolePrint("PIO0_2 was %d on startup. Is %d now.\n", 
		pio0_2_start_val, Chip_GPIO_GetPinState(LPC_GPIO, 0, 2));
	consolePrint("PIO1_12 was %d on startup. Is %d now.\n", 
		pio1_12_start_val, Chip_GPIO_GetPinState(LPC_GPIO, 1, 12));
	consolePrint("PIO0_18 was %d on startup. Is %d now.\n", 
		pio0_18_start_val, Chip_GPIO_GetPinState(LPC_GPIO, 0, 18));

	return 0;

}
