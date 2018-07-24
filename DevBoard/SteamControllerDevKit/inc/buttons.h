/**
 * \file buttons.h
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

#ifndef _BUTTONS_
#define _BUTTONS_

#include <stdint.h>

void initButtons(void);

int getSteamButtonState(void);
int getFrontLeftButtonState(void);
int getFrontRightButtonState(void);

int getJoyClickState(void);

int getXButtonState(void);
int getYButtonState(void);
int getBButtonState(void);
int getAButtonState(void);

int getRightGripState(void);
int getLeftGripState(void);

int getRightTrackpadClickState(void);
int getLeftTrackpadClickState(void);

int getRightTriggerState(void);
int getLeftTriggerState(void);

int getRightBumperState(void);
int getLeftBumperState(void);

#endif /* _BUTTONS_ */


