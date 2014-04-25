/* register.h --- forward decleration of the registers.
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

#ifndef C16_REGISTER_H
#define C16_REGISTER_H

#include "../common/arch.h"

// Reserved Registers:
extern c16_reg ipt;      // Instruction pointer.
extern c16_reg spt;      // Stack pointer.
extern c16_reg ac1;      // Accumulator register.
extern c16_reg ac2;      // Second accumulator register.
extern c16_reg tst;      // Comparison test register.
extern c16_reg inp;      // Standard input buffer.
extern c16_subreg inp_r; // The section to read from the input buffer.
extern c16_subreg inp_w; // The section to write to the input buffer.

// Free Registers:
extern c16_reg r0;       // Free register 0.
extern c16_subreg r0_f;
extern c16_subreg r0_b;
extern c16_reg r1;       // Free register 1.
extern c16_subreg r1_f;
extern c16_subreg r1_b;
extern c16_reg r2;       // Free register 2.
extern c16_subreg r2_f;
extern c16_subreg r2_b;
extern c16_reg r3;       // Free register 3.
extern c16_subreg r3_f;
extern c16_subreg r3_b;
extern c16_reg r4;       // Free register 4.
extern c16_subreg r4_f;
extern c16_subreg r4_b;
extern c16_reg r5;       // Free register 5.
extern c16_subreg r5_f;
extern c16_subreg r5_b;
extern c16_reg r6;       // Free register 6.
extern c16_subreg r6_f;
extern c16_subreg r6_b;
extern c16_reg r7;       // Free register 7.
extern c16_subreg r7_f;
extern c16_subreg r7_b;
extern c16_reg r8;       // Free register 8.
extern c16_subreg r8_f;
extern c16_subreg r8_b;
extern c16_reg r9;       // Free register 9.
extern c16_subreg r9_f;
extern c16_subreg r9_b;

#endif /* REGISTER_H */
