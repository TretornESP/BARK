#include "vm/vm.h"
#include "vm/iset.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

#define STACK_SIZE 128
#define MAX_LOCAL_VARS 128

void * load(const char* filename, size_t * size) {
    FILE * file = fopen(filename, "rb");
    if (!file) {
        printf("Could not open file: %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void * buffer = malloc(*size);
    if (!buffer) {
        printf("Could not allocate memory for file: %s\n", filename);
        fclose(file);
        return 0;
    }

    if (fread(buffer, 1, *size, file) != *size) {
        printf("Could not read file: %s\n", filename);
        fclose(file);
        free(buffer);
        return 0;
    }

    fclose(file);
    return buffer;
}

void crash(struct vm * vm, char* msg) {
    (void) vm;
    printf("VM Crashed!\n");
    printf("Message: %s\n", msg);
    vm_debug(vm, 1);

    while(1);
}

void breakpoint(struct vm * vm) {
    (void) vm;
    vm_debug(vm, 0);
    printf("breakpoint>");
    fflush(stdout);
    while (getchar() != '\n') {}
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        printf("Usage: %s <program>\n", argv[0]);
        return 1;
    }

    size_t size;
    void * program = load(argv[1], &size);
    if (!program) {
        return 1;
    }
    
    struct vm * vm = vm_create(program, (uint32_t)size, STACK_SIZE, MAX_LOCAL_VARS, crash);
    vm_run(vm, 0x0);
    //int64_t result = vm_result(vm);
    vm_destroy(vm);

    //printf("Result: %lx\n", result);
    return 0;
}