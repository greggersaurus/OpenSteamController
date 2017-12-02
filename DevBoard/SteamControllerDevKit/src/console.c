/**
 * \file console.c
 * \brief Encompasses functions for interacting with console (i.e. displaying
 *	text and reacting to user input).
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

#include "console.h"

#include "usb.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define HISTORY_SIZE (16) // Number of entries that may be saved to be recalled
	// later by user
#define MAX_ENTRY_LEN (64) // Maximum number of characters per console entry
	// string

typedef struct {
	uint8_t str[MAX_ENTRY_LEN]; // String of data received for console
		// entry 
	uint32_t len; // Number of valid characters in str
} ConsoleEntry;

static ConsoleEntry entries[HISTORY_SIZE]; // Used to keep track of current and
	// previous console entries
static uint32_t entriesWrIdx = 0; // Indicates which of entries is currently 
	// being filled by user input
static uint32_t entriesRdIdx = 0; // Inidcates which of entries was last read
	// when cycling through history.

static uint32_t cursorIdx = 0; // Indicates where cursor is for entry currently
	// being updated by user input

static uint32_t historyCnt = 0; // Number of valid entries in history.
static uint32_t oldestHistoryIdx = 0; // If entriesRdIdx is set to this value
	// we are at the limit fo entries history.

/**
 * Print the hex representation of each character in the buffer to console.
 *  Also marks where internal cursor marker is.
 *
 * \param[in] str Buffer to print as a series of hex values.
 * \param len Numbers of bytes in buff.
 *
 * \return None.
 */
static void printHex(const uint8_t* buff, uint32_t len){
	for (int idx = 0; idx < len; idx++) {
		char c = buff[idx];
		if (idx == cursorIdx) {
			consolePrint("[");
		}
		consolePrint("0x%x", c);
		if (idx == cursorIdx) {
			consolePrint("]");
		}
		consolePrint(" ");
	}
	consolePrint("\n");
}

/**
 * Handle escape sequence received from console.
 *
 * \param[in] seq Sequence of characters to handle.
 * \param len Number of valid characters in seq.
 *
 * \return 0 on successful handle. Non-zero indicates incomplete sequence.
 */
static int handleEscSeq(const uint8_t* seq, uint32_t len) {

	if (len < 3) {
		return -1;
	}

	if (seq[1] == '[' && seq[2] == 'A') {
		// Up arrow key 
		if (entriesRdIdx == oldestHistoryIdx) {
			// Sound to indicate we are at limit 
			consolePrint("\a");
		} else {
			// Clear line
			consolePrint("\r\x1B[2K");

			// Copy and display entry from history
			if (!entriesRdIdx) {
				entriesRdIdx = HISTORY_SIZE - 1;
			} else {
				entriesRdIdx--;
			}

			memcpy(entries[entriesWrIdx].str,
				entries[entriesRdIdx].str, MAX_ENTRY_LEN);
			entries[entriesWrIdx].len = entries[entriesRdIdx].len;
			cursorIdx = entries[entriesRdIdx].len;
	
			sendUsbSerialData(entries[entriesWrIdx].str, 
				entries[entriesWrIdx].len);
		}
	} else if (seq[1] == '[' && seq[2] == 'B') {
		// Down arrow key 
		if (entriesRdIdx == entriesWrIdx) {
			// Sound to indicate we are at limit 
			consolePrint("\a");
		} else {
			// Clear line
			consolePrint("\r\x1B[2K");

			// (Possibly) Copy and display entry from history
			entriesRdIdx++;
			entriesRdIdx %= HISTORY_SIZE;
			if (entriesRdIdx == entriesWrIdx) {
				entries[entriesWrIdx].len = 0;
				cursorIdx = 0;
			} else {
				memcpy(entries[entriesWrIdx].str,
					entries[entriesRdIdx].str,
					MAX_ENTRY_LEN);
				entries[entriesWrIdx].len = 
					entries[entriesRdIdx].len;
				cursorIdx = entries[entriesRdIdx].len;

				sendUsbSerialData(entries[entriesWrIdx].str,
					entries[entriesWrIdx].len);
			}
		}
	} else if (seq[1] == '[' && seq[2] == 'C') {
		// Forward arrow key 
		if (cursorIdx < entries[entriesWrIdx].len) {
			cursorIdx++;
			sendUsbSerialData(seq, 3);
		} else {
			// Sound to indicate we are at limit 
			consolePrint("\a");
		}
	} else if (seq[1] == '[' && seq[2] == 'D') {
		// Back arrow key
		if (cursorIdx > 0) {
			cursorIdx--;
			sendUsbSerialData(seq, 3);
		} else {
			// Sound to indicate we are at limit 
			consolePrint("\a");
		}
	}

	//TODO: handle ctrl-C and other escape sequences?? different lengths?

	return 0;
}

/**
 * Delete character before cursor.
 * 
 * \return None.
 */
static void delChar() {
	if (cursorIdx > 0) {
		consolePrint("\b");

		entries[entriesWrIdx].len--;
		cursorIdx--;

		for (int idx = cursorIdx; idx < entries[entriesWrIdx].len; 
			idx++) {
			consolePrint("%c", entries[entriesWrIdx].str[idx+1]);
			entries[entriesWrIdx].str[idx] = 
				entries[entriesWrIdx].str[idx+1];
		}
		consolePrint(" \b");

		for (int cnt = 0; cnt < entries[entriesWrIdx].len - cursorIdx; 
			cnt++) {
			consolePrint("\b");
		}
	} else {
		// Sound to indicate we are at limit and del must be ignored
		consolePrint("\a");
	}
}

/**
 * Handle tab completion request.
 *
 * \return 0 if completion was occurred. 
 */
static int completeCmd() {

//TODO
	return -1;
}

/**
 * Print all possible command completions to console. 
 *
 * \return None.
 */
static void printCmdCompletions() {

//TODO
}

/**
 * Insert data into current entry at current cursor position. If entry reaches
 *	limit, excess data will be ignored.
 *
 * \param[in] data Data to insert into entry.
 * \param len Length of data.
 *
 * \return None.
 */
static void insertEntryData(const uint8_t* data, uint32_t len) {
	// Print newly received characters (up to entry limit)
	uint32_t insert_len = len;
	if (insert_len > MAX_ENTRY_LEN - cursorIdx) {
		insert_len = MAX_ENTRY_LEN - cursorIdx;
	}

	if (!insert_len) {
		// Sound to indicate we are at limit 
		consolePrint("\a");
		return;
	}

	sendUsbSerialData(data, insert_len);

	// Keep track of adjusted cursor location
	uint32_t new_cursor_idx = cursorIdx + insert_len;

	// Print shifted characters (up to entry limit)
	uint32_t cpy_len = entries[entriesWrIdx].len - cursorIdx;
	if (cpy_len > MAX_ENTRY_LEN - new_cursor_idx) {
		cpy_len = MAX_ENTRY_LEN - new_cursor_idx;
		// Sound to indicate we are at limit 
		consolePrint("\a");
	}

	if (cpy_len) {
		sendUsbSerialData(&entries[entriesWrIdx].str[cursorIdx], 
			cpy_len);
		int wr_idx = new_cursor_idx + cpy_len - 1;
		int rd_idx = cursorIdx + cpy_len - 1;
		for (int cnt = 0; cnt < cpy_len; cnt++) {
			// Back up console display cursor
			consolePrint("\b");

			// Copy shifted characters (up to entry limit)
			entries[entriesWrIdx].str[wr_idx--] = 
				entries[entriesWrIdx].str[rd_idx--];
		}
	}

	// Copy inserted characters (up to entry limit)
	memcpy(&entries[entriesWrIdx].str[cursorIdx], data, insert_len);

	// Update cursor location and entry length
	cursorIdx = new_cursor_idx;
	entries[entriesWrIdx].len += insert_len;
	if (entries[entriesWrIdx].len > MAX_ENTRY_LEN) {
		entries[entriesWrIdx].len = MAX_ENTRY_LEN;
	}
}

/**
 * Mark current entry as complete and attempt to execute.
 * 
 * \return.
 */
static void entryComplete() {
	// Enter or return indicates end of command
	consolePrint("\n");

//TODO: call function to actually handle command 
	printHex(entries[entriesWrIdx].str, entries[entriesWrIdx].len);

	// Only save history if there was content in entry
	if (entries[entriesWrIdx].len) {
		entriesWrIdx++;
		entriesWrIdx %= HISTORY_SIZE;
		if (historyCnt < HISTORY_SIZE-1) {
			historyCnt++;
		} else {
			oldestHistoryIdx++;
			oldestHistoryIdx %= HISTORY_SIZE;
		}
	}

	entriesRdIdx = entriesWrIdx;

	cursorIdx = 0;
	entries[entriesWrIdx].len = 0;
}

/**
 * React to character received from serial input device.
 *
 * \param c Character received from serial input device.
 *
 * \return None.
 */
static void handleSerialChar(uint8_t c) {
	static uint8_t esc_seq[4]; // Used to store previously recievd escape
		// sequences
	static uint32_t esc_cnt = 0; // Number of valid escape sequence 
		// characters in esc_seq

	static char prev_c = ' '; // Character received last function call

	if (esc_cnt) {
		esc_seq[esc_cnt++] = c;

		int retval = handleEscSeq(esc_seq, esc_cnt);

		if (!retval) {
			esc_cnt = 0;
		}
		return;
	}
	
	switch (c) {
	case 0x3:
		// Crtl-C
		consolePrint("\n");

		entriesRdIdx = entriesWrIdx;

		cursorIdx = 0;
		entries[entriesWrIdx].len = 0;
		break;

	case 0x1b:
		// Start of escape sequence
		esc_seq[0] = c;
		esc_cnt = 1;
		break;

	case 0x7f:
	case '\b':
		delChar();
		break;
	
	case '\t':
		if (prev_c != '\t') {
			int retval = completeCmd();

			if (retval) {
				// On commpletion need to ignore tab was pressed
				prev_c = ' ';
				return;
			}
		} else {
			printCmdCompletions();
		}
		break;

	case '\r':
	case '\n':
		entryComplete();
		break;

	default:
		// Insert all other characters into buffer
		insertEntryData(&c, 1);
		break;
	}

	prev_c = c;
}

/**
 * Process characters received on serial input device and execute actions if
 *  received characters complete a command. 
 *
 * \return None.
 */
void handleConsoleInput(void) {
	uint8_t rcv_buff[64];
	int bytes_rcvd = -1;
	
	bytes_rcvd = getUsbSerialData(rcv_buff, sizeof(rcv_buff));

	if (!bytes_rcvd)
		return;

	if (bytes_rcvd < 0) {
		consolePrint("\n!!! Error receiving serial data. Flushing "
			"input stream. !!!\n");

		// Flush input stream
		do {
			bytes_rcvd = getUsbSerialData(rcv_buff, 
				sizeof(rcv_buff));
		} while (bytes_rcvd);
		return;
	}

	for (int idx = 0; idx < bytes_rcvd; idx++) {
		handleSerialChar(rcv_buff[idx]);
	}
}

/**
 * printf-like function for printing text to console.
 *
 * \param[in] format Formatted string (same rules as printf).
 *
 * \return None.
 */
void consolePrint(const char* format, ...) {
	uint8_t buff[256];
	va_list args;
	int num_chars = 0;

	va_start(args, format);
	num_chars = vsnprintf((char*)buff, sizeof(buff), format, args);
	if (num_chars > 0) {
		int start_idx = 0;
		int idx = 0;
		for (idx = 0; idx < num_chars; idx++) {
			if (buff[idx] == '\n') {
				// Send carriage return so we don't just drop
				//  down a line to same offset
				sendUsbSerialData(&buff[start_idx], 
					idx-start_idx+1);
				sendUsbSerialData((uint8_t*)"\r", 1);
				start_idx = idx+1;
			}
		}
		if (start_idx < num_chars) {
			sendUsbSerialData(&buff[start_idx], num_chars-start_idx);
		}
	}
	va_end(args);
}
