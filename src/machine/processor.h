/* processor.h --- processor simulation for the 16candles VM
   Copyright (c) 2014 Jack Pugmire, Joe Jevnik, and Ted Meyer

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

#ifndef C16_PROCESSOR_H
#define C16_PROCESSOR_H

#include "register.h"
#include "memory.h"
#include "operations.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

// System Memory
c16_mem sysmem;

// Reserved Registers:
c16_reg ipt;      // Instruction pointer.
c16_reg spt;      // Stack pointer.
c16_reg ac1;      // Accumulator register 1.
c16_reg ac2;      // Accumulator register 2.
c16_reg tst;      // Comparison test register.
c16_reg inp;      // Standard input buffer.
c16_subreg inp_r; // The section to read from the input buffer.
c16_subreg inp_w; // The section to write to the input buffer.

// Free Registers:
c16_reg r0;       // Free register 0.
c16_subreg r0_f;
c16_subreg r0_b;
c16_reg r1;       // Free register 1.
c16_subreg r1_f;
c16_subreg r1_b;
c16_reg r2;       // Free register 2.
c16_subreg r2_f;
c16_subreg r2_b;
c16_reg r3;       // Free register 3.
c16_subreg r3_f;
c16_subreg r3_b;
c16_reg r4;       // Free register 4.
c16_subreg r4_f;
c16_subreg r4_b;
c16_reg r5;       // Free register 5.
c16_subreg r5_f;
c16_subreg r5_b;
c16_reg r6;       // Free register 6.
c16_subreg r6_f;
c16_subreg r6_b;
c16_reg r7;       // Free register 7.
c16_subreg r7_f;
c16_subreg r7_b;
c16_reg r8;       // Free register 8.
c16_subreg r8_f;
c16_subreg r8_b;
c16_reg r9;       // Free register 9.
c16_subreg r9_f;
c16_subreg r9_b;

// Sets up the registers and assigns their values..
void init_registers();

// Parses the register out of the byte, returning a c16_reg or a c16_subreg.
void *parse_reg(c16_halfword);

// Fills the register with the next word at the ipt.
// WARNING: Do not call on ipt, intermediate reading will corrupt the value.
void fill_word(c16_reg);

// Run one processor tick, returning -1 on terminate instruction
int proc_tick();

// Subtracts two timeval structures storing the result in the result struct.
// Returns 1 if result is negative.
// Source: http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
int timeval_subtract (struct timeval*,struct timeval*,struct timeval*);

// Run the processor until termination, executing the current program in RAM.
// The parameter is the minimum length in micro seconds of a single tick.
void run(long);

// Process the stdin in a second thread.
// pass a NULL, it isn't used.
void *process_stdin(void*);

#endif /* C16_PROCESSOR_H */
