#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include "src/vm/iset.h"

int64_t PROGRAM[] = {
    VM_ISET_PUSH, 0x0000,
    VM_ISET_PUSH, -1,
    VM_ISET_PUSH, (MAP_PRIVATE | MAP_ANONYMOUS),
    VM_ISET_PUSH, (PROT_WRITE | PROT_READ),
    VM_ISET_PUSH, 0x0400,
    VM_ISET_PUSH, 0x0000,
    VM_ISET_HOSTCALL, 0x0009,
    VM_ISET_STORE, 0x0010,
    
    VM_ISET_LOAD, 0x0010,
    VM_ISET_STORE, 0x0001,

    VM_ISET_LOAD, 0x0001,
    VM_ISET_PUSH, 0x754f20616c6f4821,
    VM_ISET_PUSH, 0x8,
    VM_ISET_HOSTWRITE, VM_ISET_NOP,

    VM_ISET_LOAD, 0x0001,
    VM_ISET_PUSH, 0x8,
    VM_ISET_ADD, VM_ISET_NOP,
    VM_ISET_PUSH, 0x0a216365736e6572,
    VM_ISET_PUSH, 0x8,
    VM_ISET_HOSTWRITE, VM_ISET_NOP,

    VM_ISET_PUSH, 0x0000,
    VM_ISET_PUSH, 0x0000,
    VM_ISET_PUSH, 0x0000,
    VM_ISET_PUSH, 0x0011,
    VM_ISET_LOAD, 0x0010,
    VM_ISET_PUSH, 0x0001,
    VM_ISET_HOSTCALL, 0x0001,
    VM_ISET_HALT
};

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    //Open file to write the buffer
    FILE* f = fopen(argv[1], "wb");
    if (!f) {
        printf("Failed to open file %s\n", argv[1]);
        return 1;
    }

    //Write the buffer
    fwrite(PROGRAM, sizeof(PROGRAM), 1, f);
    fclose(f);
}