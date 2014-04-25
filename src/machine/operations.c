/* operations.c --- operator implementations for the 16candles VM
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

#include "operations.h"

// Parsing:
bin_op parse_bin_op(c16_opcode op){
    switch((op / 4) * 4){
    case OP_AND_:
        return &op_and;
    case OP_OR_:
        return &op_or;
    case OP_XAND_:
        return &op_xand;
    case OP_XOR_:
        return &op_xor;
    case OP_LSHIFT_:
        return &op_lshift;
    case OP_RSHIFT_:
        return &op_rshift;
    case OP_ADD_:
        return &op_add;
    case OP_SUB_:
        return &op_sub;
    case OP_MUL_:
        return &op_mul;
    case OP_DIV_:
        return &op_div;
    case OP_MOD_:
        return &op_mod;
    case OP_MIN_:
        return &op_min;
    case OP_MAX_:
        return &op_max;
    }
}

cmp_op parse_cmp_op(c16_opcode op){
    switch((op / 4) * 4){
    case OP_GTE_:
        return &op_gte;
    case OP_LTE_:
        return &op_lte;
    case OP_EQ_:
        return &op_eq;
    case OP_NEQ_:
        return &op_neq;
    case OP_GT_:
        return &op_gt;
    case OP_LT_:
        return &op_lt;
    }
}

un_op parse_un_op(c16_opcode op){
    switch((op / 2) * 2){
    case OP_INV_:
        return &op_inv;
    case OP_INC_:
        return &op_inc;
    case OP_DEC_:
        return &op_dec;
    case OP_SET_:
        return &op_set;
    }
}

// Apply:

// Applies a binary operation to 2 word lits and saves  the result into res.
void bin_lit_lit(bin_op f,void *res,bool is_subreg){
    if (is_subreg){
        *((c16_subreg) res) = (c16_halfword) f(*ac1,*ac2);
        return;
    }
    *((c16_reg) res) = f(*ac1,*ac2);
}

// Applies a binary operation to a word lit and a register and saves the result
// into res.
void bin_lit_reg(bin_op f,void *res,bool ris_subreg,void *reg,bool is_subreg){
    if (ris_subreg){
        if (is_subreg){
            *((c16_subreg) res)
                = (c16_halfword) f(*ac1,(c16_word) *((c16_subreg) reg));
            return;
        }
        *((c16_subreg) res) = (c16_halfword) f(*ac1,*((c16_reg) reg));
        return;
    }
    if (is_subreg){
        *((c16_reg) res) = f(*ac1,(c16_word) *((c16_subreg) reg));
        return;
    }
    *((c16_reg) res) = f(*ac1,*((c16_reg) reg));
}

// Applies a binary operation to 2 registers and saves  the result into res.
void bin_reg_reg(bin_op f,void *res,bool ris_subreg,
                 void *reg1,bool is_sr1,void *reg2,bool is_sr2){
    if (ris_subreg){
        if (is_sr1){
            if (is_sr2){
                *((c16_subreg) res)
                    = (c16_halfword) f((c16_word) *((c16_subreg) reg1),
                                       (c16_word) *((c16_subreg) reg2));
                return;
            }
            *((c16_subreg) res)
                = (c16_halfword) f((c16_word) *((c16_subreg) reg1),
                                   *((c16_reg) reg1));
            return;
        }
        if (is_sr2){
            *((c16_subreg) res)
                = (c16_halfword) f(*((c16_reg) reg1),
                                   (c16_word) *((c16_subreg) reg1));
            return;
        }
        *((c16_subreg) res)
            = (c16_halfword) f(*((c16_reg) reg1),*((c16_reg) reg1));
    }
    if (is_sr1){
        if (is_sr2){
            *((c16_reg) res) = f((c16_word) *((c16_subreg) reg1),
                                 (c16_word) *((c16_subreg) reg2));
            return;
        }
        *((c16_reg) res) = f((c16_word) *((c16_subreg) reg1),*((c16_reg) reg1));
        return;
    }
    if (is_sr2){
        *((c16_reg) res) = f(*((c16_reg) reg1),(c16_word) *((c16_subreg) reg1));
        return;
    }
    *((c16_reg) res) = f(*((c16_reg) reg1),*((c16_reg) reg1));
}

void cmp_lit_lit(cmp_op f){
    *tst = f(*ac1,*ac2);
}

void cmp_lit_reg(cmp_op f,void *reg1,bool is_sr1){
    if (is_sr1){
        *tst = f(*ac1,(c16_word) *((c16_subreg) reg1));
        return;
    }
    *tst = f(*ac1,*((c16_reg) reg1));
}

void cmp_reg_reg(cmp_op f,void *reg1,bool is_sr1,void *reg2,bool is_sr2){
    if (is_sr1){
        if (is_sr2){
            *tst = f((c16_word) *((c16_subreg) reg1),
                     (c16_word) *((c16_subreg) reg2));
            return;
        }
        *tst = f((c16_word) *((c16_subreg) reg1),*((c16_reg) reg2));
        return;
    }
    if (is_sr2){
        *tst = f(*((c16_reg) reg1),(c16_word) *((c16_subreg) reg2));
        return;
    }
    *tst = f(*((c16_reg) reg1),*((c16_reg) reg2));
}

void un_lit(un_op f,void *res,bool is_subreg){
    if (is_subreg){
        *((c16_subreg) res) = (c16_halfword) f(*ac1);
        return;
    }
    *((c16_reg) res) = f(*ac1);
}

void un_reg(un_op f,void *res,bool ris_subreg,void *reg,bool is_sr){
    if (ris_subreg){
        if (is_sr){
            *((c16_subreg) res)
                = (c16_halfword) f((c16_word) *((c16_subreg) reg));
            return;
        }
        *((c16_subreg) res) = (c16_halfword) f(*((c16_reg) reg));
        return;
    }
    if (is_sr){
        *((c16_reg) res) = f((c16_word) *((c16_subreg) reg));
        return;
    }
    *((c16_reg) res) = f(*((c16_reg) reg));
}

// Operator handling:
// Binary operation handling
void op_bin_ops(c16_opcode op){
    c16_halfword reg1,reg2,res;
    switch(op % 4){
    case LIT_LIT:
        fill_word(ac1);
        fill_word(ac2);
        res = sysmem.mem[(*ipt)++];
        bin_lit_lit(parse_bin_op(op),parse_reg(res),res > OP_r9);
        return;
    case LIT_REG:
        fill_word(ac1);
        reg1 = sysmem.mem[(*ipt)++];
        res  = sysmem.mem[(*ipt)++];
        bin_lit_reg(parse_bin_op(op),parse_reg(res),res > OP_r9,
                    parse_reg(reg1),reg1 > OP_r9);
        return;
    case REG_LIT:
        reg1 = sysmem.mem[(*ipt)++];
        *ac1 = (c16_word) sysmem.mem[(*ipt)++] << 8;
        *ac1 += sysmem.mem[(*ipt)++];
        res  = sysmem.mem[(*ipt)++];
        bin_lit_reg(parse_bin_op(op),parse_reg(res),res > OP_r9,
                    parse_reg(reg1),reg1 > OP_r9);
        return;
    case REG_REG:
        reg1 = sysmem.mem[(*ipt)++];
        reg2 = sysmem.mem[(*ipt)++];
        res  = sysmem.mem[(*ipt)++];
        bin_reg_reg(parse_bin_op(op),parse_reg(res),res > OP_r9,
                    parse_reg(reg1),reg1 > OP_r9,
                    parse_reg(reg2),reg2 > OP_r9);
    }
}

// Comparison operator handling
void op_cmp_ops(c16_opcode op){
    c16_halfword reg1,reg2;
    switch(op % 4){
    case LIT_LIT:
        fill_word(ac1);
        fill_word(ac2);
        cmp_lit_lit(parse_cmp_op(op));
        return;
    case LIT_REG:
        fill_word(ac1);
        reg1 = sysmem.mem[(*ipt)++];
        cmp_lit_reg(parse_cmp_op(op),parse_reg(reg1),reg1 > OP_r9);
        return;
    case REG_LIT:
        reg1 = sysmem.mem[(*ipt)++];
        fill_word(ac1);
        cmp_lit_reg(parse_cmp_op(op),parse_reg(reg1),reg1 > OP_r9);
        return;
    case REG_REG:
        reg1 = sysmem.mem[(*ipt)++];
        reg2 = sysmem.mem[(*ipt)++];
        cmp_reg_reg(parse_cmp_op(op),parse_reg(reg1),reg1 > OP_r9,
                    parse_reg(reg2),reg2 > OP_r9);
    }
}

// Unary operator handling
void op_un_ops(c16_opcode op){
    c16_halfword reg1,reg2;
    switch(op % 2){
    case LIT:
        fill_word(ac1);
        reg1 = sysmem.mem[(*ipt)++];
        un_lit(parse_un_op(op),parse_reg(reg1),reg1 > OP_r9);
        return;
    case REG:
        reg1 = sysmem.mem[(*ipt)++];
        reg2 = sysmem.mem[(*ipt)++];
        un_reg(parse_un_op(op),parse_reg(reg1),reg1 > OP_r9,
               parse_reg(reg2),reg2 > OP_r9);
    }
}

void op_push(c16_opcode op){
    c16_reg temp_reg;
    c16_halfword reg1;
    switch(op % 2){
    case LIT:
        sysmem.mem[(*spt)++] = (c16_halfword) sysmem.mem[(*ipt)++];
        sysmem.mem[(*spt)++] = (c16_halfword) sysmem.mem[(*ipt)++];
        return;
    case REG:
        reg1 = sysmem.mem[(*ipt)++];
        if (reg1 > OP_r9){
            sysmem.mem[(*spt)++] = *((c16_subreg) parse_reg(reg1));
            return;
        }
        temp_reg = parse_reg(reg1);
        sysmem.mem[(*spt)++] = (c16_halfword) (*temp_reg >> 8 << 8);
        sysmem.mem[(*spt)++] = (c16_halfword) *temp_reg;
    }
}

void op_jmp(c16_opcode op){
    switch(op){
    case OP_JMP:
        *ipt =  (c16_word) sysmem.mem[(*ipt)++] << 8;
        *ipt += sysmem.mem[(*ipt)++];
        return;
    case OP_JMPT:
        if (*tst){
            *ipt =  (c16_word) sysmem.mem[(*ipt)++] << 8;
            *ipt += sysmem.mem[(*ipt)++];
        }else{
            *ipt += 2;
        }
        return;
    case OP_JMPF:
        if (!*tst){
            *ipt =  (c16_word) sysmem.mem[(*ipt)++] << 8;
            *ipt += sysmem.mem[(*ipt)++];
        }else{
            *ipt += 2;
        }
    }
}

void op_write(c16_opcode op){
    c16_halfword reg1;
    c16_word c;
    switch(op % 2){
    case LIT:
        fill_word(ac1);
        putchar(*ac1);
        return;
    case REG:
        reg1 = sysmem.mem[(*ipt)++];
        if (reg1 > OP_r9){
            if ((c = *((c16_subreg) parse_reg(reg1))) < 128){
                putchar(c);
            }
            return;
        }
        if ((c = *((c16_reg) parse_reg(reg1))) < 128){
            putchar(c);
        }
    }
}

void op_mset(c16_opcode op){
    c16_halfword reg1,reg2;
    c16_reg temp_reg,temp_reg2;
    c16_subreg temp_subreg;
    switch(op % 3){
    case 0: // LIT_MEMREG:
        fill_word(ac1);
        reg1 = sysmem.mem[(*ipt)++];
        if (reg1 > OP_r9){
            temp_subreg = parse_reg(reg1);
            sysmem.mem[*temp_subreg] = *ac1 >> 8;
            sysmem.mem[*temp_subreg + 1] = (c16_halfword) *ac1;
            return;
        }
        temp_reg = parse_reg(reg1);
        sysmem.mem[*temp_reg] = *ac1 >> 8;
        sysmem.mem[*temp_reg + 1] = (c16_halfword) *ac1;
        return;
    case 1: // REG_MEMREG:
        reg1 = sysmem.mem[(*ipt)++];
        reg2 = sysmem.mem[(*ipt)++];
        if (reg1 > OP_r9){
            if (reg2 > OP_r9){
                sysmem.mem[*((c16_subreg) parse_reg(reg2))] =
                    *((c16_subreg) parse_reg(reg1));
            }else{
                sysmem.mem[*((c16_reg) parse_reg(reg2))] =
                    *((c16_subreg) parse_reg(reg1));
            }
            return;
        }
        temp_reg = parse_reg(reg1);
        if (reg2 > OP_r9){
            temp_subreg = parse_reg(reg2);
            sysmem.mem[*temp_subreg] = *temp_reg >> 8;
            sysmem.mem[*temp_subreg + 1] = *temp_reg;
        }else{
            temp_reg2 = parse_reg(reg2);
            sysmem.mem[*temp_reg2] = *temp_reg << 8;
            sysmem.mem[*temp_reg2 + 1] = (c16_halfword) *temp_reg;
        }
        return;
    case 2: // MEMADDR:
        fill_word(ac1);
        reg1 = sysmem.mem[(*ipt)++];
        if (reg1 > OP_r9){
            *((c16_subreg) parse_reg(reg1)) = sysmem.mem[*ac1];
            return;
        }
        temp_reg = parse_reg(reg1);
        *temp_reg = (c16_word) sysmem.mem[*ac1] << 8;
        *temp_reg += sysmem.mem[*ac1 + 1];
        return;
    case 3: // MEMREG:
        reg1 = sysmem.mem[(*ipt)++];
        reg2 = sysmem.mem[(*ipt)++];
        if (reg1 > OP_r9){
            if (reg2 > OP_r9){
                *((c16_subreg) parse_reg(reg1)) =
                    sysmem.mem[*((c16_subreg) parse_reg(reg2))];
            }else{
                *((c16_subreg) parse_reg(reg1)) =
                    sysmem.mem[*((c16_reg) parse_reg(reg2))];
            }
            return;
        }
        temp_reg = parse_reg(reg1);
        if (reg2 > OP_r9){
            temp_subreg = parse_reg(reg2);
            *temp_subreg = (c16_halfword) *temp_reg;
        }else{
            temp_reg2 = parse_reg(reg2);
            *temp_reg2 = (c16_word) sysmem.mem[*temp_reg] << 8;
            *temp_reg2 += sysmem.mem[*temp_reg + 1];
        }
        return;
    case 4: // LIT_MEMADDR:
        fill_word(ac1);
        fill_word(ac2);
        sysmem.mem[*ac2] = *ac1 << 8;
        sysmem.mem[*ac2 + 1] = (c16_halfword) *ac1;
        return;
    case 5: // REG_MEMADDR:
        reg1 = sysmem.mem[(*ipt)++];
        fill_word(ac1);
        if (reg1 > OP_r9){
            sysmem.mem[*ac1] = *((c16_subreg) parse_reg(reg1));
            return;
        }
        temp_reg = parse_reg(reg1);
        sysmem.mem[*ac1] = *temp_reg << 8;
        sysmem.mem[*ac1 + 1] = (c16_halfword) *temp_reg;
    }
}

void op_swap(){
    c16_halfword reg1,reg2;
    c16_subreg temp_subreg,temp_subreg2;
    c16_reg temp_reg,temp_reg2;
    reg1 = sysmem.mem[(*ipt)++];
    reg2 = sysmem.mem[(*ipt)++];
    if (reg1 == reg2){
        return;
    }
    if (reg1 > OP_r9){
        temp_subreg = (c16_subreg) parse_reg(reg1);
        if (reg2 > OP_r9){
            temp_subreg2  = ((c16_subreg) parse_reg(reg2));
            *temp_subreg  ^= *temp_subreg2;
            *temp_subreg2 ^= *temp_subreg;
            *temp_subreg  ^= *temp_subreg2;
            return;
        }else{
            temp_reg2    =  (c16_reg) parse_reg(reg2);
            *temp_subreg ^= (c16_halfword) *temp_reg2;
            *temp_reg2   ^= *temp_subreg;
            *temp_subreg ^= (c16_halfword) *temp_reg2;
            return;
        }
    }else{
        temp_reg = (c16_reg) parse_reg(reg1);
        if (reg2 > OP_r9){
            temp_subreg2  =  ((c16_subreg) parse_reg(reg2));
            *temp_reg     ^= *temp_subreg2;
            *temp_subreg2 ^= (c16_halfword) *temp_reg;
            *temp_reg     ^= *temp_subreg2;
            return;
        }else{
            temp_reg2  =  (c16_reg) parse_reg(reg2);
            *temp_reg  ^= *temp_reg2;
            *temp_reg2 ^= *temp_reg;
            *temp_reg  ^= *temp_reg2;
        }
    }
}

void op_pop(){
    c16_halfword reg1 = sysmem.mem[(*ipt)++];
    if (reg1 > OP_r9){
        if (*spt == stack_start){
            *((c16_subreg) parse_reg(reg1)) = 0;
            return;
        }
        *((c16_subreg) parse_reg(reg1)) = sysmem.mem[(*spt)--];
        return;
    }
    if (*spt == stack_start){
        *((c16_reg) parse_reg(reg1)) = 0;
        return;
    }
    *((c16_reg) parse_reg(reg1)) =  (c16_word) sysmem.mem[(*spt)--];
    if (*spt == stack_start){
        return;
    }
    *((c16_reg) parse_reg(reg1)) += (c16_word) sysmem.mem[(*spt)--] >> 8;
}

void op_peek(){
    c16_halfword reg1 = sysmem.mem[(*ipt)++];
    c16_word tmp_spt= *spt;
    if (reg1 > OP_r9){
        if (*spt == stack_start){
            *((c16_subreg) parse_reg(reg1)) = 0;
            return;
        }
        *((c16_subreg) parse_reg(reg1)) = sysmem.mem[tmp_spt];
        return;
    }
    if (*spt == stack_start){
        *((c16_reg) parse_reg(reg1)) = 0;
        return;
    }
    *((c16_reg) parse_reg(reg1)) =  (c16_word) sysmem.mem[tmp_spt--];
    if (*spt == stack_start){
        return;
    }
    if (tmp_spt == stack_start){
        return;
    }
    *((c16_reg) parse_reg(reg1)) += (c16_word) sysmem.mem[tmp_spt] >> 8;
}

void op_flush(){
    *spt = stack_start;
}

void op_read(){
    c16_halfword reg;
    c16_reg      temp_reg;
    reg = sysmem.mem[(*ipt)++];
    if (reg > OP_r9){
        *((c16_subreg) parse_reg(reg)) =
            (*sysmem.inputc) ? sysmem.inputv[(*inp_r)++] : 255;
        if (*sysmem.inputc){
            --(*sysmem.inputc);
        }
    }else{
        temp_reg = parse_reg(reg);
        if (*sysmem.inputc > 1){
            *temp_reg      =  (c16_word) sysmem.inputv[(*inp_r)++] << 8;
            *temp_reg      += sysmem.inputv[(*inp_r)++];
            *sysmem.inputc -= 2;
        }else if (*sysmem.inputc == 1){
            *temp_reg      =  (c16_word) sysmem.inputv[(*inp_r)++] << 8;
            --(*sysmem.inputc);
        }else{
            *temp_reg        = 255;
        }
    }
    *tst = *sysmem.inputb || *sysmem.inputc;
}

// Binary operations:

c16_word op_and(c16_word p1,c16_word p2){
    return p1 & p2;
}

c16_word op_or(c16_word p1,c16_word p2){
    return p1 | p2;
}

c16_word op_xand(c16_word p1,c16_word p2){
    return ~(p1 ^ p2);
}

c16_word op_xor(c16_word p1,c16_word p2){
    return p1 ^ p2;
}

c16_word op_lshift(c16_word p1,c16_word p2){
    return p1 << p2;
}

c16_word op_rshift(c16_word p1,c16_word p2){
    return p1 >> p2;
}

c16_word op_add(c16_word p1,c16_word p2){
    return p1 + p2;
}

c16_word op_sub(c16_word p1,c16_word p2){
    return p1 - p2;
}

c16_word op_mul(c16_word p1,c16_word p2){
    return p1 * p2;
}

c16_word op_div(c16_word p1,c16_word p2){
    return p1 / p2;
}

c16_word op_mod(c16_word p1,c16_word p2){
    return p1 % p2;
}

c16_word op_min(c16_word p1,c16_word p2){
    return (p1 < p2) ? p1 : p2;
}

c16_word op_max(c16_word p1,c16_word p2){
    return (p1 > p2) ? p1 : p2;
}

// comparison operations:

c16_word op_gte(c16_word p1,c16_word p2){
    return p1 >= p2;
}

c16_word op_lte(c16_word p1,c16_word p2){
    return p1 <= p2;
}

c16_word op_eq(c16_word p1,c16_word p2){
    return p1 == p2;
}

c16_word op_neq(c16_word p1,c16_word p2){
    return p1 != p2;
}

c16_word op_gt(c16_word p1,c16_word p2){
    return p1 > p2;
}

c16_word op_lt(c16_word p1,c16_word p2){
    return p1 < p2;
}

c16_word op_inv(c16_word p){
    return ~p;
}

c16_word op_inc(c16_word p){
    return ++p;
}

c16_word op_dec(c16_word p){
    return --p;
}

c16_word op_set(c16_word p){
    return p;
}
