/* commands.h --- The commands that a user can type in.
   Copyright (c) 2014 Joe Jevnik

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 51
   Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. */

#ifndef C16_DEBUG_COMMANDS_H
#define C16_DEBUG_COMMANDS_H

#include "../16common/common/arch.h"
#include "../16machine/machine/memory.h"
#include "../16machine/machine/processor.h"
#include "../16machine/machine/register.h"

#include <stdbool.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>

// The constant for register does not exist.
#define REG_DNE 38

// The index where halfregs start.
#define HALFREG_START 16

extern jmp_buf jump;
extern char   *binary_fl;

typedef void cmd_func(char**);

typedef struct {
    char     *name; // The name of the command, eg: step.
    cmd_func *func; // The function this command executes.
    int       argc; // The amount of arguments this function expects.
    char     *help; // The help string.
} command;

#define COMMAND_COUNT 12

// The list of commands.
command commands[COMMAND_COUNT];

// Parses the command out of the command name.
// return: The pointer to the command typed, or NULL if it is not a command.
command *resolve_cmd(char*);

// Exits the program.
void cmd_quit(char**);

// Restarts the vm.
void cmd_restart(char**);

// Steps through one operation.
void cmd_step(char**);

// Prints the state of the machines registers to stdout.
void cmd_dump(char**);

// Prints the help message.
void cmd_help(char**);

// Feeds input into the machine's standard in.
void cmd_inp(char**);

// Prints the value in a single register.
void cmd_reg(char**);

// Parses the memaddr commands paramaters.
void cmd_mem(char**);

// Prints the value stored at a particular memory address.
// If true, then print as halfword, otherwise print as word.
void print_memaddr(c16_word,bool);

// Prints the value stored at the memory address regstr.
void print_memreg(char*,bool);

// Parses escape codes out of strings. eg: "\\n" -> "\n".
// Does not parse hex, octal, or unicode.
// malloc's a string, be sure to free it.
char *escapestr(char*);

// Parses the register number out of the string.
// return: the number, or REG_DNE if it does not exist.
c16_halfword parse_regno(char*);

// Frees the registers.
void free_regs();

#endif
