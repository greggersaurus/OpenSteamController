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
 * Process characters received on serial input device and handle actions if
 *  received characters complete a command. 
 *
 * \return None.
 */
void handleSerial(void){
	//TODO: add cmd history to iterate through with arrow keys
//	static char* cmdHistory[];

	static uint8_t buff[64]; // Command line buffer
	static uint32_t buff_valid_size = 0; // Number of valid characters in cmd line buffer
	static uint32_t buff_offset = 0; // Current location of cursor in cmd line buffer

	int bytes_rcvd = -1;

#if 1
	// Debug code for knowing what exactly we are receiving
	bytes_rcvd = getUsbSerialData(buff, sizeof(buff)-1);
	if (bytes_rcvd > 0) {
		consolePrint("\nBegin sequence received\n\n");
		for (int idx = 0; idx < bytes_rcvd; idx++) {
			char c = buff[idx];
			consolePrint("Received char with hex value 0x%x. "
				"This displays as \'%c\'\n", c, c);
		}
		consolePrint("\nEnd sequence received\n");
	}
	return;
#endif
	
	// Check if there are new characters to process
	if (buff_offset < sizeof(buff)) {
		bytes_rcvd = getUsbSerialData(&buff[buff_offset], 
			sizeof(buff)-1-buff_offset);
	}

	if (!bytes_rcvd)
		return;

	if (bytes_rcvd < 0) {
		consolePrint("\n!!! Command Line Buffer Overflow. Flushing input stream. !!!\n");

		buff_valid_size = 0;
		buff_offset = 0;

		// Flush input stream
		do {
			bytes_rcvd = getUsbSerialData(buff, sizeof(buff)-1);
		} while (bytes_rcvd);
		return;
	}

#if 0
	// Check new characters 
	int rd_idx = buff_offset;
	int wr_idx = buff_offset;
	buff_cnt += bytes_rcvd;
//TODO: need to handle escape sequenes, such as arrow keys. Make while instead of for loop?
	for (; rd_idx < buff_cnt; rd_idx++) {
		switch (buff[rd_idx]) {
			// Character deletion
			case 0x7f:
			case '\b':
				if (wr_idx)
					wr_idx--;
				if (buff_cnt > 1) {
					buff_cnt -= 2;
				} else {
					buff_cnt = 0;
				}
				break;
			
			// Escape sequence. We only handle some of these.
			case 0x33:

				// Up or down to copy history to buff
				// Left or right to adjust cursor (i.e. buff_offset)

			// Tab (for completion)
			case '\t':

			// Enter or return indicates end of command
			case '\r':
			case '\n':

			// All other characters
			default:
				buff[wr_idx++] = buff[rd_idx];
				break;
		}
	}

/*
	for (; wr_idx < rd_idx; wr_idx++) {
		buff[wr_idx] = ' ';
	}
*/

//TODO: instead of re-writing entire line each time, use escape sequences to only backtrack as much as is needed?
	consolePrint("\r");
	sendUsbSerialData(buff, buff_cnt);
#endif

/*
	// TODO: temporary simple/dumb echo
	bytes_rcvd = getUsbSerialData(buff, sizeof(buff));
	if (bytes_rcvd > 0){
		sendUsbSerialData(buff, bytes_rcvd);
	}
	//TODO: report error via serial if necesssary...
*/
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
