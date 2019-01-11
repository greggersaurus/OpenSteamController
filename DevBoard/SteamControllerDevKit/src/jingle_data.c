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

#include <stdlib.h>
#include <string.h>

#include "console.h"

#define JINGLE_DATA_MAX_BYTES (0x400)

/**
 * This is the jingle data in store in raw form. This makes for easier 
 *  initialization and loading/saving from/to EEPROM, etc.
 *
 * Use getJingleData8/16 and setJingleData8/16 to access this data.
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
 *	byte[0] and byte[1] form a uint16_t that is a magic word (0xbead).
 *	  This is used to check if EEPROM 0x800 has valid Jingle Data.
 *	byte[4] is the number of Jingles in the data blob
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

static uint8_t* rawJingleData = rawJingleData32;

static const uint16_t JD_MAGIC_WORD = 0xbead; //!< First 16 bits of Jingle
	//!< Data blob. Preliminary check to verify is blob is valid.

static uint16_t numJingleBytesFree = 0; //!< Updated when adding/removing
	//!< jingle from data blob. 

/**
 * \param offset Byte offset from start of Jingle Data blob.
 * 
 * \return Byte value at given offset.
 */
static uint8_t getJingleData8(uint16_t offset) {
	if (offset >= JINGLE_DATA_MAX_BYTES)
		return 0;

	return rawJingleData[offset];
}

/**
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
 * \param offset Byte offset from start of Jingle Data blob.
 * \param data value to write to Jingle Data blob.
 * 
 * \return 0 on success.
 */
static int setJingleData8(uint16_t offset, uint8_t data) {
	if (offset >= JINGLE_DATA_MAX_BYTES)
		return -1;

	rawJingleData[offset] = data;
	
	return 0;
}

/**
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
 * \return The number of jingles 
 */
uint8_t getNumJingles() {
	return rawJingleData[4];
}

uint16_t getJingleOffset(uint8_t idx) {
	uint16_t* jingleOffsets = (uint16_t*)&rawJingleData[6];
	return jingleOffsets[idx];
}

uint16_t getNumJingleNotes(enum Haptic haptic, uint8_t idx) {
	uint16_t offset = getJingleOffset(idx);
	if (haptic == L_HAPTIC) 
		offset += sizeof(uint16_t);
	return getJingleData16(offset);
}

Note* getJingleNotes(enum Haptic haptic, uint8_t idx) {
	uint16_t offset = getJingleOffset(idx) + 2 * sizeof(uint16_t);
	if (haptic == L_HAPTIC)
		offset += getNumJingleNotes(R_HAPTIC, idx) * sizeof(Note);
	return (Note*)&rawJingleData[offset];
}

uint16_t getNumJingleBytesFree() {
	return numJingleBytesFree;
}

int addJingle(uint16_t numNotesRight, uint16_t numNotesLeft) {
	return -1;
}	

int delJingle(uint8_t idx) {
	return -1;
}

/**
 * \return True if the Jingle Data blob seems to contain valid data.
 */
bool jingleDataIsValid() {

//TODO: sanity check offsets are incrementing and match number of jingles, etc.

	return false;
}

int playJingle(uint8_t idx) {
	if (idx >= getNumJingles()) {
		consolePrint("jingleNum %d too large\n", idx);
		return -1;
	}

	consolePrint("sizeof(Note) = %d\n", sizeof(Note));

	uint16_t offset = getJingleOffset(idx);
	consolePrint("offset = 0x%03x\n", offset);
	uint16_t numNotesRight = getNumJingleNotes(R_HAPTIC, idx);
	consolePrint("numNotesRight = %d\n", numNotesRight);
	uint16_t numNotesLeft = getNumJingleNotes(L_HAPTIC, idx);
	consolePrint("numNotesLeft = %d\n", numNotesLeft);
	struct Note* notesRight = getJingleNotes(R_HAPTIC, idx);
	consolePrint("notesRight = 0x%08x\n", notesRight);
	struct Note* notesLeft = getJingleNotes(L_HAPTIC, idx);
	consolePrint("notesLeft = 0x%08x\n", notesLeft);

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

int loadJingleEEPROM() {
	return -1;
}

int saveJingleEEPROM() {
	return -1;
}

void jingleCmdUsage(void) {

}

int jingleCmdFnc(int argc, const char* argv[]) {
	if (argc != 2) {
		consolePrint("# args needs to be 2\n");
		return -1;
	}

	playJingle(strtol(argv[1], NULL, 0));

	return 0;
}
