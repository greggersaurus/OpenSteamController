/**
 * \file jingle_data.c
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

#include "jingle_data.h"

#include "eeprom_access.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define JINGLE_DATA_MAX_BYTES (0x400)

#define JINGLE_DATA_EEPROM_OFFSET (0x800)

static const uint16_t JD_MAGIC_WORD = 0xbead; //!< First 16 bits of Jingle
	//!< Data blob. Preliminary check to verify is blob is valid.

static uint16_t numJingleBytesFree = 0; //!< Updated when adding/removing
	//!< jingle from data blob. Defines how much space is left in
	//!< the Jingle Data blob for adding more Jingles.

/**
 * This is the jingle data in store in raw form. This makes for easier 
 *  initialization and loading/saving from/to EEPROM, etc.
 *
 * Use setJingleData16()/getJingleData16() to access 16-bit words in this blob.
 *
 * Note that the default initialization values were taken from simulation of
 *  official Steam Controller firmware version 
 *  vcf_wired_controller_d0g_57bf5c10.bin. This data was obtained from 
 *  0x10001200 - 0x100015ff. Note that this data range was filled from 
 *  0x100003b8 - 0x100007b7, which is filled with default Jingle Data read from
 *  flash. This default data exists in flash starting at 0x00010177, however,
 *  the data was packed into flash in a format that does not match how it ends
 *  up in RAM (or EEPROM). Note that 0x800 can store Jingle Data, however, this
 *  looks like a feature Valve did not add support for.
 *
 * Regarding the format of this raw data:
 *	Header {
 *	 byte[0] and byte[1] form a uint16_t that is a magic word (0xbead).
 *	  This is used to check if EEPROM 0x800 has valid Jingle Data.
 *	 byte[4] is the number of Jingles in the data blob
 *	}
 *	byte[6] and bytes[7] form a uint16_t that is the byte offset from the
 *	  beginning of the data blob to Jingle 0 (assuming there is a jingle 0)
 *	This is followed by uint16_t values that are offsets for each of the
 *	 valid Jingles.
 *
 *	A Jingle starts off with a uint16_t for the number of notes in the
 *	 sequence played on the Right Haptic.
 *	The next two bytes are a uint16_t for the number of notes in the
 *	 sequence played on the Left Haptic.
 *	Next comes the sequence of Notes for the Right Haptic, immediately
 *	 followed by the Notes for the Left Haptic. 
 */
static uint32_t rawJingleData32[JINGLE_DATA_MAX_BYTES/sizeof(uint32_t)] = {
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

static uint8_t* rawJingleData = (uint8_t*)rawJingleData32; //!< Granular access
	//!< to Jingle Data.

/**
 * Read 16-bit words from Jingle Data blob.
 *
 * \param offset Byte offset from start of Jingle Data blob.
 * 
 * \return Byte value at given offset.
 */
static uint16_t getJingleData16(uint16_t offset) {
	if (offset >= JINGLE_DATA_MAX_BYTES-1)
		return 0;

	return *(uint16_t*)&rawJingleData[offset];
}

/**
 * Write 16-bit words to Jingle Data blob.
 *
 * \param offset Byte offset from start of Jingle Data blob.
 * \param data value to write to Jingle Data blob.
 * 
 * \return 0 on success.
 */
static int setJingleData16(uint16_t offset, uint16_t data) {
	if (offset >= JINGLE_DATA_MAX_BYTES-1)
		return -1;

	uint16_t* ptr = (uint16_t*)&rawJingleData[offset];

	*ptr = data;

	return 0;
}

/**
 * Read the Magic Word field of the Jingle Data blob Header portion.
 *
 * \return The Magic Word. Will be JD_MAGIC_WORD if blob contains valid Jingle 
 *	Data.
 */
static uint16_t getMagicWord(void) {
	return *((uint16_t*)rawJingleData);
}

/**
 * Write the Magic Word field of the Jingle Data blob Header portion.
 *
 * \param magicWord The Magic Word. Should be JD_MAGIC_WORD to indicate the
 *	Jingle Data blob contains valid Jingle Data.
 * 
 * \return None.
 */
static void setMagicWord(uint16_t magicWord) {
	*((uint16_t*)rawJingleData) = magicWord;
}

/**
 * \return The number of jingles as specified by the Jingle Data.
 */
uint8_t getNumJingles(void) {
	return rawJingleData[4];
}

/**
 * Set how many Jingles are stored in the Jingle Data.
 *
 * \param numJingles The number of jingles in the Jingle Data. This is used to
 *	index to the Jingle Data offset values, etc. So make sure the data this
 *	implies is there, is actually there and within the Jingle Data bounds.
 *
 * \return None.
 */
static void setNumJingles(uint8_t numJingles) {
	rawJingleData[4] = numJingles;
}

/**
 * \return True if the Jingle Data blob seems to contain valid data.
 */
static bool jingleDataIsValid() {
	if (getMagicWord() != JD_MAGIC_WORD) {
		return false;
	}

	//TODO: Check that offsets are incrementing

	//TODO: validate location of each jingle

	return true;
}

/**
 * Initialize Jingle Data to known empty and valid state.
 *
 * \return None.
 */
static void initJingleData(void) {
	setMagicWord(JD_MAGIC_WORD);
	setNumJingles(0);
	// Leave room for header (i.e. Magic Word, Number of Jingles, packing
	//  and alignment bytes).
	numJingleBytesFree = JINGLE_DATA_MAX_BYTES - 6;
}

/**
 * \param idx Index of the Jingle begin referred to.
 * 
 * \return 0 on error, or the byte offset from the start of the Jingle Data to 
 *	where the Jingle related memory starts on success.
 */
static uint16_t getJingleOffset(uint8_t idx) {
	if (idx >= getNumJingles()) {
		return 0;
	}
	uint16_t* jingleOffsets = (uint16_t*)&rawJingleData[6];
	if (jingleOffsets[idx] >= JINGLE_DATA_MAX_BYTES) {
		return 0;
	}
	return jingleOffsets[idx];
}

/**
 * \param haptic Specifies which haptic is being referred to.
 * \param idx Index of the Jingle being referred to.
 *
 * \return The number of notes for the specified Channel for the specified
 *	Jingle.
 */
static uint16_t getNumJingleNotes(enum Haptic haptic, uint8_t idx) {
	uint16_t offset = getJingleOffset(idx);
	if (!offset)
		return 0;
	if (haptic == L_HAPTIC) 
		offset += sizeof(uint16_t);
	return getJingleData16(offset);
}

/**
 * \param haptic Specifies which haptic is being referred to.
 * \param idx Index of the Jingle being referred to.
 *
 * \return Pointer to the notes for the specified Channel for the specified  
 *	Jingle or NULL on error.
 */
static Note* getJingleNotes(enum Haptic haptic, uint8_t idx) {
	uint16_t offset = getJingleOffset(idx);
	if (!offset)
		return NULL;
	offset += 2 * sizeof(uint16_t);
	if (haptic == L_HAPTIC)
		offset += getNumJingleNotes(R_HAPTIC, idx) * sizeof(Note);
	return (Note*)&rawJingleData[offset];
}

//TODO
static int addJingle(uint8_t idx, uint16_t numNotesRight, uint16_t numNotesLeft) {
	// check if there is enough room in blob for another Jingle
	// add offset for new Jingle
	//	move all jingle data by 2 bytes
	//	adjust all jingle offsets
	// Set number of notes for right and left jingle
	// Increment number of Jingles
	// Clear notes
	// update number of free bytes
	return -1;
}	

//TODO
static int delJingle(uint8_t idx) {
	// decrement number of jingles
	// move all jingle data down by 2 bytes
	// adjust all remaining offsets
	// Compute/adjust how much room is left in blob
	return -1;
}

/**
 * Print information on Jingle Data Blob.
 * 
 * \return 0 on success.
 */
static int printJingleData(void) {
	printf("Magic Word = 0x%04x\n", getMagicWord());
	printf("Number of Jingles = %d\n", getNumJingles());
	for (int idx = 0; idx < getNumJingles(); idx++) {
		printf("Jingle[0] offset = 0x%03x\n", getJingleOffset(idx));
	}
	printf("Bytes free in blob = 0x%03x\n", numJingleBytesFree);
	return 0;
}

/**
 * Print to console details on a particular Jingle.
 *
 * \param idx Indicates which Jingle is being referred to.
 * 
 * \return 0 on success.
 */
static int printJingle(uint8_t idx) {
	if (idx >= getNumJingles()) {
		printf("jingleNum %d too large\n", idx);
		return -1;
	}

	uint16_t offset = getJingleOffset(idx);
	if (!offset) {
		printf("Invalid offset (0x%04x)\n", offset);
		return -1;
	}

	printf("offset = 0x%03x\n", offset);
	uint16_t numNotesRight = getNumJingleNotes(R_HAPTIC, idx);
	printf("numNotesRight = %d\n", numNotesRight);
	uint16_t numNotesLeft = getNumJingleNotes(L_HAPTIC, idx);
	printf("numNotesLeft = %d\n", numNotesLeft);
	struct Note* notesRight = getJingleNotes(R_HAPTIC, idx);
	printf("notesRight = 0x%08x\n", notesRight);
	struct Note* notesLeft = getJingleNotes(L_HAPTIC, idx);
	printf("notesLeft = 0x%08x\n", notesLeft);

	for (int idx = 0; idx < numNotesRight; idx++) {
		if (!notesRight)
			break;

		printf("Note[%d] = 0x%04x, 0x%04x, 0x%04x\n", idx,
			notesRight[idx].dutyCycle, 
			notesRight[idx].pulseFreq, 
			notesRight[idx].duration);
	}
	for (int idx = 0; idx < numNotesLeft; idx++) {
		if (!notesLeft)
			break;

		printf("Note[%d] = 0x%04x, 0x%04x, 0x%04x\n", idx,
			notesLeft[idx].dutyCycle,
			notesLeft[idx].pulseFreq,
			notesLeft[idx].duration);
	}

	return 0;
}

/**
 * Play a specified Jingle using the haptics.
 * 
 * \param idx Indicates which Jingle is being referred to. 
 * 
 * \return 0 on success.
 */
int playJingle(uint8_t idx) {
	if (idx >= getNumJingles())
		return -1;

	uint16_t offset = getJingleOffset(idx);
	if (!offset)
		return -1;

	uint16_t numNotesRight = getNumJingleNotes(R_HAPTIC, idx);
	uint16_t numNotesLeft = getNumJingleNotes(L_HAPTIC, idx);
	struct Note* notesRight = getJingleNotes(R_HAPTIC, idx);
	struct Note* notesLeft = getJingleNotes(L_HAPTIC, idx);

	if (numNotesRight && notesRight)
		playHaptic(R_HAPTIC, notesRight, numNotesRight);
	if (numNotesLeft && notesLeft)
		playHaptic(L_HAPTIC, notesLeft, numNotesLeft);

	return 0;
}

/**
 * Load Jingle Data from EEPROM. This will attempt to replace Jingle Data with 
 *  data from EEPROM. If EEPROM data is invalid, local Jingle Data will be
 *  cleared.
 * 
 * \return 0 on success.
 */
static int loadJingleEEPROM(void) {
	int retval = eepromRead(JINGLE_DATA_EEPROM_OFFSET, rawJingleData, 
		JINGLE_DATA_MAX_BYTES);

	if (retval) {
		initJingleData();
		return -1;
	}

	if (!jingleDataIsValid()) {
		initJingleData();
		return -2;
	}

	return 0;
}

/**
 * Save Jingle Data to EEPROM. This will write current Jingle Data to EEPROM. 
 *  Note: This will PERSIST even after updating firmware and may affect how the
 *  official firmware functions. Use \"clear\" to erase.
 * 
 * \return 0 on success.
 */
static int saveJingleEEPROM(void) {
	int retval = eepromWrite(JINGLE_DATA_EEPROM_OFFSET, rawJingleData, 
		JINGLE_DATA_MAX_BYTES);
	if (retval) {
		return -1;
	}

	return 0;
}

/**
 * Clear out Jingle Data saved to EEPROM. This will cause official firmware to
 *   use default Jingle Data embedded in firmware.
 * 
 * \return 0 on success.
 */
static int clearJingleEEPROM(void) {
	uint16_t empty_word = 0;
	// Clear out header bytes for Jingle Data blob
	int retval = eepromWrite(JINGLE_DATA_EEPROM_OFFSET, &empty_word, 
		sizeof(empty_word));
	if (retval) {
		return -1;
	}
	retval = eepromWrite(JINGLE_DATA_EEPROM_OFFSET+2, &empty_word, 
		sizeof(empty_word));
	if (retval) {
		return -2;
	}
	retval = eepromWrite(JINGLE_DATA_EEPROM_OFFSET+4, &empty_word, 
		sizeof(empty_word));
	if (retval) {
		return -3;
	}

	return 0;
}

/**
 * Prints details to console regarding how to use the haptic command line 
 *  function.
 *
 * \return None.
 */
void jingleCmdUsage(void) {
	printf(
		"usage: jingle play {jingleIdx}\n"
		"       jingle print [{jingleIdx}]\n"
		"       jingle clear\n"
		"       jingle delete {jingleIdx}\n"
		"       jingle add {jingleIdx} {numNotesRight} {numNotesLeft}\n"
		"       jingle note {jingleIdx} {hapticId} {notdeIdx} {dutyCycle} {freq} {dur}\n"
		"       jingle eeprom {cmd}\n"
		"\n"
		"play = play the jingle associated with the given jingleIdx\n"
		"print = Print info on all the jingles, or details on the notes\n"
		"       for a jingle associated with a given jingleIdx\n"
		"delete = Delete Jingle associated with the given jingleIdx\n"
		"clear = Initialize the Jingle Data structure to have 0 Jingles\n"
		"add = Add a new jingle with the specified number of notes\n"
		"	for each channel (i.e. haptic)\n"
		"note = Change a particular note in a particular jingle\n"
		"	See \"haptic\" command for parameter details\n"
		"eeprom = Allows access to EEPROM where custom Jingle Data\n"
		"	can persist, even if firmware is updated.\n"
		"	Below are descriptions of supported commands:\n"
		" 	\"load\" Load Jingle Data from EEPROM. This will \n"
		"	 attempt to replace Jingle Data with data from EEPROM.\n"
		"	 If EEPROM data is invalid, local Jingle Data will be\n"
		"	 cleared.\n"
		"	\"save\"  Save Jingle Data to EEPROM. This will write\n"
		"	 current Jingle Data to EEPROM. Note: This will PERSIST\n"
		"	 even after updating firmware and may affect how the\n"
		"	 official firmware functions. Use \"clear\" to erase\n"
		"	\"clear\" Clear out Jingle Data saved to EEPROM. This\n"
		"	 will cause official firmware to use default Jingle\n"
		"	 Data embedded in firmware.\n"
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
int jingleCmdFnc(int argc, const char* argv[]) {
	uint32_t jingle_idx = 0;
	int retval = 0;
	if (argc < 2) {
		jingleCmdUsage();
		return -1;
	}

	if (!strcmp("play", argv[1])) {
		if (argc != 3) {
			jingleCmdUsage();
			return -1;
		}

		jingle_idx = strtol(argv[2], NULL, 0);
		if (jingle_idx > 255) {
			printf("jingleIdx must bein range 0 to 255\n");
			return -1;
		}
		if (jingle_idx >= getNumJingles()) {
			printf("Only %d jingles available\n", 
				getNumJingles());
			return -1;
		}
		retval = playJingle(jingle_idx);
		if (retval) {
			printf("Error playing Jingle (err = %d)\n", retval);
			return -1;
		}
	} else if (!strcmp("print", argv[1])) {
		if (argc == 2) {
			printJingleData();	
		} else if (argc == 3) {
			jingle_idx = strtol(argv[2], NULL, 0);
			if (jingle_idx > 255) {
				printf("jingleIdx must bein range 0 to 255\n");
				return -1;
			}
			if (jingle_idx >= getNumJingles()) {
				printf("Only %d jingles available\n", 
					getNumJingles());
				return -1;
			}
			retval = printJingle(jingle_idx);
			if (retval) {
				printf("Error pringting Jingle (err = %d)\n", retval);
				return -1;
			}
		} else { 
			jingleCmdUsage();
			return -1;
		}
	} else if (!strcmp("clear", argv[1])) {
		if (argc != 2) {
			jingleCmdUsage();
			return -1;
		}
		initJingleData();
	} else if (!strcmp("delete", argv[1])) {
		if (argc != 3) {
			jingleCmdUsage();
			return -1;
		}

		jingle_idx = strtol(argv[2], NULL, 0);
		if (jingle_idx > 255) {
			printf("jingleIdx must bein range 0 to 255\n");
			return -1;
		}
		if (jingle_idx >= getNumJingles()) {
			printf("Only %d jingles available\n", 
				getNumJingles());
			return -1;
		}
		retval = delJingle(jingle_idx);
		if (retval) {
			printf("Error deleting Jingle (err = %d)\n", retval);
			return -1;
		}
	} else if (!strcmp("add", argv[1])) {
		if (argc != 5) {
			jingleCmdUsage();
			return -1;
		}

		jingle_idx = strtol(argv[2], NULL, 0);
		if (jingle_idx > 255) {
			printf("jingleIdx must bein range 0 to 255\n");
			return -1;
		}
		if (jingle_idx >= getNumJingles()) {
			printf("Only %d jingles available\n", 
				getNumJingles());
			return -1;
		}
		uint32_t num_notes_right = strtol(argv[3], NULL, 0);
		if (num_notes_right > 65535) {
			printf("numNotesRight must be in range 0 to 65535\n");
			return -1;
		}
		uint32_t num_notes_left = strtol(argv[4], NULL, 0);
		if (num_notes_left > 65535) {
			printf("numNotesLeft must be in range 0 to 65535\n");
			return -1;
		}
		retval = addJingle(jingle_idx, num_notes_right, num_notes_left);
		if (retval) {
			printf("Error adding Jingle (err = %d)\n", retval);
			return -1;
		}
	} else if (!strcmp("note", argv[1])) {
		if (argc != 8) {
			jingleCmdUsage();
			return -1;
		}

		jingle_idx = strtol(argv[2], NULL, 0);
		if (jingle_idx > 255) {
			printf("jingleIdx must bein range 0 to 255\n");
			return -1;
		}
		if (jingle_idx >= getNumJingles()) {
			printf("Only %d jingles available\n", 
				getNumJingles());
			return -1;
		}

		Haptic hapticId = L_HAPTIC;
		if (!strcmp(argv[3], "left")) {
			hapticId = L_HAPTIC;
		} else if (!strcmp(argv[3], "right")) {
			hapticId = R_HAPTIC;
		} else {
			printf("Invalid hapticId of \'%s\'\n", argv[3]);
			return -1;
		}

		uint32_t note_idx = strtol(argv[4], NULL, 0);
		uint32_t duty_cycle = strtol(argv[5], NULL, 0);
		if (duty_cycle > 255) {
			printf("dutyCycle must be in range 0 to 255\n");
			return -1;
		}
		uint32_t frequency = strtol(argv[6], NULL, 0);
		if (frequency > 65535) {
			printf("frequency must be in range 0 to 65535\n");
			return -1;
		}
		uint32_t duration = strtol(argv[7], NULL, 0);
		if (duration > 65535) {
			printf("duration must be in range 0 to 65535\n");
			return -1;
		}

		uint16_t num_notes = getNumJingleNotes(hapticId, jingle_idx);
		if (note_idx >= num_notes) {
			printf("Invalid noteIdx of %d. Only %d notes for given channel\n", 
				note_idx, num_notes);
			return -1;
		}
		
		Note* notes = getJingleNotes(hapticId, jingle_idx);
		if (!notes) {
			printf("Error getting notes.\n");
			return -1;
		}

		notes[note_idx].dutyCycle = duty_cycle;
		notes[note_idx].pulseFreq = frequency;
		notes[note_idx].duration = duration;

		printf("Note updated successfully.\n");
	} else if (!strcmp("eeprom", argv[1])) {
		if (argc != 3) {
			jingleCmdUsage();
			return -1;
		}

		if (!strcmp("load", argv[2])) {
			retval = loadJingleEEPROM();
			if (retval) {
				printf("Loading from EEPROM failed (err = %d).\n",
					retval);
				return -1;
			}
			printf("Load complete\n");
		} else if (!strcmp("save", argv[2])) {
			retval = saveJingleEEPROM();
			if (retval) {
				printf("Saving to EEPROM failed (err = %d).\n",
					retval);
				return -1;
			}
			printf("Save complete\n");
		} else if (!strcmp("clear", argv[2])) {
			retval = clearJingleEEPROM();
			if (retval) {
				printf("Clearing EEPROM failed (err = %d).\n",
					retval);
				return -1;
			}
			printf("Clear complete\n");
		} else {
			jingleCmdUsage();
			return -1;
		}
	} else {
		jingleCmdUsage();
		return -1;
	}

	return 0;
}
