/* debug.c --- debugging utility for the 16candles vm.
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

#include "debug.h"

// The stdin pipe for the machine.
int       pipe_fds[2];

// The jump environment.
jmp_buf   jump;

// The input thread.
pthread_t input_thread;

// A list of all the valid commands.
command commands[COMMAND_COUNT] ={
    { "step",cmd_step,0,
      "step            Step the ipt over one full operation"                  },
    { "s",   cmd_step,0,
      "s               Alias of `step`"                                       },
    { "dump",cmd_dump,0,
      "dump            Prints the values stored in every register"            },
    { "d",   cmd_dump,0,
      "d               Alias of `dump`"                                       },
    { "reg", cmd_reg,1,
      "reg REG         Prints the value stored in register REG"               },
    { "mem", cmd_mem,2,
      "mem w|h ADR|REG Prints the word (w) or halfword (h) in mem at ADR|REG" },
    { "inp", cmd_inp,1,
      "inp STR         Feeds STR to the virtual machines standard input"      },
    { "help",cmd_help,0,
      "help            Prints this message"                                   },
    { "?",   cmd_help,0,
      "?               Alias of `help`"                                       },
    { "quit",cmd_quit,0,
      "quit            Exits the debugging repl session"                      },
    { NULL,NULL,0,NULL                                                        }
};

// The handler for when the machine crashes.
void sigsegv_handler(int sig){
    char b[30];
    snprintf(b,30 * sizeof(char),"SIGSEGV caught: ipt = 0x%04x\n",*ipt);
    write(STDOUT_FILENO,b,30 * sizeof(char));
    siglongjmp(jump,1);
}

// Converts an opcode into the string command.
const char *cmdstr(c16_halfword op, bool use_symbols){
    static const char* const bin_ops[][2] = { { "and",   "&&" },
                                              { "or",    "||" },
                                              { "xand",  "!&" },
                                              { "xor",   "!|" },
                                              { "lshift","<<" },
                                              { "rshift",">>" },
                                              { "add",   "+"  },
                                              { "sub",   "-"  },
                                              { "mul",   "*"  },
                                              { "div",   "/"  },
                                              { "mod",   "%"  },
                                              { "min",   ""   },
                                              { "max",   ""   } };
    size_t      subindex = (use_symbols) ? 1 : 0;
    int         n;
    const char *r;
    if (op <= OP_MAX_REG_REG){
        op /= 4;
        r = bin_ops[op][subindex];
        if (!r[0]){
            r = bin_ops[op][0];
        }
        return r;
    }else if (op == OP_SET_LIT || op == OP_SET_REG){
        return (use_symbols) ? "=" : "set";
    }else if (op <= OP_LT_REG_REG || op <= OP_SET_REG){
        op /= 2;
        switch(op){
        case OP_INV_:
            return (use_symbols) ? "~"  : "inv";
        case OP_INC_:
            return (use_symbols) ? "++" : "inc";
        case OP_DEC_:
            return (use_symbols) ? "--" : "dec";
        case OP_GT_:
            return (use_symbols) ? ">"  : "gt";
        case OP_LT_:
            return (use_symbols) ? "<"  : "lt";
        case OP_GTE_:
            return (use_symbols) ? ">=" : "gte";
        case OP_LTE_:
            return (use_symbols) ? "<=" : "lte";
        case OP_EQ_:
            return (use_symbols) ? "==" : "eq";
        case OP_NEQ_:
            return (use_symbols) ? "!=" : "neq";
        }
    }else if ((op >> 1) << 1 == OP_PUSH_){
        return (use_symbols) ? ":" : "push";
    }else if (op <= OP_JMPF){
        switch(op){
        case OP_JMP:
            return (use_symbols) ? "=>" : "jmp";
        case OP_JMPT:
            return (use_symbols) ? "->" : "jmpt";
        case OP_JMPF:
            return (use_symbols) ? "<-" : "jmpf";
        }
    }else if (op <= OP_WRITE_REG){
        return "write";
    }else if (op <= OP_MSET_MEMREG){
        return (use_symbols) ? ":=" : "mset";
    }else if (op == OP_SWAP){
        return (use_symbols) ? "\\\\" : "swap";
    }else if (op == OP_POP){
        return (use_symbols) ? "$" : "pop";
    }else if (op == OP_PEEK){
        return (use_symbols) ? "@" : "peek";
    }else if (op == OP_FLUSH){
        return (use_symbols) ? "#" : "flush";
    }else if (op == OP_READ){
        return "read";
    }else if (op == OP_NOP){
        return "nop";
    }
    return "";
}

// Evaluate a line of user input.
void eval_line(char *s){
    int n,e = 0;
    char *t;
    command *cmd;
    static char *argv[256];
    memset(argv,0,256 * sizeof(char*));
    t = strtok(s," ");
    cmd = resolve_cmd(t);
    if (!cmd){
        printf("error: '%s': not valid command\n",s);
        return;
    }
    if (!strcmp(cmd->name,"inp")){
        argv[0] = &s[4] ;
        cmd->func(argv);
        return;
    }
    for (n = 0;n < 256;n++){
        argv[n] = strtok(NULL," ");
        if (!argv[n] && n < cmd->argc){
            printf("error: command `%s` expects %d arguments but recieved %d\n",
                   cmd->name,cmd->argc,n);
            return;
        }else if (argv[n] && n >= cmd->argc){
            ++e;
        }
    }
    if (e){
        printf("error: command `%s` expects %d arguments but recieved %d\n",
               cmd->name,cmd->argc,cmd->argc + e);
        return;
    }
    cmd->func(argv);
}

// Parses the command out of the command name.
// return: The pointer to the command typed, or NULL if it is not a command.
command *resolve_cmd(char *s){
    int n;
    for (n = 0;commands[n].name != NULL;n++){
        if (!strcmp(s,commands[n].name)){
            return &commands[n];
        }
    }
    return NULL;
}

// Prints a welcome message, then begins the repl.
void start_debug_repl(){
    printf("16cdb 0.0.0.1 (2014.3.26)\nWelcome to the 16 candles debugger:\n\
type `help` to see a list of commands\n");
    repl();
}

// The read eval print loop.
void repl(){
    char *s;
    do{
        s = readline("> ");
        add_history(s);
        eval_line(s);
        free(s);
    }while(s);
}

int main(int argc,char **argv){
    FILE *in;
    int n,cs = 0;
    pipe(pipe_fds);
    signal(SIGSEGV,sigsegv_handler);
    init_regs();
    init_mem(&sysmem,"/tmp/16cdb");
    if (argc == 1){
        puts("Usage: 16cdb BINARY");
        return 0;
    }
    in = fopen(argv[1],"r");
    if (!in){
        fprintf(stderr,"Error: Unable to open file '%s'\n",argv[1]);
        return -1;
    }
    load_file(&sysmem,0,in);
    pthread_create(&input_thread,NULL,process_stdin,NULL);
    start_debug_repl();
    return EXIT_SUCCESS;
}
