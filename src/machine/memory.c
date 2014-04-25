/* memory.c --- memory for the 16candles VM
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

#include "memory.h"
#include "register.h"

const unsigned int C16_MEM_WIDTH = 0x10000;
const c16_word C16_VIRTMEM_START = 0x8000; // virtmem starts halfway through
const c16_word C16_VIRTMEM_WIDTH = 0x8000;
const char const* C16_DEFAULT_FILENAME = "/tmp/16c";
c16_word stack_start;

// initialize a memory, creating the default virtual memory file
void init_mem_default(c16_mem* const mem){
    init_mem(mem,C16_DEFAULT_FILENAME);
}

// initialize a memory struct, creating the specified virtual memory file
void init_mem(c16_mem* const mem,const char* const filename){
    int n;
    mem->filename = filename;
    mem->fd = open(filename,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (!mem->fd){
        perror("init_mem: open");
    }
    fallocate(mem->fd,0,0,C16_MEM_WIDTH);
    mem->mem        = mmap(NULL,C16_MEM_WIDTH,PROT_READ | PROT_WRITE,
                           MAP_SHARED,mem->fd,0);
    mem->virtmem    = mem->mem + C16_VIRTMEM_START;
    mem->inputv     = mem->virtmem;
    mem->inputc     = &mem->inputv[256];
    mem->inputb     = &mem->inputv[257];
    *mem->inputc    = 0; // Reset the input count
    *mem->inputb    = 1; // Set the input to "have more"
}

// free a memory struct, unmapping the virtual memory file
void free_mem(c16_mem *const mem){
    munmap(mem->mem,C16_MEM_WIDTH);
    // unlink(mem->filename); Cannot decide if I should unlink this.
    close(mem->fd);
}

// load a file from the host machine into the vm's ram starting at the
// specified location
void load_file(const c16_mem* const mem,c16_word start_loc,FILE *f){
    struct stat *st = malloc(sizeof(struct stat));
    fstat(fileno(f),st);
    stack_start = st->st_size;
    stack_start = fread(mem->mem + start_loc,1,C16_MEM_WIDTH - start_loc,f);
    *spt        = stack_start;
}
