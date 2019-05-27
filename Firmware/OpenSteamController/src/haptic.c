/**
 * \file haptic.c
 * \brief Encompasses functions for interfacing with haptics.
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

#include "haptic.h"

#include "lpc_types.h"
#include "chip.h"
#include "timer_11xx.h"

#define GPIO_HAPTICS_EN_N 1, 7
#define GPIO_HAPTICS_L 0, 18
#define GPIO_HAPTICS_R 1, 12

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static LPC_TIMER_T* hapticTimer = LPC_TIMER32_0; //!< Timer used to toggle 
	//!< GPIO that make haptics react.

static volatile bool hapticBusy[2]; //!< non-zero if the haptic is currently 
	//!< playing a sequence.
static const Note* hapticNotes[2]; //!< Pointer to the first note in a 
 	//!< sequence to play on the haptics.
static int hapticNotesIdx[2]; //!< Index of which note is currently
	//!< being played on a haptic.
static int hapticNotesLen[2]; //!< The number of notes in a sequence 
	//!< to be played on a haptic.
static uint32_t pulseHiDur[2]; //!< Number of microseconds for which
	//!< Haptic GPIO is high for pulse being generated for current Note.
static uint32_t pulseLoDur[2]; //!< Number of microseconds for which
	//!< Haptic GPIO is low for pulse being generated for current Note.
static uint32_t pulseRptCntr[2]; //!< Down counter for the number of
	//!< of times to geneate the pulse sequence.
static uint32_t nextMR[2]; //!< Calculated value of what MR should be set to
	//!< so that next IRQ occurs to change GPIO states to produce desired
	//!< frequency. 

static const uint8_t SLEEP_MR = 3; //!< MR used for sleep functionality.
static volatile bool sleepDoneHaptic = true; //!< Flag used to indicate 
	//!< requested sleep is complete (i.e. IRQ has fired).

/**
 * \param haptic Defines which haptic we are referring too. 
 *
 * \return the Match Register number for the corresponding haptic.
 */
inline static int8_t getHapticMR(enum Haptic haptic) {
	return (int8_t)haptic + 1;
}

/**
 * Change state of GPIO that controls haptic.
 *
 * \param haptic Defines which haptic we are referring too. 
 * \param setting True to drive GPIO high. False to drive low.
 *
 * \return None.
 */
inline static void setHapticGpioState(enum Haptic haptic, bool setting) {
	if (haptic == R_HAPTIC) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_R, setting);
	}
	if (haptic == L_HAPTIC) {
		Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_L, setting);
	}
}

/**
 * \param haptic Defines which haptic we are referring too. 
 *
 * \return Get state of GPIO that controls haptic.
 */
inline static bool getHapticGpioState(enum Haptic haptic) {
	if (haptic == R_HAPTIC) {
		return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_HAPTICS_R);
	}
	if (haptic == L_HAPTIC) {
		return Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_HAPTICS_L);
	}

	return false;
}
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
		// This Note is just a delay, so keep GPIO low
		setHapticGpioState(haptic, false);

		pulseHiDur[haptic] = 0;
		pulseLoDur[haptic] = 0;
		pulseRptCntr[haptic] = 1;

		// Setup interrupt to occur after full delay
		nextMR[haptic] = note->duration * 1000 + 
			Chip_TIMER_ReadCount(hapticTimer);
		hapticTimer->MR[getHapticMR(haptic)] = nextMR[haptic]; 
	} else {
		uint32_t pulse_width = 1000000 / note->pulseFreq;

		pulseHiDur[haptic] = (pulse_width * note->dutyCycle) / 512;
		pulseLoDur[haptic] = pulse_width - pulseHiDur[haptic];
		pulseRptCntr[haptic] = (note->duration * note->pulseFreq) / 1000;

		// Start with haptic GPIO in high state
		setHapticGpioState(haptic, true);

		// Setup interrupt to occur when high portion is done
		nextMR[haptic] = pulseHiDur[haptic] + 
			Chip_TIMER_ReadCount(hapticTimer);
		hapticTimer->MR[getHapticMR(haptic)] = nextMR[haptic]; 
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
		nextMR[haptic] += pulseLoDur[haptic];
		hapticTimer->MR[getHapticMR(haptic)] = nextMR[haptic];
	} else {
		// Pulse low finished (i.e. iteration of pulse is complete)
		pulseRptCntr[haptic]--;

		// Check if another pulse is to be generated for this Note
		if (pulseRptCntr[haptic]) {
			// If this is last repeat count stay low to create a
			//  distinct break between notes
			if (pulseRptCntr[haptic] > 1) {
				// Start another high portion of pulse
				setHapticGpioState(haptic, true);

				// Setup interrupt to occur when high portion is done
				nextMR[haptic] += pulseHiDur[haptic];
				hapticTimer->MR[getHapticMR(haptic)] = 
					nextMR[haptic];
			} else {
				// Since we are not changing GPIO state, we 
				//  do not want to come back to ISR until
				//  a full period has elapsed
				nextMR[haptic] += pulseHiDur[haptic] + 
					pulseLoDur[haptic];
				hapticTimer->MR[getHapticMR(haptic)] = 
					nextMR[haptic];
			}
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

/**
 * Interrupt handler for CT32B0. Used to toggle Haptics GPIOs based on struct
 *   Note speciications.
 *
 * \return None.
 */
void TIMER32_0_IRQHandler(void) {
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

	// Check if interrupt was generated by sleep request
	if (Chip_TIMER_MatchPending(hapticTimer, SLEEP_MR)) {
		Chip_TIMER_ClearMatch(hapticTimer, SLEEP_MR);
		Chip_TIMER_MatchDisableInt(hapticTimer, SLEEP_MR);
		sleepDoneHaptic = true;
	}
}

/**
 * Initialization that needs to happen for haptics to work.
 * 
 * \return None.
 */
void initHaptics(void) {
	// GPIO to enable haptics
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_EN_N, true);
	Chip_GPIO_WriteDirBit(LPC_GPIO, GPIO_HAPTICS_EN_N, true);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_HAPTICS_EN_N, IOCON_MODE_PULLDOWN, 
		IOCON_FUNC0);

	// GPIO to cause left haptic to react
	Chip_GPIO_WritePortBit(LPC_GPIO, GPIO_HAPTICS_L, false);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, GPIO_HAPTICS_L);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_HAPTICS_L, IOCON_DIGMODE_EN, 
		IOCON_FUNC0);

	// GPIO to cause right haptic to react
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
 * Sleep for the specific number of microseconds.
 * 
 * \param usec The number of microseconds to sleep for.
 * 
 * \return None.
 */
void usleepHaptic(uint32_t usec) {
	// Reset the flag for indicating we have slept the desired amout of time
	sleepDoneHaptic = false;

	// Setup MR to generate interrupt when sleep is complete
	hapticTimer->MR[SLEEP_MR] = Chip_TIMER_ReadCount(hapticTimer) + usec;
	Chip_TIMER_MatchEnableInt(hapticTimer, SLEEP_MR);

	while (!sleepDoneHaptic) {
		__WFI();
	}
}

/**
 * Get the current value of a timer running with usec precision.
 * 
 * \return The count value for a timer configured where the count increments
 *	each usec. 
 */
uint32_t getUsTickCntHaptic(void) {
	return Chip_TIMER_ReadCount(hapticTimer);
}

/**
 * Prints details to console regarding how to use the haptic command line 
 *  function.
 *
 * \return None.
 */
void hapticCmdUsage(void) {
	printf(
		"usage: haptic {hapticId} {dutyCycle} {frequency} {duration}\n"
		"\n"
		"hapticId = \"right\" or \"left\" to specify which haptic\n"
		"dutyCycle = 0-255 for percentage pulse should be in high state\n"
		"frequency = Frequency of pulse to generate in Hz\n"
		"duration = Duration of repeated pulse in ms\n"
	);
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
	static struct Note note = {0, 0, 0, 0};

	if (argc != 5) {
		hapticCmdUsage();
		
		return -1;
	}

	enum Haptic haptic = R_HAPTIC;
//TODO: input as uint32_t, then check range...
	note.dutyCycle = strtol(argv[2], NULL, 0);
	note.pulseFreq = strtol(argv[3], NULL, 0);
	note.duration = strtol(argv[4], NULL, 0);

	if (note.dutyCycle < 0 || note.dutyCycle > 255) {
		printf("dutyCycle outside range 0-255\n");
		return -1;
	}
	if (note.pulseFreq < 0 || note.pulseFreq > 65535) {
		printf("frequency outside range 0-65535\n");
		return -1;
	}
	if (note.duration < 0 || note.duration > 65535) {
		printf("duration outside range 0-65535\n");
		return -1;
	}

	if (!strcmp("right", argv[1])) {
		haptic = R_HAPTIC;
	} else if (!strcmp("left", argv[1])) {
		haptic = L_HAPTIC;
	} else {
		printf("haptId is not \"right\" or \"left\"\n");
		return -1;
	}

	int retval = playHaptic(haptic, &note, 1);
	if (retval) {
		printf("Failed to play note (error = %d)\n", retval);
		return -1;
	}

	return 0;
}
