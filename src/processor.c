/* processor.c --- processor simulation for the 16candles VM debugger.
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

#include "machine/processor.h"
#include "debug.h"

// Initializes all registers and subregisters.
void init_regs(){
    c16_halfword *rs = calloc(32,sizeof(c16_halfword));
    ipt   = (c16_reg) &rs[0];
    spt   = (c16_reg) &rs[2];
    ac1   = (c16_reg) &rs[4];
    ac2   = (c16_reg) &rs[6];
    tst   = (c16_reg) &rs[8];
    inp   = (c16_reg) &rs[10];
    inp_r = &rs[10];
    inp_w = &rs[11];
    r0    = (c16_reg) &rs[12];
    r0_f  = &rs[13];
    r0_b  = &rs[12];
    r1    = (c16_reg) &rs[14];
    r1_f  = &rs[15];
    r1_b  = &rs[14];
    r2    = (c16_reg) &rs[16];
    r2_f  = &rs[17];
    r2_b  = &rs[16];
    r3    = (c16_reg) &rs[18];
    r3_f  = &rs[19];
    r3_b  = &rs[20];
    r4    = (c16_reg) &rs[20];
    r4_f  = &rs[21];
    r4_b  = &rs[29];
    r5    = (c16_reg) &rs[22];
    r5_f  = &rs[23];
    r5_b  = &rs[22];
    r6    = (c16_reg) &rs[24];
    r6_f  = &rs[25];
    r6_b  = &rs[24];
    r7    = (c16_reg) &rs[26];
    r7_f  = &rs[27];
    r7_b  = &rs[26];
    r8    = (c16_reg) &rs[28];
    r8_f  = &rs[29];
    r8_b  = &rs[28];
    r9    = (c16_reg) &rs[30];
    r9_f  = &rs[32];
    r9_b  = &rs[31];
}

void free_regs(){
    free(ipt);
}

// Fills the register with the next word at the ipt.
// WARNING: Do not call on ipt, intermediate reading will corrupt the value.
void fill_word(c16_reg reg){
    *reg =  (c16_word) sysmem.mem[(*ipt)++] << 8;
    *reg += sysmem.mem[(*ipt)++];
}

// Prints "machine: %c" and the value to help decipher the output.
void debugging_op_write(c16_opcode op){
    c16_halfword reg1;
    c16_word c;
    switch(op % 2){
    case LIT:
        fill_word(ac1);
        printf("machine: %c\n",*ac1);
        return;
    case REG:
        reg1 = sysmem.mem[(*ipt)++];
        if (reg1 > OP_r9){
            if ((c = *((c16_subreg) parse_reg(reg1))) < 128){
                printf("machine: %c\n",c);
            }
            return;
        }
        if ((c = *((c16_reg) parse_reg(reg1))) < 128){
            printf("machine: %c\n",c);
        }
    }
}

// simulate one processor tick
// return: -1 if an exit opcode was encountered
int proc_tick(){
    c16_opcode op = sysmem.mem[(*ipt)++];
    if (op == OP_TERM){ // exit case
        return -1;
    }
    if (op <= OP_MAX_REG_REG){             // binary operators
        op_bin_ops(op);
    }else if (op <= OP_LT_REG_REG){        // comparison operators
        op_cmp_ops(op);
    }else if (op <= OP_SET_REG){           // unary operators
        op_un_ops(op);
    }else if ((op >> 1) << 1 == OP_PUSH_){ // op push
        op_push(op);
    }else if (op <= OP_JMPF){              // jump operations
        op_jmp(op);
    }else if (op <= OP_WRITE_REG){         // escaping write
        debugging_op_write(op);            // DEBUGGING VERSION!
    }else if (op <= OP_MSET_MEMREG){       // memset operations
        op_mset(op);
    }else if (op == OP_SWAP){              // swap operator
        op_swap();
    }else if (op == OP_POP){               // pop operator
        op_pop();
    }else if (op == OP_PEEK){              // peek operator
        op_peek();
    }else if (op == OP_FLUSH){             // flush the stack operator
        op_flush();
    }else if (op == OP_READ){              // escaping read
        op_read();
    }

    return 0;
}

// Returns the register from the given byte.
// return: The reg or subreg that the opcode describes.
void *parse_reg(c16_halfword reg){
    switch(reg){
    case OP_ipt:   return ipt;
    case OP_spt:   return spt;
    case OP_ac1:   return ac1;
    case OP_ac2:   return ac2;
    case OP_tst:   return tst;
    case OP_inp:   return inp;
    case OP_inp_r: return inp_r;
    case OP_inp_w: return inp_w;
    case OP_r0:    return r0;
    case OP_r0_f:  return r0_f;
    case OP_r0_b:  return r0_b;
    case OP_r1:    return r1;
    case OP_r1_f:  return r1_f;
    case OP_r1_b:  return r1_b;
    case OP_r2:    return r2;
    case OP_r2_f:  return r2_f;
    case OP_r2_b:  return r2_b;
    case OP_r3:    return r3;
    case OP_r3_f:  return r3_f;
    case OP_r3_b:  return r3_b;
    case OP_r4:    return r4;
    case OP_r4_f:  return r4_f;
    case OP_r4_b:  return r4_b;
    case OP_r5:    return r5;
    case OP_r5_f:  return r5_f;
    case OP_r5_b:  return r5_b;
    case OP_r6:    return r6;
    case OP_r6_f:  return r6_f;
    case OP_r6_b:  return r6_b;
    case OP_r7:    return r7;
    case OP_r7_f:  return r7_f;
    case OP_r7_b:  return r7_b;
    case OP_r8:    return r8;
    case OP_r8_f:  return r8_f;
    case OP_r8_b:  return r8_b;
    case OP_r9:    return r9;
    case OP_r9_f:  return r9_f;
    case OP_r9_b:  return r9_b;
    default:       return NULL;
    }
}

// Process the stdin in a second thread.
// pass a NULL, it isn't used.
void *process_stdin(void *_){
    unsigned char c;
    while (read(pipe_fds[0],&c,sizeof(unsigned char))  != 0){
        sysmem.inputv[(*inp_w)++] = (c16_halfword) c;
        if (*sysmem.inputc < 256){
            ++(*sysmem.inputc);
        }
    }
    *sysmem.inputb = 0;
    return NULL;
}
