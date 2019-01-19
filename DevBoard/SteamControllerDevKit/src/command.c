/**
 * \file command.c
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

#include "command.h"

#include "chip.h"
#include "gpio_11xx_1.h"

#include "eeprom_access.h"
#include "mem_access.h"
#include "led_ctrl.h"
#include "init.h"
#include "adc_read.h"
#include "monitor.h"
#include "trackpad.h"
#include "haptic.h"
#include "jingle_data.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

typedef struct {
	const char* cmdName;
	int (*cmdFnc)(int argc, const char* argv[]);
} Cmd;

/**
 * Prints details to console regarding how to use the test print command line 
 *  function.
 *
 * \return None.
 */
void testPrintCmdUsage(void) {
	//TODO: add details on command how to exit
}

/**
 * Command to stress test printing. This makes a very long string and then
 *  continuously prints it, with null term inserted randomly. 
 * Idea is to run this and make sure that 1) The system never locks up and
 *  2) Upon exit printing still works fine.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int testPrintCmdFnc(int argc, const char* argv[]) {
	const uint32_t STR_SZ = 2048;
	char* tst_str = malloc(STR_SZ);

	if (!tst_str) {
		printf("Could not allocate tst_str\n");
		return -1;
	}

//TODO: more options for more tests (maybe allow for selecting max string length and variability or something?
//	i.e. we want this really aggressive test to see if anything locks up and perminently screws up, but also
//	 want test to make sure things are printing in a sane manner repeatedly... (i.e. try adcRead and see strings are cut short once in a while... what is this?)
//	 Is that due to ADC interrupts?? (shouldn't it not affect UART output...?)

	// Initialize string with characters that have printable symbols
	for (int idx = 0; idx < STR_SZ; idx++) {
		tst_str[idx] = (idx % 94) + 33;
	}
	tst_str[STR_SZ-1] = 0;

	//TODO: add prompt before starting?

	//TODO: switch to using usb_tstc for exiting loop
	while (1) {
		int rnd_idx = rand();	
	
		rnd_idx %= STR_SZ;	
	
		char tmp = tst_str[rnd_idx];

		// Change length of string
		tst_str[rnd_idx] = 0;

		//TODO: why are we getting bell sound...?
		printf("%s\n", tst_str);

		// Restore length of string
		tst_str[rnd_idx] = tmp;

		//TODO: Change to using sleep function (once implemented)
		for (volatile int cnt = 0; cnt < 0x4000; cnt++) {
		}
	}

	free(tst_str);
}

static Cmd cmds[] = {
	{.cmdName = "adcRead", .cmdFnc = adcReadCmdFnc},
	{.cmdName = "eeprom", .cmdFnc = eepromCmdFnc},
	{.cmdName = "haptic", .cmdFnc = hapticCmdFnc},
	{.cmdName = "initStats", .cmdFnc = initStatsCmdFnc},
	{.cmdName = "jingle", .cmdFnc = jingleCmdFnc},
	{.cmdName = "led", .cmdFnc = ledCmdFnc},
	{.cmdName = "mem", .cmdFnc = memCmdFnc},
	{.cmdName = "monitor", .cmdFnc = monitorCmdFnc},
	{.cmdName = "trackpad", .cmdFnc = trackpadCmdFnc},
	{.cmdName = "testPrint", .cmdFnc = testPrintCmdFnc},
//TODO: add version function (print version of fw, and build date, etc.)
//TODO: add help fnc (and structure to have usage functions for each command)
};

/**
 * Find commands whose name match the given string.
 *
 * \param[in] cmd String containing potential command name.
 * \param len Number of characters in cmd to consider for command name.
 *
 * \return NULL terminated list of Cmds that could match cmd string.
 */
static const Cmd** searchCmds(const char* cmd, uint32_t len) {
	static const Cmd* found_cmds[ARRAY_SIZE(cmds) + 1];
	found_cmds[0] = 0;

	//TODO: replace with trie instead of linear search?
	int insert_idx = 0;
	for (int cmd_idx = 0; cmd_idx < ARRAY_SIZE(cmds); cmd_idx++) {
		int skip = 0;
		for (int str_idx = 0; str_idx < len; str_idx++) {
			if (!cmds[cmd_idx].cmdName[str_idx]) {
				skip = 1;
				break;
			}
			if (cmds[cmd_idx].cmdName[str_idx] != cmd[str_idx]) {
				skip = 1;
				break;
			}
		}
		if (!skip) {
			found_cmds[insert_idx] = &cmds[cmd_idx];
			insert_idx++;
		}
	}
	found_cmds[insert_idx] = 0;

	return found_cmds;
}

/**
 * Find possible command completions for given string.
 *
 * \param[in] str String containing data to potentially complete with command
 *	names.
 * \param len Number of valid characters in str.
 *
 * \return A NULL terminated list of strings that are completions of str.
 */
const char** getCmdCompletions(const char* str, uint32_t len) {
	static const char* completions[ARRAY_SIZE(cmds) + 1];
	completions[0] = 0;

	const Cmd** possible_cmds = searchCmds(str, len);
	for (int idx = 0; possible_cmds[idx]; idx++) {
		completions[idx] = possible_cmds[idx]->cmdName;
		completions[idx+1] = 0;
	}

	return completions;
}

/**
 * Attempt to execute command as instructed by entry buffer.
 *
 * \param[in] str Entry buffer.
 * \param len Number of valid characters in entry buffer.
 *
 * \return None.
 */
void executeCmd(const char* entry, uint32_t len) {
	int cmd_len = 0;

	// Search for whitespace to mark end of command name
	while (cmd_len < len) {
		if (entry[cmd_len] == ' ') 
			break;
		cmd_len++;
	}

	const Cmd** possible_cmds = searchCmds(entry, cmd_len);

	if (!possible_cmds[0] || possible_cmds[1]) {
		printf("command not found\n");
		return;
	}

	int argc = 0;
	const char* argv[16];
	// So we can add in NULL terminations for argument strings without
	//  modifying entry buffer
	char* entry_cpy = (char*)malloc(sizeof(char)*(len + 1));

	memcpy(entry_cpy, entry, len);
	entry_cpy[len] = 0;

	argv[argc] = entry_cpy;
	argc++;

	for (int idx = 0; idx < len; idx++) {
		if (entry_cpy[idx] == ' ') {
			entry_cpy[idx] = 0;
		} else if (idx && !entry_cpy[idx-1]) {
			if (argc >= ARRAY_SIZE(argv)) {
				printf("Too many arguments for system to "
					"handle!\n");
				goto exit;
			}

			argv[argc] = &entry_cpy[idx];
			argc++;
		}
	}

	possible_cmds[0]->cmdFnc(argc, argv);

exit:
	free(entry_cpy);
}
