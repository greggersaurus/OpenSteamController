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
#include "command.h"
#include "led_ctrl.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define HISTORY_SIZE (16) // Number of entries that may be saved to be recalled
	// later by user
#define MAX_ENTRY_LEN (64) // Maximum number of characters per console entry
	// string

typedef struct {
	char str[MAX_ENTRY_LEN]; // String of data received for console
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
static void printHex(const char* buff, uint32_t len){
	printf("\n");
	for (int idx = 0; idx < len; idx++) {
		char c = buff[idx];
		if (idx == cursorIdx) {
			printf("[");
		}
		printf("0x%x", c);
		if (idx == cursorIdx) {
			printf("]");
		}
		printf(" ");
	}
	printf("\n");
	printf("\n");
}

/**
 * Handle escape sequence received from console.
 *
 * \param[in] seq Sequence of characters to handle.
 * \param len Number of valid characters in seq.
 *
 * \return 0 on successful handle. Non-zero indicates incomplete sequence.
 */
static int handleEscSeq(const char* seq, uint32_t len) {

	if (len < 3) {
		return -1;
	}

	if (seq[1] == '[' && seq[2] == 'A') {
		// Up arrow key 
		if (entriesRdIdx == oldestHistoryIdx) {
			// Sound to indicate we are at limit 
			printf("\a");
			usb_flush();
		} else {
			// Clear line
			printf("\r\x1B[2K");
			usb_flush();

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
	
			usb_putb(entries[entriesWrIdx].str, 
				entries[entriesWrIdx].len);
			usb_flush();
		}
	} else if (seq[1] == '[' && seq[2] == 'B') {
		// Down arrow key 
		if (entriesRdIdx == entriesWrIdx) {
			// Sound to indicate we are at limit 
			printf("\a");
			usb_flush();
		} else {
			// Clear line
			printf("\r\x1B[2K");
			usb_flush();

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

				usb_putb(entries[entriesWrIdx].str,
					entries[entriesWrIdx].len);
				usb_flush();
			}
		}
	} else if (seq[1] == '[' && seq[2] == 'C') {
		// Forward arrow key 
		if (cursorIdx < entries[entriesWrIdx].len) {
			cursorIdx++;
			usb_putb(seq, 3);
			usb_flush();
		} else {
			// Sound to indicate we are at limit 
			printf("\a");
			usb_flush();
		}
	} else if (seq[1] == '[' && seq[2] == 'D') {
		// Back arrow key
		if (cursorIdx > 0) {
			cursorIdx--;
			usb_putb(seq, 3);
			usb_flush();
		} else {
			// Sound to indicate we are at limit 
			printf("\a");
			usb_flush();
		}
	}

	return 0;
}

/**
 * Delete character before cursor.
 * 
 * \return None.
 */
static void delChar() {
	if (cursorIdx > 0) {
		printf("\b");

		entries[entriesWrIdx].len--;
		cursorIdx--;

		for (int idx = cursorIdx; idx < entries[entriesWrIdx].len; 
			idx++) {
			printf("%c", entries[entriesWrIdx].str[idx+1]);
			entries[entriesWrIdx].str[idx] = 
				entries[entriesWrIdx].str[idx+1];
		}
		printf(" \b");

		for (int cnt = 0; cnt < entries[entriesWrIdx].len - cursorIdx; 
			cnt++) {
			printf("\b");
		}
	} else {
		// Sound to indicate we are at limit and del must be ignored
		printf("\a");
	}
	usb_flush();
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
static void insertEntryData(const char* data, uint32_t len) {
	// Print newly received characters (up to entry limit)
	uint32_t insert_len = len;
	if (insert_len > MAX_ENTRY_LEN - cursorIdx) {
		insert_len = MAX_ENTRY_LEN - cursorIdx;
	}

	if (!insert_len) {
		// Sound to indicate we are at limit 
		printf("\a");
		usb_flush();
		return;
	}

	usb_putb(data, insert_len);
	usb_flush();

	// Keep track of adjusted cursor location
	uint32_t new_cursor_idx = cursorIdx + insert_len;

	// Print shifted characters (up to entry limit)
	uint32_t cpy_len = entries[entriesWrIdx].len - cursorIdx;
	if (cpy_len > MAX_ENTRY_LEN - new_cursor_idx) {
		cpy_len = MAX_ENTRY_LEN - new_cursor_idx;
		// Sound to indicate we are at limit 
		printf("\a");
		usb_flush();
	}

	if (cpy_len) {
		usb_putb(&entries[entriesWrIdx].str[cursorIdx], cpy_len);
		usb_flush();
		int wr_idx = new_cursor_idx + cpy_len - 1;
		int rd_idx = cursorIdx + cpy_len - 1;
		for (int cnt = 0; cnt < cpy_len; cnt++) {
			// Back up console display cursor
			printf("\b");
			usb_flush();

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
 * Handle tab completion request.
 *
 * \return NULL terminated list of potential completions, or NULL if fewer
 *	completions was handled. 
 */
static const char** completeCmd() {
	const char** cmd_completions = NULL;

	cmd_completions = getCmdCompletions(
		(const char*)entries[entriesWrIdx].str, cursorIdx);

	// Sound to indicate no completion matches available
	if (!cmd_completions[0]) {
		printf("\a");
		usb_flush();
		return NULL;
	}

	// If only single match, insert completion data into current entry
	if (cmd_completions[0] && !cmd_completions[1]) {
		int insert_len = strlen(cmd_completions[0]) - cursorIdx;
		insertEntryData(&cmd_completions[0][cursorIdx],
			insert_len);
		insertEntryData(" ", 1);
		return NULL;
	}

	return cmd_completions;
}

/**
 * Print all possible command completions to console. 
 *
 * \param[in] NULL terminated list of possible command completions.
 *
 * \return None.
 */
static void printCmdCompletions(const char** cmdCompletions) {
	printf("\n");
	for (int idx = 0; cmdCompletions[idx]; idx++) {
		printf("%s\n", cmdCompletions[idx]);
	}
	printf("\n");

	// Reprint current entry (putting cursor in proper position)
	usb_putb(entries[entriesWrIdx].str, entries[entriesWrIdx].len);
	for (int cnt = 0; cnt < entries[entriesWrIdx].len - cursorIdx; cnt++) {
		printf("\b");
	}
	usb_flush();
}

/**
 * Mark current entry as complete and attempt to execute.
 * 
 * \return.
 */
static void entryComplete() {
	// Enter or return indicates end of command
	printf("\n");

	// Debug so we see exactly what was in entry buffer
	//printHex(entries[entriesWrIdx].str, entries[entriesWrIdx].len);

	executeCmd(entries[entriesWrIdx].str, 
		entries[entriesWrIdx].len);

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
static void handleSerialChar(char c) {
	static char esc_seq[4]; // Used to store previously recievd escape
		// sequences
	static uint32_t esc_cnt = 0; // Number of valid escape sequence 
		// characters in esc_seq

	static const char** prev_cmd_completions = NULL;
	const char** cmd_completions = NULL;

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
		printf("\n");

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
		if (prev_cmd_completions) {
			cmd_completions = prev_cmd_completions;
			printCmdCompletions(cmd_completions);
		} else {
			cmd_completions = completeCmd();
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

	prev_cmd_completions = cmd_completions;
}

/**
 * Process characters received on serial input device and execute actions if
 *  received characters complete a command. 
 *
 * \return None.
 */
void handleConsoleInput(void) {
	while (usb_tstc()) {
		handleSerialChar(usb_getc());
	}
}
