/**
 * \file haptic.c
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

#include "haptic.h"

#include <stdlib.h>
#include <string.h>

#include "console.h"

#include "lpc_types.h"
#include "chip.h"
#include "timer_11xx.h"

#define GPIO_HAPTICS_EN_N 1, 7
#define GPIO_HAPTICS_L 0, 18
#define GPIO_HAPTICS_R 1, 12

/**
 * \return the Match Register number for the corresponding haptic.
 */
inline static int8_t getHapticMR(Haptic haptic) {
	return (int8_t)haptic + 1;
}

/**
 * Change state of GPIO that controls haptic.
 */
inline static void setHapticGpioState(Haptic haptic, bool setting) {
	if (haptic == R_HAPTIC) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_R, setting);
	}
	if (haptic == L_HAPTIC) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_L, setting);
	}
}

/**
 * \return Get state of GPIO that controls haptic.
 */
inline static bool getHapticGpioState(Haptic haptic) {
	if (haptic == R_HAPTIC) {
		return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_HAPTICS_R);
	}
	if (haptic == L_HAPTIC) {
		return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_HAPTICS_L);
	}

	return false;
}

static LPC_TIMER_T* hapticTimer = LPC_TIMER32_0;

// Breakdown of jingle default data
/*
	0xbead // magic word						0x000
	0x0000 // unused						0x002
	0x0e // number of jingles					0x004
	0x7b // Unknown...						0x005
	0x0022 // Offset for jingle[0]					0x006
	0x0062 // Offset for jingle[1]					0x008
	0x00de // Offset for jingle[2]					0x00a
	0x012a // Offset for jingle[3]					0x00c
	0x0158 // Offset for jingle[4]					0x00e
	0x017a // Offset for jingle[5]					0x010
	0x0196 // Offset for jingle[6]					0x012
	0x01dc // Offset for jingle[7]					0x014
	0x02a0 // Offset for jingle[8]					0x016
	0x02ce // Offset for jingle[9]					0x018
	0x02f6 // Offset for jingle[10]					0x01a
	0x0318 // Offset for jingle[11]					0x01c
	0x034c // Offset for jingle[12]					0x01e
	0x036e // Offset for jingle[13]					0x020
	0x0005 // Number of notes in right Jingle[0] 			0x022
	0x0005 // Number of notes in left Jingle[0]			0x024
	0x005e // Jingle[0] Right Note[0].highDuration			0x026
	0x0417 // Jingle[0] Right Note[0].lowDuration			0x028
	0x0088 // Jingle[0] Right Note[0].repeatCnt			0x02a
	0x007f // Jingle[0] Right Note[1].highDuration			0x02c
	0x0497 // Jingle[0] Right Note[1].lowDuration			0x02e
	0x0088 // Jingle[0] Right Note[1].repeatCnt			0x030
	0x0000 // Jingle[0] Right Note[2].highDuration			0x032
	0x0000 // Jingle[0] Right Note[2].lowDuration			0x034 
	0x0001 // Jingle[0] Right Note[2].repeatCnt			0x036
	0x0033 // Jingle[0] Right Note[3].highDuration			0x038
	0x0527 // Jingle[0] Right Note[3].lowDuration			0x03a
	0x0088 // Jingle[0] Right Note[3].repeatCnt			0x03c
	0x004f // Jingle[0] Right Note[4].highDuration			0x03e
	0x0575 // Jingle[0] Right Note[4].lowDuration			0x040 
	0x00c7 // Jingle[0] Right Note[4].repeatCnt			0x042
	0x0054 // Jingle[0] Left Note[0].highDuration			0x044
	0x0575 // Jingle[0] Left Note[0].lowDuration			0x046
	0x0088 // Jingle[0] Left Note[0].repeatCnt			0x048
	0x007b // Jingle[0] Left Note[1].highDuration			0x04a
	0x06e0 // Jingle[0] Left Note[1].lowDuration			0x04e
	0x0088 // Jingle[0] Left Note[1].repeatCnt			0x04c
	0x0000 // Jingle[0] Left Note[2].highDuration			0x04e
	0x0000 // Jingle[0] Left Note[2].lowDuration			0x052
	0x0001 // Jingle[0] Left Note[2].repeatCnt			0x054
	0x0033 // Jingle[0] Left Note[3].highDuration			0x056
	0x0620 // Jingle[0] Left Note[3].lowDuration			0x058
	0x0088 // Jingle[0] Left Note[3].repeatCnt			0x05a
	0x004f // Jingle[0] Left Note[4].highDuration			0x05c
	0x06e0 // Jingle[0] Left Note[4].lowDuration			0x05e
	0x00c7 // Jingle[0] Left Note[4].repeatCnt			0x060
	0x000a
	0x000a 
	 0x0071
	0x0575 
	 0x0022
	0x005c 
	 0x0527
	0x0022 
	 0x0051
	0x04dd 
	 0x0022
	0x0043 
	 0x0417
	0x0022 
	 0x003b
	0x03a4 
	 0x0022
	0x0033 
	 0x0417
	 0x00000022 
	 0x00010000 
	 0x04dd002c 
	 0x00260022 
	 0x00220527 
	 0x05750021 
	 0x00710022 
	 0x002206e0 
	 0x0620005c 
	 0x00510022 
	 0x00220575 
	 0x05270043 
	 0x003b0022 
	 0x00220497 
	 0x05270033 
	 0x00000022 
	 0x00010000 
	 0x0575002c 
	 0x00260022 
	 0x00220620 
	 0x06e00021 
	 0x000a0022 
	 0x00600002 
	 0x002f0dc0 
	 0x00000000 
	 0x007f0016 
	 0x00410417 
	 0x00000000 
	 0x007d0003 
	 0x002c0dc0 
	 0x00000000 
	 0x00000001 
	 0x00180000 
	 0x0417007b 
	 0x00000048 
	 0x00410000 
	 0x04170075 
	 0x0000003e 
	 0x01110000 
	 0x0dc0007f 
	 0x00050032 
	 0x005c0002 
	 0x008802ba 
	 0x00000000 
	 0x00000088 
	 0x00010000 
	 0x02ba0066 
	 0x00760088 
	 0x01180417 
	 0x00000000 
	 0x00380199 
	 0x01190575 
	 0x00030002 
	 0x02ba0040 
	 0x00560199 
	 0x02440417 
	 0x00000000 
	 0x00460111 
	 0x011103a4 
	 0x0575008e 
	 0x000201bb 
	 0x00400002 
	 0x00880575 
	 0x082d00c8 
	 0x00000062 
	 0x00440000 
	 0x06e0007a 
	 0x000600a5 
	 0x005e0005 
	 0x00aa0417 
	 0x00000000 
	 0x00000066 
	 0x00010000 
	 0x0370004b 
	 0x00000066 
	 0x00220000 
	 0x0575007f 
	 0x00000066 
	 0x00880000 
	 0x02ba002e 
	 0x000000aa 
	 0x00010000 
	 0x00000000 
	 0x005e0022 
	 0x00660417 
	 0x001c0004 
	 0x015d000e 
	 0x002a0332 
	 0x03100188 
	 0x00000000 
	 0x00320022 
	 0x0332020b 
	 0x01d20030 
	 0x0000005b 
	 0x000b0000 
	 0x01b80036 
	 0x00000084 
	 0x00050000 
	 0x01d2003e 
	 0x00000057 
	 0x000f0000 
	 0x01b80038 
	 0x00000083 
	 0x00060000 
	 0x01d20046 
	 0x00000044 
	 0x01110000 
	 0x024b0058 
	 0x004c0066 
	 0x0084020b 
	 0x00000000 
	 0x00520005 
	 0x0066024b 
	 0x020b0048 
	 0x00000083 
	 0x00060000 
	 0x024b004e 
	 0x00000044 
	 0x00010000 
	 0x00000000 
	 0x00660111 
	 0x006602ba 
	 0x00000000 
	 0x00820001 
	 0x00850293 
	 0x00000000 
	 0x007c0003 
	 0x006602ba 
	 0x02930090 
	 0x00000080 
	 0x00010000 
	 0x00000000 
	 0x00aa0008 
	 0x015502ba 
	 0x00000007 
	 0x0575007f 
	 0x00000032 
	 0x002f0000 
	 0x0497007f 
	 0x0000003d 
	 0x00340000 
	 0x05750078 
	 0x00000039 
	 0x00430000 
	 0x0497007f 
	 0x00030046 
	 0x00460003 
	 0x008b0293 
	 0x00000000 
	 0x005a0020 
	 0x00aa0293 
	 0x02ba0052 
	 0x0000008b 
	 0x00200000 
	 0x02ba0046 
	 0x0002009e 
	 0x00470003 
	 0x01110575 
	 0x03700032 
	 0x00260111 
	 0x008806e0 
	 0x04170022 
	 0x00330111 
	 0x008802ba 
	 0x00030005 
	 0x0417005e 
	 0x007f0088 
	 0x008803a4 
	 0x00000000 
	 0x00330001 
	 0x00880370 
	 0x02ba004f 
	 0x000000c7 
	 0x01110000 
	 0x05750054 
	 0x007b0088 
	 0x00cd06e0 
	 0x00000005 
	 0x06200064 
	 0x00c800c8 
	 0x00c805c8 
	 0x05270064 
	 0x00c800c8 
	 0x00c80527 
	 0x05c80064 
	 0x000b00d6 
	 0x0000000b 
	 0x00040000 
	 0x03dc0056 
	 0x000000c6 
	 0x00040000 
	 0x03a4005b 
	 0x000000ce 
	 0x00010000 
	 0x00000000 
	 0x005b0006 
	 0x00ce0370 
	 0x00000000 
	 0x003b0003 
	 0x00c10620 
	 0x00000000 
	 0x00610002 
	 0x00d70293 
	 0x00000000 
	 0x00390006 
	 0x00c507b8 
	 0x00000000 
	 0x00320005 
	 0x00ce0749 
	 0x00000000 
	 0x00000001 
	 0x00040000 
	 0x06e0003f 
	 0x000000c8 
	 0x000a0000 
	 0x0310005b 
	 0x000000bf 
	 0x00090000 
	 0x05270035 
	 0x000000d5 
	 0x00 (modified bits = 0x00)
	 0x00 (modified bits = 0x00)
*/

//TODO: add details on how this was pulled from firmware vcf_wired_controller_d0g_57bf5c10.bin (address 0x10001200)
uint32_t defaultJingleData[] = {
	 0x0000bead,
	 0x00227b0e,
	 0x00de0062,
	 0x0158012a,
	 0x0196017a,
	 0x02a001dc,
	 0x02f602ce,
	 0x034c0318,
	 0x0005036e,
	 0x005e0005,
	 0x00880417,
	 0x0497007f,
	 0x00000088,
	 0x00010000,
	 0x05270033,
	 0x004f0088,
	 0x00c70575,
	 0x05750054,
	 0x007b0088,
	 0x008806e0,
	 0x00000000,
	 0x00330001,
	 0x00880620,
	 0x06e0004f,
	 0x000a00c7,
	 0x0071000a,
	 0x00220575,
	 0x0527005c,
	 0x00510022,
	 0x002204dd,
	 0x04170043,
	 0x003b0022,
	 0x002203a4,
	 0x04170033,
	 0x00000022,
	 0x00010000,
	 0x04dd002c,
	 0x00260022,
	 0x00220527,
	 0x05750021,
	 0x00710022,
	 0x002206e0,
	 0x0620005c,
	 0x00510022,
	 0x00220575,
	 0x05270043,
	 0x003b0022,
	 0x00220497,
	 0x05270033,
	 0x00000022,
	 0x00010000,
	 0x0575002c,
	 0x00260022,
	 0x00220620,
	 0x06e00021,
	 0x000a0022,
	 0x00600002,
	 0x002f0dc0,
	 0x00000000,
	 0x007f0016,
	 0x00410417,
	 0x00000000,
	 0x007d0003,
	 0x002c0dc0,
	 0x00000000,
	 0x00000001,
	 0x00180000,
	 0x0417007b,
	 0x00000048,
	 0x00410000,
	 0x04170075,
	 0x0000003e,
	 0x01110000,
	 0x0dc0007f,
	 0x00050032,
	 0x005c0002,
	 0x008802ba,
	 0x00000000,
	 0x00000088,
	 0x00010000,
	 0x02ba0066,
	 0x00760088,
	 0x01180417,
	 0x00000000,
	 0x00380199,
	 0x01190575,
	 0x00030002,
	 0x02ba0040,
	 0x00560199,
	 0x02440417,
	 0x00000000,
	 0x00460111,
	 0x011103a4,
	 0x0575008e,
	 0x000201bb,
	 0x00400002,
	 0x00880575,
	 0x082d00c8,
	 0x00000062,
	 0x00440000,
	 0x06e0007a,
	 0x000600a5,
	 0x005e0005,
	 0x00aa0417,
	 0x00000000,
	 0x00000066,
	 0x00010000,
	 0x0370004b,
	 0x00000066,
	 0x00220000,
	 0x0575007f,
	 0x00000066,
	 0x00880000,
	 0x02ba002e,
	 0x000000aa,
	 0x00010000,
	 0x00000000,
	 0x005e0022,
	 0x00660417,
	 0x001c0004,
	 0x015d000e,
	 0x002a0332,
	 0x03100188,
	 0x00000000,
	 0x00320022,
	 0x0332020b,
	 0x01d20030,
	 0x0000005b,
	 0x000b0000,
	 0x01b80036,
	 0x00000084,
	 0x00050000,
	 0x01d2003e,
	 0x00000057,
	 0x000f0000,
	 0x01b80038,
	 0x00000083,
	 0x00060000,
	 0x01d20046,
	 0x00000044,
	 0x01110000,
	 0x024b0058,
	 0x004c0066,
	 0x0084020b,
	 0x00000000,
	 0x00520005,
	 0x0066024b,
	 0x020b0048,
	 0x00000083,
	 0x00060000,
	 0x024b004e,
	 0x00000044,
	 0x00010000,
	 0x00000000,
	 0x00660111,
	 0x006602ba,
	 0x00000000,
	 0x00820001,
	 0x00850293,
	 0x00000000,
	 0x007c0003,
	 0x006602ba,
	 0x02930090,
	 0x00000080,
	 0x00010000,
	 0x00000000,
	 0x00aa0008,
	 0x015502ba,
	 0x00000007,
	 0x0575007f,
	 0x00000032,
	 0x002f0000,
	 0x0497007f,
	 0x0000003d,
	 0x00340000,
	 0x05750078,
	 0x00000039,
	 0x00430000,
	 0x0497007f,
	 0x00030046,
	 0x00460003,
	 0x008b0293,
	 0x00000000,
	 0x005a0020,
	 0x00aa0293,
	 0x02ba0052,
	 0x0000008b,
	 0x00200000,
	 0x02ba0046,
	 0x0002009e,
	 0x00470003,
	 0x01110575,
	 0x03700032,
	 0x00260111,
	 0x008806e0,
	 0x04170022,
	 0x00330111,
	 0x008802ba,
	 0x00030005,
	 0x0417005e,
	 0x007f0088,
	 0x008803a4,
	 0x00000000,
	 0x00330001,
	 0x00880370,
	 0x02ba004f,
	 0x000000c7,
	 0x01110000,
	 0x05750054,
	 0x007b0088,
	 0x00cd06e0,
	 0x00000005,
	 0x06200064,
	 0x00c800c8,
	 0x00c805c8,
	 0x05270064,
	 0x00c800c8,
	 0x00c80527,
	 0x05c80064,
	 0x000b00d6,
	 0x0000000b,
	 0x00040000,
	 0x03dc0056,
	 0x000000c6,
	 0x00040000,
	 0x03a4005b,
	 0x000000ce,
	 0x00010000,
	 0x00000000,
	 0x005b0006,
	 0x00ce0370,
	 0x00000000,
	 0x003b0003,
	 0x00c10620,
	 0x00000000,
	 0x00610002,
	 0x00d70293,
	 0x00000000,
	 0x00390006,
	 0x00c507b8,
	 0x00000000,
	 0x00320005,
	 0x00ce0749,
	 0x00000000,
	 0x00000001,
	 0x00040000,
	 0x06e0003f,
	 0x000000c8,
	 0x000a0000,
	 0x0310005b,
	 0x000000bf,
	 0x00090000,
	 0x05270035,
	 0x000000d5,
	 0x00000000
};
	
static volatile bool hapticBusy[2]; //!< non-zero if the haptic is currently 
	//!< playing a sequence.
static const Note* volatile hapticNotes[2]; //!< Pointer to the first note in a 
 	//!< sequence to play on the haptics.
static volatile int hapticNotesIdx[2]; //!< Index of which note is currently
	//!< being played on a haptic.
static volatile int hapticNotesLen[2]; //!< The number of notes in a sequence 
	//!< to be played on a haptic.
static volatile uint32_t pulseHiDur[2]; //!< Number of microseconds for which
	//!< Haptic GPIO is high for pulse being generated for current Note.
static volatile uint32_t pulseLoDur[2]; //!< Number of microseconds for which
	//!< Haptic GPIO is low for pulse being generated for current Note.
static volatile uint32_t pulseRptCntr[2]; //!< Down counter for the number of
		//!< of times to geneate the pulse sequence.

/**
 * Convert data from Note struct to microsecond durations and counter values to
 *  to be used by interrupt handler for toggling GPIO appropriately.
 * 
 * \param haptic Defines which haptic we are referring too. 
 * \param[in] note Points to Note to be played.
 *
 * \return None.
 */
static void startHapticNote(Haptic haptic, const struct Note* note) {
	// Make sure pointer is valid
	if (!note) {
		return;
	}

	// Make sure note is not currently being played
	if (hapticBusy[haptic]) {
		return;
	}
	
	if (!note->duration) {
		return;
	}

	// Convert data in struct Note to variables for IRQ handler
	if (!note->dutyCycle || !note->pulseFreq) {
		pulseHiDur[haptic] = 0;
		pulseLoDur[haptic] = 0;
		pulseRptCntr[haptic] = 1;

		// This Note is just a delay, so keep GPIO low
		setHapticGpioState(haptic, false);

		// Setup interrupt to occur after full delay
		hapticTimer->MR[getHapticMR(haptic)] = note->duration * 1000 + 
			Chip_TIMER_ReadCount(hapticTimer);
	} else {
		uint32_t pulse_width = 1000000 / note->pulseFreq;

		pulseHiDur[haptic] = (pulse_width * note->dutyCycle) / 512;
		pulseLoDur[haptic] = pulse_width - pulseHiDur[haptic];
		pulseRptCntr[haptic] = (note->duration * note->pulseFreq) / 1000;

		// Start with haptic GPIO in high state
		setHapticGpioState(haptic, true);

		// Setup interrupt to occur when high portion is done
		hapticTimer->MR[getHapticMR(haptic)] = pulseHiDur[haptic] + 
			Chip_TIMER_ReadCount(hapticTimer);
	}

	// Mark that note is being played
	hapticBusy[haptic] = true;
}

/**
 * Change state of haptic based on currently setup sequence.
 * 
 * \param haptic Defines which haptic we are referring too. 
 *
 * \return None.
 */
static void nextHapticState(Haptic haptic) {
	const Note* notes = hapticNotes[haptic];

	if (!notes) {
		Chip_TIMER_MatchDisableInt(hapticTimer, getHapticMR(haptic));
	}

	if (getHapticGpioState(haptic)) {
		// High portion of pulse is finished, on to low portion
		setHapticGpioState(haptic, false);
		// Setup interrupt to occur when low portion is done
		hapticTimer->MR[getHapticMR(haptic)] = 
			pulseLoDur[haptic] + Chip_TIMER_ReadCount(hapticTimer);
	} else {
		// Pulse low finished (i.e. iteration of pulse is complete)
		pulseRptCntr[haptic]--;

		// Check if another pulse is to be generated for this Note
		if (pulseRptCntr[haptic]) {
			// Start another high portion of pulse
			setHapticGpioState(haptic, true);
			// Setup interrupt to occur when high portion is done
			hapticTimer->MR[getHapticMR(haptic)] = 
				pulseHiDur[haptic] + 
				Chip_TIMER_ReadCount(hapticTimer);
		} else {
			// Attempt to move onto next note in sequence
			hapticBusy[haptic] = false;			
			hapticNotesIdx[haptic]++;
			if (hapticNotesIdx[haptic] < hapticNotesLen[haptic]) {
				startHapticNote(haptic, 
					&notes[hapticNotesIdx[haptic]]);
			} else {
				// Stop interrupt from firing as all notes in 
				//  sequence have been played
				Chip_TIMER_MatchDisableInt(hapticTimer, 
					getHapticMR(haptic));
			}
		}
	}
}

volatile uint32_t ct32b0_cnt = 0;

/**
 * Interrupt handler for CT32B0. Used to toggle Haptics GPIOs based on struct
 *   Note speciications.
 *
 * \return None.
 */
void TIMER32_0_IRQHandler(void) {

	ct32b0_cnt++;

	// Check if interrupt was generated by Right haptic finishing high or
	//  low pulse
	if (Chip_TIMER_MatchPending(hapticTimer, getHapticMR(R_HAPTIC))) {
		Chip_TIMER_ClearMatch(hapticTimer, getHapticMR(R_HAPTIC));
		nextHapticState(R_HAPTIC);
	}

	// Check if interrupt was generated by Left haptic finishing high or
	//  low pulse
	if (Chip_TIMER_MatchPending(hapticTimer, getHapticMR(L_HAPTIC))) {
		Chip_TIMER_ClearMatch(hapticTimer, getHapticMR(L_HAPTIC));
		nextHapticState(L_HAPTIC);
	}
}

/**
 * Initialization that needs to happen for haptics to work.
 * 
 * \return None.
 */
void initHaptics(void) {
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_EN_N, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, GPIO_HAPTICS_EN_N, true);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_HAPTICS_EN_N, IOCON_MODE_PULLDOWN, 
		IOCON_FUNC0);

	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_L, false);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_HAPTICS_L);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_HAPTICS_L, IOCON_DIGMODE_EN, 
		IOCON_FUNC0);

	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_R, false);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_HAPTICS_R);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_HAPTICS_R, IOCON_DIGMODE_EN, 
		IOCON_FUNC0);

	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_EN_N, false);

	Chip_TIMER_Init(hapticTimer);

	// Set the timer to increment every microsecond
	Chip_TIMER_PrescaleSet(hapticTimer, SystemCoreClock/1000000-1);

	// Set priority of Timer IRQ to max
	NVIC_SetPriority(TIMER_32_0_IRQn, 0);
	
	// Clear interrup pending for Timer IRQ
	NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);

	// Enable Timer interrupt
	NVIC_EnableIRQ(TIMER_32_0_IRQn);

	Chip_TIMER_Enable(hapticTimer);

	//TODO: play startup jingle
}

int playDefaultJingle(int jingleNum) {
	const uint8_t* defaultJingleBytes = (const uint8_t*)defaultJingleData;

	if (jingleNum >= defaultJingleBytes[4]) {
		consolePrint("jingleNum %d too large\n", jingleNum);
		return -1;
	}

	consolePrint("sizeof(Note) = %d\n", sizeof(Note));

	consolePrint("defaultJingleBytes = 0x%08x\n", defaultJingleBytes);
	uint16_t offset = *((uint16_t*)&defaultJingleBytes[6 + 2*jingleNum]);
	consolePrint("offset = 0x%03x\n", offset);
	uint16_t numNotesRight = *((uint16_t*)&defaultJingleBytes[offset]);
	consolePrint("numNotesRight = %d\n", numNotesRight);
	uint16_t numNotesLeft = *((uint16_t*)&defaultJingleBytes[offset + 2]);
	consolePrint("numNotesLeft = %d\n", numNotesLeft);
	struct Note* notesRight = (struct Note*)&defaultJingleBytes[offset + 4];
	consolePrint("notesRight = 0x%08x\n", notesRight);
	struct Note* notesLeft = (struct Note*)&defaultJingleBytes[offset + 4 + numNotesRight * 6];
	consolePrint("notesRight = 0x%08x\n", notesLeft);

	for (int idx = 0; idx < numNotesRight; idx++) {
		consolePrint("Note[%d] = 0x%04x (0x%08x), 0x%04x (0x%08x), 0x%04x (0x%08x)\n", idx,
			notesRight[idx].dutyCycle, &notesRight[idx].dutyCycle,
			notesRight[idx].pulseFreq, &notesRight[idx].pulseFreq,
			notesRight[idx].duration, &notesRight[idx].duration);
	}
	for (int idx = 0; idx < numNotesLeft; idx++) {
		consolePrint("Note[%d] = 0x%04x (0x%08x), 0x%04x (0x%08x), 0x%04x (0x%08x)\n", idx,
			notesLeft[idx].dutyCycle, &notesLeft[idx].dutyCycle,
			notesLeft[idx].pulseFreq, &notesLeft[idx].pulseFreq,
			notesLeft[idx].duration, &notesLeft[idx].duration);
	}

	playHaptic(R_HAPTIC, notesRight, numNotesRight);
	playHaptic(L_HAPTIC, notesLeft, numNotesLeft);

	return 0;
}

/**
 * Initiate playing a sequence of notes via a particular haptic.
 *
 * Note: The notes buffer must persist until the sequence is finished playing
 *	(which will be after this returns). Don't put this on the stack!
//TODO: It is dangerous that buffer must persist beyond function exit... Figure out a better way or split into two functions (i.e. add one that copies)!
 * 
 * \param haptic Defines which haptic is being referred to.
 * \param[in] notes Buffer containing a sequence of notes to be played.
 * \param numNotes The number of notes in the notes buffer.
 *
 * \return 0 on sucess.
 */
int playHaptic(enum Haptic haptic, const struct Note* notes, uint32_t numNotes) {
	if (!notes) {
		return -1;
	}

	if (hapticBusy[haptic]) {
		return -2;
	}

	if (!numNotes) {
		return -3;
	}

	hapticNotes[haptic] = notes;
	hapticNotesIdx[haptic] = 0;
	hapticNotesLen[haptic] = numNotes;

	Chip_TIMER_ClearMatch(hapticTimer, getHapticMR(haptic));

	startHapticNote(haptic, notes);

	//TODO: need to protect this with locking mechanism?
	Chip_TIMER_MatchEnableInt(hapticTimer, getHapticMR(haptic));

	return 0;
}

/**
 * Prints details to console regarding how to use the haptic command line 
 *  function.
 *
 * \return None.
 */
void hapticCmdUsage(void) {
	//TODO
}

/**
 * Handle Haptic control command line function.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int hapticCmdFnc(int argc, const char* argv[]) {
	//TODO: do this right

	if (argc != 2) {

		consolePrint("Wrong number args!\n");
		
		return -1;
	}

	consolePrint("ct32b0_cnt = %d\n", ct32b0_cnt);
	consolePrint("IR = 0x%08x\n", hapticTimer->IR);
	consolePrint("TCR = 0x%08x\n", hapticTimer->TCR);
	consolePrint("TC = 0x%08x\n", hapticTimer->TC);
	consolePrint("PR = 0x%08x\n", hapticTimer->PR);
	consolePrint("PC = 0x%08x\n", hapticTimer->PC);
	consolePrint("MCR = 0x%08x\n", hapticTimer->MCR);
	consolePrint("MR[0] = 0x%08x\n", hapticTimer->MR[0]);
	consolePrint("MR[1] = 0x%08x\n", hapticTimer->MR[1]);
	consolePrint("MR[2] = 0x%08x\n", hapticTimer->MR[2]);
	consolePrint("MR[3] = 0x%08x\n", hapticTimer->MR[3]);
	consolePrint("%d\n\n", SystemCoreClock);

	int jingleNum = strtol(argv[1], NULL, 0);

	playDefaultJingle(jingleNum);

	return 0;


/*
	static Note note;

	if (argc != 4) {

		consolePrint("Wrong number args!\n");
		
		return -1;
	}

	consolePrint("%d %d 0x%x %d %d %d\n", itHappened, 
		hapticBusy[R_HAPTIC], 
		hapticNotes[R_HAPTIC], hapticRepeatCnt[R_HAPTIC], 
		hapticNotesIdx[R_HAPTIC], hapticNotesLen[R_HAPTIC]);
	consolePrint("IR = 0x%08x\n", hapticTimer->IR);
	consolePrint("TCR = 0x%08x\n", hapticTimer->TCR);
	consolePrint("TC = 0x%08x\n", hapticTimer->TC);
	consolePrint("PR = 0x%08x\n", hapticTimer->PR);
	consolePrint("PC = 0x%08x\n", hapticTimer->PC);
	consolePrint("MCR = 0x%08x\n", hapticTimer->MCR);
	consolePrint("MR[0] = 0x%08x\n", hapticTimer->MR[0]);
	consolePrint("MR[1] = 0x%08x\n", hapticTimer->MR[1]);
	consolePrint("MR[2] = 0x%08x\n", hapticTimer->MR[2]);
	consolePrint("MR[3] = 0x%08x\n", hapticTimer->MR[3]);
	consolePrint("%d\n", SystemCoreClock);

	uint32_t hiDur = strtol(argv[1], NULL, 0);
	uint32_t loDur = strtol(argv[2], NULL, 0);
	uint32_t repeatCnt = strtol(argv[3], NULL, 0);

	note.highDuration = hiDur;
	note.lowDuration = loDur;
	note.repeatCnt = repeatCnt;

	playHaptic(R_HAPTIC, &note, 1);

	return 0;
*/
}
