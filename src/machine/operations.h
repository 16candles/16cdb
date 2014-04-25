/* operations.h --- operator implementations for the 16candles VM
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

#ifndef C16_OPERATIONS_H
#define C16_OPERATIONS_H

#include "../common/arch.h"
#include "processor.h"
#include "memory.h"

#include <stdbool.h>

// location the stack starts.
c16_word stack_start;

// Binary operator.
typedef c16_word (*bin_op)(c16_word,c16_word);

// Comparison operator (specialization of binary operator).
typedef c16_word (*cmp_op)(c16_word,c16_word);

// Unary operator.
typedef c16_word (*un_op)(c16_word);

// parsing:
bin_op parse_bin_op(c16_opcode);
cmp_op parse_cmp_op(c16_opcode);
un_op  parse_un_op(c16_opcode);

// Applying:
void bin_lit_lit(bin_op,void*,bool);
void bin_lit_reg(bin_op,void*,bool,void*,bool);
void bin_reg_reg(bin_op,void*,bool,void*,bool,void*,bool);

void cmp_lit_lit(cmp_op);
void cmp_lit_reg(cmp_op,void*,bool);
void cmp_reg_reg(cmp_op,void*,bool,void*,bool);

void un_lit(un_op,void*,bool);
void un_reg(un_op,void*,bool,void*,bool);


// Operations:
void op_bin_ops(c16_opcode);
void op_cmp_ops(c16_opcode);
void op_un_ops(c16_opcode);
void op_push(c16_opcode);
void op_jmp(c16_opcode);
void op_write(c16_opcode);
void op_mset(c16_opcode);
void op_swap();
void op_pop();
void op_peek();
void op_flush();
void op_read();

c16_word op_and(c16_word,c16_word);
c16_word op_or(c16_word,c16_word);
c16_word op_xand(c16_word,c16_word);
c16_word op_xor(c16_word,c16_word);
c16_word op_lshift(c16_word,c16_word);
c16_word op_rshift(c16_word,c16_word);
c16_word op_add(c16_word,c16_word);
c16_word op_sub(c16_word,c16_word);
c16_word op_mul(c16_word,c16_word);
c16_word op_div(c16_word,c16_word);
c16_word op_min(c16_word,c16_word);
c16_word op_max(c16_word,c16_word);
c16_word op_mod(c16_word,c16_word);
c16_word op_gte(c16_word,c16_word);
c16_word op_lte(c16_word,c16_word);
c16_word op_eq(c16_word,c16_word);
c16_word op_neq(c16_word,c16_word);
c16_word op_gt(c16_word,c16_word);
c16_word op_lt(c16_word,c16_word);
c16_word op_inv(c16_word);
c16_word op_inc(c16_word);
c16_word op_dec(c16_word);
c16_word op_set(c16_word);


#endif /* OPERATIONS_H */
