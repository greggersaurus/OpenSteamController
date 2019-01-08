/**
 * \file haptic.h
 * \brief Encompasses functions for interfacing with haptics.
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

#ifndef _HAPTIC_ 
#define _HAPTIC_

#include <stdint.h>

/**
 * Defines which haptic we are communicating with.
 */
typedef enum Haptic {
	R_HAPTIC = 0,
	L_HAPTIC = 1
} Haptic;

/**                                                                     
 * Contains information needed to produce a tone for a duration via     
 *  the Steam Controller haptics.                                       
 */                                                                     
typedef struct Note {                                                           
	uint8_t dutyCycle; //!< Percentage time that pulse is high, where 
		//!< value with 0% = 0 and 100% = 511
	uint8_t RSVD;
	uint16_t pulseFreq; //!< Frequency of the pulse being generated for
		//!< this note in Hz.
	uint16_t duration; //!< Duration of the note in milliseconds.
} Note;

void initHaptics(void);
void hapticCmdUsage(void);
int hapticCmdFnc(int argc, const char* argv[]);
int playHaptic(enum Haptic haptic, const struct Note* notes, uint32_t numNotes);

#endif /* _HAPTIC_ */

