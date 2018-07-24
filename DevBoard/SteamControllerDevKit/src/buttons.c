/**
 * \file buttons.c
 * \brief Encompasses functions for reading Steam Controller digital button 
 *	states.
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

#include "buttons.h"

#include "lpc_types.h"
#include "chip.h"
#include "gpio_11xx_1.h"

#define GPIO_ANALOG_JOY_CLICK 1, 0
#define GPIO_L_GRIP 1, 25
#define GPIO_STEAM_BTN 1, 19
#define GPIO_L_TRACKPAD 1, 26
#define GPIO_L_TRIGGER 1, 27
#define GPIO_L_BUMPER 1, 4
#define GPIO_FRONT_L 1, 20
#define GPIO_FRONT_R 1, 2
#define GPIO_R_TRACKPAD 1, 21
#define GPIO_Y_BTN 1, 11
#define GPIO_R_TRIGGER 1, 13
#define GPIO_R_BUMPER 1, 14
#define GPIO_R_GRIP 1, 3
#define GPIO_B_BTN 1, 22
#define GPIO_X_BTN 1, 9
#define GPIO_A_BTN 0, 17

/**
 * Initialize GPIOs used to read button states.
 *
 * \return None.
 */
void initButtons(void) {
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_ANALOG_JOY_CLICK, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_GRIP, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_STEAM_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRIGGER, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_BUMPER, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_FRONT_L, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_FRONT_R, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_TRACKPAD, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_Y_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_TRIGGER, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_BUMPER, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_GRIP, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_B_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_X_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_A_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
}

/**
 * \return true if Steam Button is being pressed. False otherwise.
 */
int getSteamButtonState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_STEAM_BTN);
}

/**
 * \return true if Front Left Arrow Button is being pressed. False otherwise.
 */
int getFrontLeftButtonState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_FRONT_L);
}

/**
 * \return true if Front Right Arrow Button is being pressed. False otherwise.
 */
int getFrontRightButtonState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_FRONT_R);
}

/**
 * \return true if Analog Joystick Click Button is being pressed. False otherwise.
 */
int getJoyClickState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_ANALOG_JOY_CLICK);
}

/**
 * \return true if X Button is being pressed. False otherwise.
 */
int getXButtonState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_X_BTN);
}

/**
 * \return true if Y Button is being pressed. False otherwise.
 */
int getYButtonState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_Y_BTN);
}

/**
 * \return true if B Button is being pressed. False otherwise.
 */
int getBButtonState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_B_BTN);
}

/**
 * \return true if A Button is being pressed. False otherwise.
 */
int getAButtonState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_A_BTN);
}

/**
 * \return true if Inner Right Grip Button is being pressed. False otherwise.
 */
int getRightGripState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_GRIP);
}

/**
 * \return true if Inner Left Grip Button is being pressed. False otherwise.
 */
int getLeftGripState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_L_GRIP);
}

/**
 * \return true if Right Trackpad Click Button is being pressed. False otherwise.
 */
int getRightTrackpadClickState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD);
}

/**
 * \return true if Left Trackpad Click Button is being pressed. False otherwise.
 */
int getLeftTrackpadClickState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_L_TRACKPAD);
}

/**
 * \return true if Right Digital Trigger Button is being pressed. False otherwise.
 */
int getRightTriggerState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRIGGER);
}

/**
 * \return true if Left Digital Trigger Button is being pressed. False otherwise.
 */
int getLeftTriggerState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_L_TRIGGER);
}

/**
 * \return true if Right Bumper Button is being pressed. False otherwise.
 */
int getRightBumperState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_BUMPER);
}

/**
 * \return true if Left Bumper Button is being pressed. False otherwise.
 */
int getLeftBumperState(void) {
	return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_L_BUMPER);
}
