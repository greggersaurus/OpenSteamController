/**
 * \file console.c
 * \brief Encompasses functions for command line interface for interacting with
 *	the Steam Controller.
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

typedef struct {
	const char* cmdName;
	int (*cmdFnc)(int argc, const char* argv[]);
} Cmd;

int memCmdFnc(int argc, const char* argv[]) {
	return 0;
}

int eepromCmdFnc(int argc, const char* argv[]) {
	return 0;
}

static Cmd memCmd = { .cmdName = "mem", .cmdFnc = memCmdFnc };
static Cmd eepromCmd = { .cmdName = "eeprom", .cmdFnc = eepromCmdFnc };

static Cmd* cmds[] = {
	&memCmd,
	&eepromCmd
};

/**
 * \return NULL terminated list of Cmds that could match cmd string.
 */
static Cmd* searchCmds(const char* cmd) {
	return 0;
}

/**
 * Print the hex representation of each character in the buffer to console.
 *
 * \param[in] str Buffer to print as a series of hex values.
 * \param len Numbers of bytes in buff.
 *
 * \return None.
 */
void printHex(const uint8_t* buff, uint32_t len){
	for (int idx = 0; idx < len; idx++) {
		char c = buff[idx];
		consolePrint("0x%x ", c);
	}
	consolePrint("\n");
}

#define CMD_HISTORY_LEN (16) // Number of commands that may be saved
#define MAX_CMD_LEN (64) // Maximum number of characters per command, including
	// null termination of cmd string.

/**
 * React to character received from serial input device.
 *
 * \param c Character received from serial input device.
 *
 * \return None.
 */
void handleSerialChar(uint8_t c) {
	static uint8_t cmd_history[CMD_HISTORY_LEN][MAX_CMD_LEN];

	static uint32_t cmd_history_idx = 0; // Indicates which string in
		// cmd_history (of length MAX_CMD_LEN) is currently being filled

	static uint32_t cmd_len = 0; // Number of valid characters in current 
		// command
	static uint32_t cursor_idx = 0; // Location of cursor in current command

	static uint8_t esc_seq[3]; // Used to store previously recievd escape
		// sequences
	static uint32_t esc_cnt = 0; // Number of valid escape sequence 
		// characters in esc_seq

	static char last_c = ' '; // Character received last function call

	if (esc_cnt) {
		esc_seq[esc_cnt++] = c;

		if (esc_cnt == 3) {
			if (esc_seq[1] == '[' && esc_seq[2] == 'A') {
				// Up arrow key 
//TODO: cycle through cmd history
			} else if (esc_seq[1] == '[' && esc_seq[2] == 'B') {
				// Down arrow key 
//TODO: cycle through cmd history:
			} else if (esc_seq[1] == '[' && esc_seq[2] == 'C') {
				// Forward arrow key 
				if (cursor_idx < cmd_len) {
					cursor_idx++;
					sendUsbSerialData(esc_seq, 3);
				}
			} else if (esc_seq[1] == '[' && esc_seq[2] == 'D') {
				// Back arrow key
				if (cursor_idx > 0) {
					cursor_idx--;
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
		if (cursor_idx > 0) {
			consolePrint("\b");

			cmd_len--;
			cursor_idx--;

			for (int idx = cursor_idx; idx < cmd_len; idx++) {
				consolePrint("%c", 
					cmd_history[cmd_history_idx][idx+1]);
				cmd_history[cmd_history_idx][idx] = 
					cmd_history[cmd_history_idx][idx+1];
			}
			consolePrint(" \b");

			for (int cnt = 0; cnt < cmd_len - cursor_idx; cnt++) {
				consolePrint("\b");
			}
		}
		break;
	
	case '\t':
		// Tab (for completion)
//TODO: use last_c to know how to autocomplete
		break;

	case '\r':
	case '\n':
		// Enter or return indicates end of command
		consolePrint("\n");

//TODO: call function to actually handle command 
		printHex(cmd_history[cmd_history_idx], cmd_len);

		cmd_history_idx++;
		cmd_history_idx %= CMD_HISTORY_LEN;

		cursor_idx = 0;
		cmd_len = 0;
		break;

	default:
		// Insert all other characters into buffer
		if (cursor_idx < MAX_CMD_LEN) {
			sendUsbSerialData(&c, 1);

			for (int idx = cmd_len; idx > cursor_idx; idx--) {
				cmd_history[cmd_history_idx][idx] = 
					cmd_history[cmd_history_idx][idx-1];
			}

			cmd_history[cmd_history_idx][cursor_idx] = c;
			cursor_idx++;
			cmd_len++;

			sendUsbSerialData(
				&cmd_history[cmd_history_idx][cursor_idx],
				cmd_len - cursor_idx);
			for (int cnt = 0; cnt < cmd_len - cursor_idx; cnt++) {
				consolePrint("\b");
			}
		}
		break;
	}

	last_c = c;
}

/**
 * Process characters received on serial input device and handle actions if
 *  received characters complete a command. 
 *
 * \return None.
 */
void handleSerial(void) {
	uint8_t rcv_buff[64]; // For receiving characters
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

#if (0)
//TODO: add back in support for this...
void eepromCmd(const uint8_t* params, uint32_t len)
{
	uint8_t resp_msg[256];
	int resp_msg_len = 0;

	int word_size = 0;
	uint32_t offset = 0;
	int num_words = 0;	

	int bytes_per_word = 0;

	int num_params_rd = 0;

	num_params_rd = sscanf(params, "%d %x %d", &word_size, &offset, &num_words);

	if (num_params_rd != 3)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUsage: e word_size hex_offset num_words\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	if (word_size != 8 && word_size != 16 && word_size != 32)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUnsuported word size %d\r\n", word_size);
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	bytes_per_word = word_size / 8;
	uint32_t num_read_bytes = bytes_per_word * num_words;

	resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
		"\r\nReading %d %d-bit words starting at 0x%X in EEPROM\r\n", num_words, word_size, offset);
	sendUsbSerialData(resp_msg, resp_msg_len);


	void* read_data = malloc(num_read_bytes);
	if (!read_data){
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nmalloc failed for read_data buffer\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	int err_code = eeprom_read(offset, read_data, num_read_bytes);
	if (CMD_SUCCESS != err_code){
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nEEPROM Read failed with error code %d\r\n", err_code);
		sendUsbSerialData(resp_msg, resp_msg_len);
		free(read_data);
		return;
	}

	for (int word_cnt = 0; word_cnt < num_words; word_cnt++)
	{
		if (!(word_cnt % 8))
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"\r\n%08X: ", offset);
			sendUsbSerialData(resp_msg, resp_msg_len);
		}

		if (word_size == 8)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%02X ", *(uint8_t*)(read_data+offset));
		}
		else if (word_size == 16)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%04X ", *(uint16_t*)(read_data+offset));
		}
		else if (word_size == 32)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%08X ", *(uint32_t*)(read_data+offset));
		}
		sendUsbSerialData(resp_msg, resp_msg_len);

		offset += bytes_per_word;
	}

	resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), "\r\n");
	sendUsbSerialData(resp_msg, resp_msg_len);

	free(read_data);
}

/**
 * Process input command.
 *
 * \param[in] buffer Contains null terminated command string.
 * \param len Number of valid bytes in command buffer.
 *
 * \return None.
 */
void processCmd(const uint8_t* buffer, uint32_t len)
{
	uint8_t resp_msg[256];
	int resp_msg_len = 0;
	uint8_t cmd = 0;

	if (!len)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nInvalid Command Length of 0\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	cmd = buffer[0];

	switch (cmd)
	{
	case 'r':
		readCmd(&buffer[1], len-1);
		sendUsbSerialData(resp_msg, resp_msg_len);
		break;

	case 'w':
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nWrite Command not supported yet\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		break;

	case 'e':
		eepromCmd(&buffer[1], len-1);
		sendUsbSerialData(resp_msg, resp_msg_len);
		break;

	default:
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUnrecognized command %c\r\n", cmd);
		sendUsbSerialData(resp_msg, resp_msg_len);
		break;
	}
}

/**
 * Process a read command.
 *
 * \param[in] params Portion of null terminated command string containing parameters.
 * \param len Number of bytes in params
 *
 * \return None.
 */
void readCmd(const uint8_t* params, uint32_t len)
{
	uint8_t resp_msg[256];
	int resp_msg_len = 0;

	int word_size = 0;
	uint32_t addr = 0;
	int num_words = 0;	

	int bytes_per_word = 0;

	int num_params_rd = 0;

	num_params_rd = sscanf(params, "%d %x %d", &word_size, &addr, &num_words);

	if (num_params_rd != 3)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUsage: r word_size hex_base_addr num_words\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	if (word_size != 8 && word_size != 16 && word_size != 32)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUnsuported word size %d\r\n", word_size);
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	bytes_per_word = word_size / 8;

	resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
		"\r\nReading %d %d-bit words starting at 0x%X\r\n", num_words, word_size, addr);
	sendUsbSerialData(resp_msg, resp_msg_len);

	for (int word_cnt = 0; word_cnt < num_words; word_cnt++)
	{
		if (!(word_cnt % 8))
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"\r\n%08X: ", addr);
			sendUsbSerialData(resp_msg, resp_msg_len);
		}

		if (word_size == 8)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%02X ", *(uint8_t*)addr);
		}
		else if (word_size == 16)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%04X ", *(uint16_t*)addr);
		}
		else if (word_size == 32)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%08X ", *(uint32_t*)addr);
		}
		sendUsbSerialData(resp_msg, resp_msg_len);

		addr += bytes_per_word;
	}

	resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), "\r\n");
	sendUsbSerialData(resp_msg, resp_msg_len);
}
#endif
