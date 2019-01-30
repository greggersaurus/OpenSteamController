/**
 * \file time.c
 *
 * \brief Encompasses functions for time related tasks (i.e. sleeping, tracking
 *  how long something took).
 *
 * MIT License
 *
 * Copyright (c) 2019 Gregory Gluszek
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

#include "time.h"

#include "haptic.h"

/**
 * Any initialization related to time functions.
 * 
 * \return None.
 */
void initTime(void) {
	// Make sure haptics are initialized as we are using their time 
	//  functions (for now at least)
	initHaptics();
}

/**
 * Sleep for the specific number of microseconds.
 * 
 * \param usec The number of microseconds to sleep for.
 * 
 * \return None.
 */
void usleep(uint32_t usec) {
	// Share timer used by haptics
	usleepHaptic(usec);
}

/**
 * Get the current value of a timer running with usec precision.
 * 
 * \return The count value for a timer configured where the count increments
 *	each usec. 
 */
uint32_t getUsTickCnt(void) {
	// Share timer used by haptics
	return getUsTickCntHaptic();
}
