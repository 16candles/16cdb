/* memory.h --- memory for the 16candles VM
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

#include "../common/arch.h"

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef C16_MEM_H
#define C16_MEM_H

extern const unsigned int C16_MEM_WIDTH;
extern const c16_word C16_VIRTMEM_START;
extern const c16_word C16_VIRTMEM_WIDTH;
extern const char const* C16_DEFAULT_FILENAME;
extern c16_word stack_start;

typedef struct {
    c16_halfword *mem;
    c16_halfword *virtmem; // The virtual memory start, halfway through mem.
    c16_halfword *inputv;  // stdin buffer.
    c16_halfword *inputc;  // stdin buffer count.
    c16_halfword *inputb;  // is there more to read (EOF not hit).
    int           fd;
    const char*   filename;
} c16_mem;

// init memory with the default filename
void init_mem_default(c16_mem* const);

// init memory with a given filename
void init_mem(c16_mem* const,const char* const);

// free memory
void free_mem(c16_mem* const);

// load the contents of a file into the memory
void load_file(const c16_mem*,c16_word,FILE*);

#endif
