/**
 * \file jingle_data.h
 * \brief Encompasses utilities for interfacing with Jingle data format used
 *	in official Steam Controller firmware. A Jingle is a series of Notes
 *	played via the left and right haptics.
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

#ifndef _JINGLE_DATA_
#define _JINGLE_DATA_

#include "haptic.h"

#include "lpc_types.h"

#include <stdint.h>

uint8_t getNumJingles();
uint16_t getNumJingleNotes(enum Haptic haptic, uint8_t idx);
Note* getJingleNotes(enum Haptic haptic, uint8_t idx);
uint16_t getNumJingleBytesFree();
int addJingle(uint16_t numNotesRight, uint16_t numNotesLeft);
int delJingle(uint8_t idx);
bool jingleDataIsValid();
int playJingle(uint8_t idx);

int loadJingleEEPROM();
int saveJingleEEPROM();

void jingleCmdUsage(void);
int jingleCmdFnc(int argc, const char* argv[]);

#endif /* _JINGLE_DATA_ */

