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

/**
 * Print the hex representation of each character in the buffer to console.
 *
 * \param[in] str Buffer to print as a series of hex values.
 * \param len Numbers of bytes in buff.
 *
 * \return None.
 */
static void printHex(const uint8_t* buff, uint32_t len){
	for (int idx = 0; idx < len; idx++) {
		char c = buff[idx];
		consolePrint("0x%x ", c);
	}
	consolePrint("\n");
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

//TODO: handle in separate function/C file?
	if (esc_cnt) {
		esc_seq[esc_cnt++] = c;

		if (esc_cnt == 3) {
			if (esc_seq[1] == '[' && esc_seq[2] == 'A') {
				// Up arrow key 
				uint32_t next_idx = entriesRdIdx - 1;
				if (!entriesRdIdx) {
					next_idx = HISTORY_SIZE - 1;
				}

				if (next_idx == entriesWrIdx) {
					consolePrint("\a");
				} else {
					consolePrint("\r\x1B[2K");

					entriesRdIdx = next_idx;
					memcpy(entries[entriesWrIdx].str,
						entries[entriesRdIdx].str,
						MAX_ENTRY_LEN);
					entries[entriesWrIdx].len = 
						entries[entriesRdIdx].len;
					cursorIdx = entries[entriesRdIdx].len;
			
					sendUsbSerialData(
						entries[entriesWrIdx].str, 
						entries[entriesWrIdx].len);
				}
			} else if (esc_seq[1] == '[' && esc_seq[2] == 'B') {
				// Down arrow key 
				if (entriesRdIdx == entriesWrIdx) {
					consolePrint("\a");
				} else {
					consolePrint("\r\x1B[2K");

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
			} else if (esc_seq[1] == '[' && esc_seq[2] == 'C') {
				// Forward arrow key 
				if (cursorIdx < entries[entriesWrIdx].len) {
					cursorIdx++;
					sendUsbSerialData(esc_seq, 3);
				}
			} else if (esc_seq[1] == '[' && esc_seq[2] == 'D') {
				// Back arrow key
				if (cursorIdx > 0) {
					cursorIdx--;
					sendUsbSerialData(esc_seq, 3);
				}
			}

			esc_cnt = 0;
		}
		return;
	}
	
	switch (c) {
	case 0x1b:
		// Start of escape sequence
		esc_seq[0] = c;
		esc_cnt = 1;
		break;

	case 0x7f:
	case '\b':
		// Character deletion
		if (cursorIdx > 0) {
			consolePrint("\b");

			entries[entriesWrIdx].len--;
			cursorIdx--;

			for (int idx = cursorIdx; idx < entries[entriesWrIdx].len; idx++) {
				consolePrint("%c", 
					entries[entriesWrIdx].str[idx+1]);
				entries[entriesWrIdx].str[idx] = 
					entries[entriesWrIdx].str[idx+1];
			}
			consolePrint(" \b");

			for (int cnt = 0; cnt < entries[entriesWrIdx].len - cursorIdx; cnt++) {
				consolePrint("\b");
			}
		}
		break;
	
	case '\t':
		// Tab (for completion)
//TODO: use prev_c to know how to autocomplete
		break;

	case '\r':
	case '\n':
		// Enter or return indicates end of command
		consolePrint("\n");

//TODO: call function to actually handle command 
		printHex(entries[entriesWrIdx].str, entries[entriesWrIdx].len);

		entriesWrIdx++;
		entriesWrIdx %= HISTORY_SIZE;
		entriesRdIdx = entriesWrIdx;

		cursorIdx = 0;
		entries[entriesWrIdx].len = 0;
		break;

	default:
		// Insert all other characters into buffer
		if (cursorIdx < MAX_ENTRY_LEN) {
			sendUsbSerialData(&c, 1);

			for (int idx = entries[entriesWrIdx].len; idx > cursorIdx; idx--) {
				entries[entriesWrIdx].str[idx] = 
					entries[entriesWrIdx].str[idx-1];
			}

			entries[entriesWrIdx].str[cursorIdx] = c;
			cursorIdx++;
			entries[entriesWrIdx].len++;

			sendUsbSerialData(
				&entries[entriesWrIdx].str[cursorIdx],
				entries[entriesWrIdx].len - cursorIdx);
			for (int cnt = 0; cnt < entries[entriesWrIdx].len - cursorIdx; cnt++) {
				consolePrint("\b");
			}
		}
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
