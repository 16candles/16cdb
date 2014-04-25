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

#include "commands.h"

// The stdin pipe for the machine.
int       pipe_fds[2];

// The jump environment.
jmp_buf   jump;

// The stdin thread, used to mimic the stdin features on the machine.
pthread_t input_thread;

// The names of all the registers.
static char *reg_strs[] = { "ipt",
                            "spt",
                            "ac1",
                            "ac2",
                            "tst",
                            "inp",
                            "r0",
                            "r1",
                            "r2",
                            "r3",
                            "r4",
                            "r5",
                            "r6",
                            "r7",
                            "r8",
                            "r9",
                            "inp_r",
                            "inp_w",
                            "r0_f",
                            "r0_b",
                            "r1_f",
                            "r1_b",
                            "r2_f",
                            "r2_b",
                            "r3_f",
                            "r3_b",
                            "r4_f",
                            "r4_b",
                            "r5_f",
                            "r5_b",
                            "r6_f",
                            "r6_b",
                            "r7_f",
                            "r7_b",
                            "r8_f",
                            "r8_b",
                            "r9_f",
                            "r9_b" };

// Terminates the program.
void cmd_quit(char **_){
    pthread_cancel(input_thread);
    free_regs();
    free_mem(&sysmem);
    exit(0);
}

// Prints the state of the machines registers to stdout.
void cmd_dump(char **_){
    printf(
        "ipt: 0x%04x\n"
        "spt: 0x%04x\n"
        "ac1: 0x%04x\n"
        "ac2: 0x%04x\n"
        "tst: 0x%04x\n"
        "inp: 0x%04x: inp_w: 0x%02x, inp_r: 0x%02x\n"
        "r0:  0x%04x: r0_f:  0x%02x, r0_b:  0x%02x\n"
        "r1:  0x%04x: r1_f:  0x%02x, r1_b:  0x%02x\n"
        "r2:  0x%04x: r2_f:  0x%02x, r2_b:  0x%02x\n"
        "r3:  0x%04x: r3_f:  0x%02x, r3_b:  0x%02x\n"
        "r4:  0x%04x: r4_f:  0x%02x, r4_b:  0x%02x\n"
        "r5:  0x%04x: r5_f:  0x%02x, r5_b:  0x%02x\n"
        "r6:  0x%04x: r6_f:  0x%02x, r6_b:  0x%02x\n"
        "r7:  0x%04x: r7_f:  0x%02x, r7_b:  0x%02x\n"
        "r8:  0x%04x: r8_f:  0x%02x, r8_b:  0x%02x\n"
        "r9:  0x%04x: r9_f:  0x%02x, r9_b:  0x%02x\n",
        *ipt,*spt,*ac1,*ac2,*tst,*inp,*inp_w,*inp_r,
        *r0,*r0_f,*r0_b,*r1,*r1_f,*r1_b,*r2,*r2_f,*r2_b,*r3,*r3_f,*r3_b,
        *r4,*r4_f,*r4_b,*r5,*r5_f,*r5_b,*r6,*r6_f,*r6_b,*r7,*r7_f,*r7_b,
        *r8,*r8_f,*r8_b,*r9,*r9_f,*r9_b);
}

// Step the program through a single operation.
void cmd_step(char **_){
    if (sigsetjmp(jump,1) == 0){
        if (proc_tick()){
            puts("Read `term`, exited succesfully");
        }
    }
}

// Feeds input into the machine's standard in.
void cmd_inp(char **argv){
    char *esc;
    esc = escapestr(argv[0]);
    if (!esc){
        return;
    }
    write(pipe_fds[1],esc,strlen(esc));
    free(esc);
}

// Parses escape codes out of strings. eg: "\\n" -> "\n".
// Does not parse hex, octal, or unicode.
// malloc's a string, be sure to free it.
char *escapestr(char *src){
    size_t n,m,c = 0;
    size_t l     = strlen(src);
    char  *dest  = malloc((l + 1) * sizeof(char));
    static char esc[][2] = { { 'a','\a'  },
                             { 'b','\b'  },
                             { 'f','\f'  },
                             { 'n','\n'  },
                             { 'r','\r'  },
                             { 't','\t'  },
                             { 'v','\v'  },
                             { '\\','\\' },
                             { '\'','\'' },
                             { '"','"'   },
                             { '?','?'   },
                             { 'e','\e'  },
                             { '0','\0'  } };
    memset(dest,0,(l + 1) * sizeof(char));
    for (n = 0;n <= l;n++){
        if (src[n++] == '\\'){
            for (m = 0;m < 13;m++){
                if (src[n] == esc[m][0]){
                    dest[c++] = esc[m][1];
                    break;
                }
            }
            if (m == 13){
                printf("error: '\\%c' is not a valid escape sequence\n",src[n]);
                free(dest);
                return NULL;
            }
        }else{
            dest[c++] = src[--n];
        }
    }
    return dest;
}

// Parses the register number out of the string.
// return: the number, or REG_DNE if it does not exist.
c16_halfword parse_regno(char *s){
    c16_halfword n;
    puts(s);
    for (n = 0;n < REG_DNE;n++){
        if (!strcmp(s,reg_strs[n])){
            break;
        }
    }
    return n;
}

// Prints the value of the register on a new line.
void cmd_reg(char **argv){
    c16_halfword n;
    if ((n = parse_regno(argv[0])) == REG_DNE){
        printf("register '%s' does not exist\n",argv[0]);
        return;
    }
    if (n > OP_r9){
        printf("%s = 0x%02x\n",reg_strs[n],*((c16_subreg) parse_reg(n)));
    }else{
        printf("%s = 0x%04x\n",reg_strs[n],*((c16_reg) parse_reg(n)));
    }
}

// Parses the memaddr commands paramaters.
void cmd_mem(char **argv){
    char        *e;
    bool         b;
    long         l;
    c16_halfword r;
    if (!strcmp(argv[0],"w")){
        b = false;
    }else if (!strcmp(argv[0],"h")){
        b = true;
    }else{
        puts("Usage: mem w|h ADR|REG");
        return;
    }
    if ((r = parse_regno(argv[1])) != REG_DNE){
        print_memreg(argv[1],b);
        return;
    }
    l = strtol(argv[1],&e,0);
    if (!l && *e != '\0'){
        printf("memory address: '%s': does not  exist\n",argv[1]);
        return;
    }
    if (l < 0 || l > 0xffff){
        printf("memory address: 0x%x: is out of bounds\n",l);
        return;
    }
    print_memaddr((c16_word) l,b);
    return;
}

// Prints the value stored at a particular memory address.
// If true, then print as halfword, otherwise print as word.
void print_memaddr(c16_word a,bool b){
    printf("%s: *0x%04x = %0*x\n",(b) ? "halfword" : "word",a,(b) ? 2 : 4,
           (b) ? sysmem.mem[a] : *((c16_word*) &sysmem.mem[a]));
}

// Prints the value stored at the memory address regstr.
// If true, then print as halfword, otherwise print as word.
void print_memreg(char *regstr,bool w){
    c16_halfword reg = parse_regno(regstr);
    c16_word     v;
    int          b   = reg >= HALFREG_START;
    int          l   = (b) ? 2 : 4;
    void        *r   = parse_reg(reg);
    if (reg == REG_DNE){
        printf("register: '%s': does not exist\n");
        return;
    }
    v = (b) ? *((c16_subreg) r) : *((c16_reg) r);
    printf("%s: *%s (0x%0*x) = 0x%0*x\n",(w) ? "halfword" : "word",regstr,l,v,
           (b) ? 2 : 4,(b) ? sysmem.mem[v] : *((c16_word*) &sysmem.mem[v]));
}

// Prints the help message.
void cmd_help(char **_){
    int n;
    puts("Commands:");
    for (n = 0;commands[n].name != NULL;n++){
        printf("  %s\n",commands[n].help);
    }
}
