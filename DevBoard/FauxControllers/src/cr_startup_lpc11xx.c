//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Copyright (c) 2011-12 Code Red Technologies Ltd.
//
// Microcontroller Startup code for use with Red Suite
//
// Version : 120126
//
// Software License Agreement
// 
// The software is owned by Code Red Technologies and/or its suppliers, and is 
// protected under applicable copyright laws.  All rights are reserved.  Any 
// use in violation of the foregoing restrictions may subject the user to criminal 
// sanctions under applicable laws, as well as to civil liability for the breach 
// of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD. 
//
//*****************************************************************************
#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" {
	extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))
/* Include sys_config.h to get the CHIP_11* device identifier */
#include "sys_config.h"

// Code Red - if CMSIS is being used, then SystemInit() routine
// will be called by startup code rather than in application's main()
extern void SystemInit(void);

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
//
// Forward declaration of the default handlers. These are aliased.
// When the application defines a handler (with the same name), this will 
// automatically take precedence over these weak definitions
//
//*****************************************************************************
void ResetISR(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//
//*****************************************************************************

#if defined(CHIP_LPC110X)
void TIMER16_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler (void) ALIAS(IntDefaultHandler);
void UART_IRQHandler (void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler (void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler (void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void WAKEUP_IRQHandler  (void) ALIAS(IntDefaultHandler);

#elif defined(CHIP_LPC11XXLV)
void I2C_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler (void) ALIAS(IntDefaultHandler);
void UART_IRQHandler (void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler (void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler (void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler (void) ALIAS(IntDefaultHandler);
void FMC_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT3_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT2_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void WAKEUP_IRQHandler  (void) ALIAS(IntDefaultHandler);

#elif defined(CHIP_LPC11AXX)
void PIN_INT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIN_INT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIN_INT2_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIN_INT3_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIN_INT4_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIN_INT5_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIN_INT6_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIN_INT7_IRQHandler (void) ALIAS(IntDefaultHandler);
void GINT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void GINT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void ACMP_IRQHandler (void) ALIAS(IntDefaultHandler);
void DAC_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP1_IRQHandler (void) ALIAS(IntDefaultHandler);
void I2C_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler (void) ALIAS(IntDefaultHandler);
void USART_IRQHandler (void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler (void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler (void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler (void) ALIAS(IntDefaultHandler);
void FMC_IRQHandler (void) ALIAS(IntDefaultHandler);

#elif defined(CHIP_LPC11CXX)
void CAN_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP1_IRQHandler (void) ALIAS(IntDefaultHandler);
void I2C_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler (void) ALIAS(IntDefaultHandler);
void UART_IRQHandler (void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler (void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler (void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT3_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT2_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void WAKEUP_IRQHandler  (void) ALIAS(IntDefaultHandler);

#elif defined(CHIP_LPC1125)
void ADCB_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP1_IRQHandler (void) ALIAS(IntDefaultHandler);
void I2C_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler (void) ALIAS(IntDefaultHandler);
void UART_IRQHandler (void) ALIAS(IntDefaultHandler);
void UART1_IRQHandler (void) ALIAS(IntDefaultHandler);
void UART2_IRQHandler (void) ALIAS(IntDefaultHandler);
void ADCA_IRQHandler (void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler (void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT3_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT2_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void PIOINT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void WAKEUP_IRQHandler  (void) ALIAS(IntDefaultHandler);

#elif defined(CHIP_LPC11EXX)
void FLEX_INT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT2_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT3_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT4_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT5_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT6_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT7_IRQHandler (void) ALIAS(IntDefaultHandler);
void GINT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void GINT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP1_IRQHandler (void) ALIAS(IntDefaultHandler);
void I2C_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler (void) ALIAS(IntDefaultHandler);
void USART_IRQHandler (void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler (void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler (void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler (void) ALIAS(IntDefaultHandler);
void FMC_IRQHandler (void) ALIAS(IntDefaultHandler);
void IOH_IRQHandler (void) ALIAS(IntDefaultHandler);

#elif defined(CHIP_LPC11UXX)
void FLEX_INT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT2_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT3_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT4_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT5_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT6_IRQHandler (void) ALIAS(IntDefaultHandler);
void FLEX_INT7_IRQHandler (void) ALIAS(IntDefaultHandler);
void GINT0_IRQHandler (void) ALIAS(IntDefaultHandler);
void GINT1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP1_IRQHandler (void) ALIAS(IntDefaultHandler);
void I2C_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler (void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler (void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler (void) ALIAS(IntDefaultHandler);
void USART_IRQHandler (void) ALIAS(IntDefaultHandler);
void USB_IRQHandler (void) ALIAS(IntDefaultHandler);
void USB_FIQHandler (void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler (void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler (void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler (void) ALIAS(IntDefaultHandler);
void FMC_IRQHandler (void) ALIAS(IntDefaultHandler);
void USBWakeup_IRQHandler (void) ALIAS(IntDefaultHandler);
void IOH_IRQHandler (void) ALIAS(IntDefaultHandler);

#else
#error No CHIP_11* device defined  
#endif

//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for redlib based applications
// main() is the entry point for newlib based applications
//
//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//
//*****************************************************************************
#if defined (__REDLIB__)
extern void __main(void);
#endif
extern int main(void);
//*****************************************************************************
//
// External declaration for the pointer to the stack top from the Linker Script
//
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    &_vStackTop,		      // The initial stack pointer
    ResetISR,                         // The reset handler
    NMI_Handler,                      // The NMI handler
    HardFault_Handler,                // The hard fault handler
    0,                                // Reserved
    0,                      	      // Reserved
    0,                                // Reserved
    0,                                // Reserved
    0,                                // Reserved
    0,                                // Reserved
    0,                                // Reserved
    SVC_Handler,                   // SVCall handler
    0,                                // Reserved
    0,                                // Reserved
    PendSV_Handler,                   // The PendSV handler
    SysTick_Handler,                  // The SysTick handler

#if defined(CHIP_LPC110X)
    // Wakeup sources for the I/O pins:
    //   PIO0 (0:11)
    //   PIO1 (0)
    WAKEUP_IRQHandler,                      // PIO0_0  Wakeup
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    WAKEUP_IRQHandler,                      // PIO0_8  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_9  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_10 Wakeup
    WAKEUP_IRQHandler,                      // PIO0_11 Wakeup
    WAKEUP_IRQHandler,                      // PIO1_0  Wakeup
    
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    TIMER16_0_IRQHandler,                   // CT16B0 (16-bit Timer 0)
    TIMER16_1_IRQHandler,                   // CT16B1 (16-bit Timer 1)
    TIMER32_0_IRQHandler,                   // CT32B0 (32-bit Timer 0)
    TIMER32_1_IRQHandler,                   // CT32B1 (32-bit Timer 1)
    SSP0_IRQHandler,                      	// SPI/SSP0 Interrupt
    UART_IRQHandler,                      	// UART0

    0, 				                     	// Reserved
    0,                      				// Reserved

    ADC_IRQHandler,                      	// ADC   (A/D Converter)
    WDT_IRQHandler,                      	// WDT   (Watchdog Timer)
    BOD_IRQHandler,                      	// BOD   (Brownout Detect)
    0,                      				// Reserved
    0, 				                     	// Reserved
    0, 				                     	// Reserved
    PIOINT1_IRQHandler,                     // PIO INT1
    PIOINT0_IRQHandler,                     // PIO INT0

#elif defined(CHIP_LPC11XXLV)
    // Wakeup sources for the I/O pins:
    //   PIO0 (0:11)
    //   PIO1 (0)
    WAKEUP_IRQHandler,                      // PIO0_0  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_1  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_2  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_3  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_4  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_5  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_6  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_7  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_8  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_9  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_10 Wakeup
    WAKEUP_IRQHandler,                      // PIO0_11 Wakeup
    WAKEUP_IRQHandler,                      // PIO1_0  Wakeup
    
    0, 				                     	// Reserved
    0,                      				// Reserved

    I2C_IRQHandler,                      	// I2C0
    TIMER16_0_IRQHandler,                   // CT16B0 (16-bit Timer 0)
    TIMER16_1_IRQHandler,                   // CT16B1 (16-bit Timer 1)
    TIMER32_0_IRQHandler,                   // CT32B0 (32-bit Timer 0)
    TIMER32_1_IRQHandler,                   // CT32B1 (32-bit Timer 1)
    SSP0_IRQHandler,                      	// SPI/SSP0 Interrupt
    UART_IRQHandler,                      	// UART0

    0, 				                     	// Reserved
    0,                      				// Reserved

    ADC_IRQHandler,                      	// ADC   (A/D Converter)
    WDT_IRQHandler,                      	// WDT   (Watchdog Timer)
    BOD_IRQHandler,                      	// BOD   (Brownout Detect)
	FMC_IRQHandler,                         // 16+27: IP2111 Flash Memory Controller
    PIOINT3_IRQHandler,                     // PIO INT3
    PIOINT2_IRQHandler,                     // PIO INT2
    PIOINT1_IRQHandler,                     // PIO INT1
    PIOINT0_IRQHandler,                     // PIO INT0

#elif defined(CHIP_LPC11AXX)
    // LPC11A specific handlers
	PIN_INT0_IRQHandler,          // 16+ 0: Pin interrupt
	PIN_INT1_IRQHandler,          // 16+ 1: Pin interrupt
	PIN_INT2_IRQHandler,          // 16+ 2: Pin interrupt
	PIN_INT3_IRQHandler,          // 16+ 3: Pin interrupt
	PIN_INT4_IRQHandler,          // 16+ 4: Pin interrupt
	PIN_INT5_IRQHandler,          // 16+ 5: Pin interrupt
	PIN_INT6_IRQHandler,          // 16+ 6: Pin interrupt
	PIN_INT7_IRQHandler,          // 16+ 7: Pin interrupt
	GINT0_IRQHandler,             // 16+ 8: Port interrupt
	GINT1_IRQHandler,             // 16+ 9: Port interrupt
	ACMP_IRQHandler,              // 16+10: Analog Comparator
	DAC_IRQHandler,               // 16+11: D/A Converter
	0,                            // 16+12: Reserved
	0,                            // 16+13: Reserved
	SSP1_IRQHandler,              // 16+14: SSP1
	I2C_IRQHandler,               // 16+15: I2C
	TIMER16_0_IRQHandler,         // 16+16: 16-bit Timer0
	TIMER16_1_IRQHandler,         // 16+17: 16-bit Timer1
	TIMER32_0_IRQHandler,         // 16+18: 32-bit Timer0
	TIMER32_1_IRQHandler,         // 16+19: 32-bit Timer1
	SSP0_IRQHandler,              // 16+20: SSP0
	USART_IRQHandler,             // 16+21: USART
	0,                            // 16+22: Reserved
	0,                            // 16+23: Reserved
	ADC_IRQHandler,               // 16+24: A/D Converter
	WDT_IRQHandler,               // 16+25: Watchdog Timer
	BOD_IRQHandler,               // 16+26: Brown Out Detect
	FMC_IRQHandler,               // 16+27: IP2111 Flash Memory Controller
	0,                            // 16+28: Reserved
	0,                            // 16+29: ReserveD
	0,                            // 16+30: Reserved
	0,                            // 16+31: Reserved

#elif defined(CHIP_LPC11CXX)
    // Wakeup sources for the I/O pins:
    //   PIO0 (0:11)
    //   PIO1 (0)
    WAKEUP_IRQHandler,                      // PIO0_0  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_1  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_2  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_3  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_4  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_5  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_6  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_7  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_8  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_9  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_10 Wakeup
    WAKEUP_IRQHandler,                      // PIO0_11 Wakeup
    WAKEUP_IRQHandler,                      // PIO1_0  Wakeup
    
    CAN_IRQHandler,							// C_CAN Interrupt
    SSP1_IRQHandler, 						// SPI/SSP1 Interrupt
    I2C_IRQHandler,                      	// I2C0
    TIMER16_0_IRQHandler,                   // CT16B0 (16-bit Timer 0)
    TIMER16_1_IRQHandler,                   // CT16B1 (16-bit Timer 1)
    TIMER32_0_IRQHandler,                   // CT32B0 (32-bit Timer 0)
    TIMER32_1_IRQHandler,                   // CT32B1 (32-bit Timer 1)
    SSP0_IRQHandler,                      	// SPI/SSP0 Interrupt
    UART_IRQHandler,                      	// UART0

    0, 				                     	// Reserved
    0,                      				// Reserved

    ADC_IRQHandler,                      	// ADC   (A/D Converter)
    WDT_IRQHandler,                      	// WDT   (Watchdog Timer)
    BOD_IRQHandler,                      	// BOD   (Brownout Detect)
    0,                      				// Reserved
    PIOINT3_IRQHandler,                     // PIO INT3
    PIOINT2_IRQHandler,                     // PIO INT2
    PIOINT1_IRQHandler,                     // PIO INT1
    PIOINT0_IRQHandler,                     // PIO INT0

#elif defined(CHIP_LPC1125)
    // Wakeup sources for the I/O pins:
    //   PIO0 (0:11)
    //   PIO1 (0)
    WAKEUP_IRQHandler,                      // PIO0_0  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_1  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_2  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_3  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_4  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_5  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_6  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_7  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_8  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_9  Wakeup
    WAKEUP_IRQHandler,                      // PIO0_10 Wakeup
    WAKEUP_IRQHandler,                      // PIO0_11 Wakeup
    WAKEUP_IRQHandler,                      // PIO1_0  Wakeup
    
    ADCB_IRQHandler,						// ADC Sequence B Interrupt
    SSP1_IRQHandler, 						// SPI/SSP1 Interrupt
    I2C_IRQHandler,                      	// I2C0
    TIMER16_0_IRQHandler,                   // CT16B0 (16-bit Timer 0)
    TIMER16_1_IRQHandler,                   // CT16B1 (16-bit Timer 1)
    TIMER32_0_IRQHandler,                   // CT32B0 (32-bit Timer 0)
    TIMER32_1_IRQHandler,                   // CT32B1 (32-bit Timer 1)
    SSP0_IRQHandler,                      	// SPI/SSP0 Interrupt
    UART_IRQHandler,                      	// UART0
    UART1_IRQHandler,                     	// UART1
    UART2_IRQHandler,                      	// UART2
    ADCA_IRQHandler,                      	// ADC Sequence A (A/D Converter)
    WDT_IRQHandler,                      	// WDT   (Watchdog Timer)
    BOD_IRQHandler,                      	// BOD   (Brownout Detect)
    0,                      				// Reserved
    PIOINT3_IRQHandler,                     // PIO INT3
    PIOINT2_IRQHandler,                     // PIO INT2
    PIOINT1_IRQHandler,                     // PIO INT1
    PIOINT0_IRQHandler,                     // PIO INT0

#elif defined(CHIP_LPC11EXX)
    // LPC11E specific handlers
    FLEX_INT0_IRQHandler,             //  0 - GPIO pin interrupt 0
    FLEX_INT1_IRQHandler,             //  1 - GPIO pin interrupt 1
    FLEX_INT2_IRQHandler,             //  2 - GPIO pin interrupt 2
    FLEX_INT3_IRQHandler,             //  3 - GPIO pin interrupt 3
    FLEX_INT4_IRQHandler,             //  4 - GPIO pin interrupt 4
    FLEX_INT5_IRQHandler,             //  5 - GPIO pin interrupt 5
    FLEX_INT6_IRQHandler,             //  6 - GPIO pin interrupt 6
    FLEX_INT7_IRQHandler,             //  7 - GPIO pin interrupt 7
    GINT0_IRQHandler,                 //  8 - GPIO GROUP0 interrupt
    GINT1_IRQHandler,                 //  9 - GPIO GROUP1 interrupt
    0,                                // 10 - Reserved
    0,                                // 11 - Reserved
    0,                                // 12 - Reserved
    0,                                // 13 - Reserved
    SSP1_IRQHandler,                  // 14 - SPI/SSP1 Interrupt
    I2C_IRQHandler,                   // 15 - I2C0
    TIMER16_0_IRQHandler,             // 16 - CT16B0 (16-bit Timer 0)
    TIMER16_1_IRQHandler,             // 17 - CT16B1 (16-bit Timer 1)
    TIMER32_0_IRQHandler,             // 18 - CT32B0 (32-bit Timer 0)
    TIMER32_1_IRQHandler,             // 19 - CT32B1 (32-bit Timer 1)
    SSP0_IRQHandler,                  // 20 - SPI/SSP0 Interrupt
    USART_IRQHandler,                 // 21 - USART0
    0,                   			  // 22 - Reserved
    0,                   			  // 23 - Reserved
    ADC_IRQHandler,                   // 24 - ADC (A/D Converter)
    WDT_IRQHandler,                   // 25 - WDT (Watchdog Timer)
    BOD_IRQHandler,                   // 26 - BOD (Brownout Detect)
    FMC_IRQHandler,                   // 27 - IP2111 Flash Memory Controller
    0,                                // 28 - Reserved
    0,                                // 29 - Reserved
    0,             					  // 30 - Reserved
    IOH_IRQHandler,                   // 31 - I/OH Interrupt

#elif defined(CHIP_LPC11UXX)
    // LPC11U specific handlers
    FLEX_INT0_IRQHandler,             //  0 - GPIO pin interrupt 0
    FLEX_INT1_IRQHandler,             //  1 - GPIO pin interrupt 1
    FLEX_INT2_IRQHandler,             //  2 - GPIO pin interrupt 2
    FLEX_INT3_IRQHandler,             //  3 - GPIO pin interrupt 3
    FLEX_INT4_IRQHandler,             //  4 - GPIO pin interrupt 4
    FLEX_INT5_IRQHandler,             //  5 - GPIO pin interrupt 5
    FLEX_INT6_IRQHandler,             //  6 - GPIO pin interrupt 6
    FLEX_INT7_IRQHandler,             //  7 - GPIO pin interrupt 7
    GINT0_IRQHandler,                 //  8 - GPIO GROUP0 interrupt
    GINT1_IRQHandler,                 //  9 - GPIO GROUP1 interrupt
    0,                                // 10 - Reserved
    0,                                // 11 - Reserved
    0,                                // 12 - Reserved
    0,                                // 13 - Reserved
    SSP1_IRQHandler,                  // 14 - SPI/SSP1 Interrupt
    I2C_IRQHandler,                   // 15 - I2C0
    TIMER16_0_IRQHandler,             // 16 - CT16B0 (16-bit Timer 0)
    TIMER16_1_IRQHandler,             // 17 - CT16B1 (16-bit Timer 1)
    TIMER32_0_IRQHandler,             // 18 - CT32B0 (32-bit Timer 0)
    TIMER32_1_IRQHandler,             // 19 - CT32B1 (32-bit Timer 1)
    SSP0_IRQHandler,                  // 20 - SPI/SSP0 Interrupt
    USART_IRQHandler,                 // 21 - UART0
    USB_IRQHandler,                   // 22 - USB IRQ
    USB_FIQHandler,                   // 23 - USB FIQ
    ADC_IRQHandler,                   // 24 - ADC (A/D Converter)
    WDT_IRQHandler,                   // 25 - WDT (Watchdog Timer)
    BOD_IRQHandler,                   // 26 - BOD (Brownout Detect)
    FMC_IRQHandler,                   // 27 - IP2111 Flash Memory Controller
    0,                                // 28 - Reserved
    0,                                // 29 - Reserved
    USBWakeup_IRQHandler,             // 30 - USB wake-up interrupt
    IOH_IRQHandler,                   // 31 - I/OH Interrupt

#else
#error No CHIP_11* device defined  
#endif
};

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int *pulSrc = (unsigned int*) romstart;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors")))
void bss_init(unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = 0;
}

#ifndef USE_OLD_STYLE_DATA_BSS_INIT
//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;
#else
//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the load address, execution address and length of the RW data section and
// the execution and length of the BSS (zero initialized) section.
// Note that these symbols are not normally used by the managed linker script
// mechanism in Red Suite/LPCXpresso 3.6 (Windows) and LPCXpresso 3.8 (Linux).
// They are provide here simply so this startup code can be used with earlier
// versions of Red Suite which do not support the more advanced managed linker
// script mechanism introduced in the above version. To enable their use,
// define "USE_OLD_STYLE_DATA_BSS_INIT".
//*****************************************************************************
extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;
#endif


//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void
ResetISR(void) {

#ifndef USE_OLD_STYLE_DATA_BSS_INIT
    //
    // Copy the data sections from flash to SRAM.
    //
	unsigned int LoadAddr, ExeAddr, SectionLen;
	unsigned int *SectionTableAddr;

	// Load base address of Global Section Table
	SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
	while (SectionTableAddr < &__data_section_table_end) {
		LoadAddr = *SectionTableAddr++;
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		data_init(LoadAddr, ExeAddr, SectionLen);
	}
	// At this point, SectionTableAddr = &__bss_section_table;
	// Zero fill the bss segment
	while (SectionTableAddr < &__bss_section_table_end) {
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		bss_init(ExeAddr, SectionLen);
	}
#else
	// Use Old Style Data and BSS section initialization.
	// This will only initialize a single RAM bank.
	unsigned int * LoadAddr, *ExeAddr, *EndAddr, SectionLen;

    // Copy the data segment from flash to SRAM.
	LoadAddr = &_etext;
	ExeAddr = &_data;
	EndAddr = &_edata;
	SectionLen = (void*)EndAddr - (void*)ExeAddr;
	data_init((unsigned int)LoadAddr, (unsigned int)ExeAddr, SectionLen);
	// Zero fill the bss segment
	ExeAddr = &_bss;
	EndAddr = &_ebss;
	SectionLen = (void*)EndAddr - (void*)ExeAddr;
	bss_init ((unsigned int)ExeAddr, SectionLen);
#endif

	extern void SystemInit(void);
	SystemInit();

#if defined (__cplusplus)
	//
	// Call C++ library initialisation
	//
	__libc_init_array();
#endif

#if defined (__REDLIB__)
	// Call the Redlib library, which in turn calls main()
	__main() ;
#else
	main();
#endif
	//
	// main() shouldn't return, but if it does, we'll just enter an infinite loop
	//
	while (1) {
		;
	}
}

//*****************************************************************************
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void NMI_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void HardFault_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void SVC_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void PendSV_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void SysTick_Handler(void)
{
    while(1)
    {
    }
}

//*****************************************************************************
//
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
    while(1)
    {
    }
}

