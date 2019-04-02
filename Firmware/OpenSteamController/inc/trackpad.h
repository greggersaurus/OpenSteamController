/**
 * \file trackpad.h
 * \brief Encompasses functions communicating with Trackpads to get X/Y 
 *	locations.
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

#ifndef _TRACKPAD_ 
#define _TRACKPAD_

#include <stdint.h>

/**
 * Defines which Trackpad is being communicated with.
 */
typedef enum Trackpad_t {
	R_TRACKPAD = 0,
	L_TRACKPAD = 1
} Trackpad;

void initTrackpad(void);

void trackpadLocUpdate(Trackpad trackpad);
void trackpadGetLastXY(Trackpad trackpad, uint16_t* xLoc, uint16_t* yLoc);

void trackpadCmdUsage(void);
int trackpadCmdFnc(int argc, const char* argv[]);

#endif /* _TRACKPAD_ */


