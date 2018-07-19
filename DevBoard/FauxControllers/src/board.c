/*
 * @brief NXP LPCXpresso 11U37H board file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "retarget.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define BUTTONS_BUTTON1_GPIO_PORT_NUM			0
#define BUTTONS_BUTTON1_GPIO_BIT_NUM			16
#define BUTTONS_BUTTON2_GPIO_PORT_NUM			0
#define BUTTONS_BUTTON2_GPIO_BIT_NUM			1

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	Chip_UART_SendBlocking(DEBUG_UART, &ch, 1);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	uint8_t data;

	if (Chip_UART_Read(DEBUG_UART, &data, 1) == 1) {
		return (int) data;
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, IOCON_FUNC1 | IOCON_MODE_INACT); /* PIO0_18 used for RXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, IOCON_FUNC1 | IOCON_MODE_INACT); /* PIO0_19 used for TXD */

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART, 115200);
	Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART);
#endif
}

#define MAXLEDS 7
static const uint8_t ledports[MAXLEDS] = {1, 1, 1, 1, 1, 1, 1};
static const uint8_t ledpins[MAXLEDS] = {0, 1, 2, 3, 24, 25, 26};

/* Initializes board LED(s) */
static void Board_LED_Init(void)
{
	int idx;

	for (idx = 0; idx < MAXLEDS; idx++) {
		/* Set the GPIO as output with initial state off (low) */
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, ledports[idx], ledpins[idx]);
		Chip_GPIO_SetPinState(LPC_GPIO, ledports[idx], ledpins[idx], false);
	}
}

/* Sets the state of a board LED to on or off */
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber < MAXLEDS) {
		/* Toggle state, low is on, high is off */
		Chip_GPIO_SetPinState(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber], !On);
	}
}

/* Returns the current state of a board LED */
bool Board_LED_Test(uint8_t LEDNumber)
{
	bool state = false;

	if (LEDNumber < MAXLEDS) {
		state = !Chip_GPIO_GetPinState(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber]);
	}

	return state;
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
	if (LEDNumber < MAXLEDS) {
		Chip_GPIO_SetPinToggle(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber]);
	}
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Initialize GPIO */
	Chip_GPIO_Init(LPC_GPIO);

	/* Initialize LEDs */
	Board_LED_Init();
}

/* Initialize buttons on the board */
void Board_Buttons_Init(void)
{
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM);
}

/* Get button status */
uint32_t Buttons_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;
	if (!Chip_GPIO_GetPinState(LPC_GPIO, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM)) {
		ret |= BUTTONS_BUTTON1;
	}
	if (!Chip_GPIO_GetPinState(LPC_GPIO, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM)) {
		ret |= BUTTONS_BUTTON2;
	}
	return ret;
}

/* Ordered up, down, left, right, press */
#define NUM_BUTTONS 5
static const uint8_t portButton[NUM_BUTTONS] = {1, 1, 1, 1, 1};
static const uint8_t pinButton[NUM_BUTTONS] = {22, 20, 23, 21, 19};
static const uint8_t stateButton[NUM_BUTTONS] = {JOY_UP, JOY_DOWN, JOY_LEFT,
	JOY_RIGHT, JOY_PRESS};

/* Initialize Joystick */
void Board_Joystick_Init(void)
{
	int i;

	/* IOCON states already selected in SystemInit(), GPIO setup only */
	for (i = 0; i < NUM_BUTTONS; i++) {
	  	Chip_GPIO_SetPinDIRInput(LPC_GPIO, portButton[i], pinButton[i]);
	}
}

/* Get Joystick status */
uint8_t Joystick_GetStatus(void)
{
	uint8_t i, ret = 0;

	for (i = 0; i < NUM_BUTTONS; i++) {
		if ((Chip_GPIO_GetPinState(LPC_GPIO, portButton[i], pinButton[i])) == 0x00) {
			ret |= stateButton[i];
		}
	}

	return ret;
}


/* Sets up board specific I2C interface */
void Board_I2C_Init(I2C_ID_T id)
{
	/* Pin muxing handled in SystemInit() or application */
	(void)id;
}
