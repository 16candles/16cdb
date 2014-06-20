/* debug.h --- debugging utility for the 16candles vm.
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

#ifndef C16_DEBUG_H
#define C16_DEBUG_H

#include "../16common/common/arch.h"
#include "../16machine/machine/memory.h"
#include "../16machine/machine/processor.h"
#include "../16machine/machine/register.h"
#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

extern int       pipe_fds[2];
extern pthread_t input_thread;

extern command commands[];

// The handler for when the machine crashes.
void sigsegv_handler(int);

// Return the string that that generates the given instruction, the second
// argument denotes whether to print with the symbols or not.
const char *cmdstr(c16_halfword,bool);

// Initializes the registers.
void init_regs(void);

// Frees the registers.
void free_regs(void);

// The debugging read eval print loop.
void repl(void);

// The handler for when the machine crashes.
void sigsegv_handler(int);

// Evaluate a line of user input.
void eval_line(char*);

#endif
