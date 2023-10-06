RESULT = vm.elf

GDBFILE = ./gdbinit

CC = gcc
GDB = gdb

CFLAGS = -g -Wall -Wextra -pedantic -O2
GDBFLAGS = --nx --command=$(GDBFILE)
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

.PHONY: clean cleansetup setup run debug compile

all:
	@make clean
	@make vm
	@make run

vm: $(BIN_DIR)/$(RESULT)

$(BIN_DIR)/$(RESULT): $(OBJ_FILES)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf $(OBJ_DIR)
	@rm -f $(BIN_DIR)/$(RESULT)
	@mkdir -p $(OBJ_DIR)

setup:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)
	@echo set disassembly-flavor intel > $(GDBFILE)
	@echo file $(BIN_DIR)/$(RESULT) >> $(GDBFILE)
	@echo b main >> $(GDBFILE)
	@echo r >> $(GDBFILE)

cleansetup:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@rm -f $(GDBFILE)

compile:
	@rm -f cuak.bin
	@gcc compile.c -o compile
	@./compile ./cuak.bin

run:
	@$(BIN_DIR)/$(RESULT) ./cuak.bin

trace:
	@make clean
	@make vm
	@strace $(BIN_DIR)/$(RESULT) ./cuak.bin
	
debug:
	@$(GDB) $(GDBFLAGS) $(BIN_DIR)/$(RESULT)

reset:
	@make clean
	@make cleansetup
	@make setup
	@make vm
	@make debug