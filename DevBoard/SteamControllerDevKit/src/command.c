/**
 * \file command.h
 * \brief Encompasses functions for processing commands received via console.
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
