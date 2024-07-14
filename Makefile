# Project's Makefile

# Now our Makefile is silent without repeating @ before commands
ifndef VERBOSE
.SILENT:
endif

SECTOR_SIZE := 512

# Directories
BOOT_DIR := boot
KERNEL_DIR := kernel
BUILD_DIR := build
OBJ_DIR := build/obj

# Files
BOOT_S := $(BOOT_DIR)/boot.s
BOOT_BIN := $(BUILD_DIR)/boot.bin

KERNEL_S := $(KERNEL_DIR)/kernel_entry.s # Can add all the assembly files in a dir via $(wildcard $(KERNEL_DIR)/*.s)
KERNEL_C := $(KERNEL_DIR)/kernel.c # Can add all the c files in a dir via $(wildcard $(KERNEL_DIR)/*.c) 
KERNEL_LD_ORG := $(KERNEL_DIR)/klink.ld
KERNEL_LD := $(BUILD_DIR)/klink_temp.ld
KERNEL_BIN := $(BUILD_DIR)/kernel.bin

FLOPPY_BIN := bios.img

# Constants
SHELL := /bin/bash
CC := x86_64-elf-gcc
CFLAGS := -ffreestanding -m64 -masm=intel
DD := dd
NASM := nasm
LD := x86_64-elf-ld
LDFLAGS := -T $(KERNEL_LD)

SRC_S := $(KERNEL_S)
SRC_C := $(KERNEL_C)

# Objects (example: kernel/kernel_entry.s -> build/obj/kernel/kernel_entry.o)
OBJ := $(patsubst %, $(OBJ_DIR)/%, $(SRC_S:.s=.o)) $(patsubst %, $(OBJ_DIR)/%, $(SRC_C:.c=.o))

KERNEL_LOAD_ADDR := 0x8C00

BOCHS_CONFIG_ORG = ./bochs_config
BOCHS_CONFIG = ./bochs_config_temp



# -----------------------------------------------

.PHONY: all build boot always run clean

all: build

# Build Disk (Floppy Image)
build: $(FLOPPY_BIN)

$(FLOPPY_BIN): kernel boot
	$(DD) if=$(BOOT_BIN) of=$@ conv=notrunc,fsync
	$(DD) if=$(KERNEL_BIN) of=$@ bs=1 seek=$$(stat -c %s $(BOOT_BIN)) conv=notrunc,fsync
	FILE_SIZE=$$(stat -c %s $@); \
	PADDING=$$(( $(SECTOR_SIZE) - FILE_SIZE % $(SECTOR_SIZE) )); \
	$(DD) if=/dev/zero of=$@ bs=1 seek=$$FILE_SIZE count=$$PADDING conv=notrunc,fsync

# Bootloader
boot: $(BOOT_BIN)
$(BOOT_BIN): always kernel
	$(NASM) $(BOOT_S) -I $(BOOT_DIR)  \
		-DSECTOR_SIZE=$(SECTOR_SIZE) \
		-DKERNEL_SIZE_IN_SECTORS=$(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s $(KERNEL_BIN)) + $(SECTOR_SIZE) -1 ) / $(SECTOR_SIZE)))') \
		-DKERNEL_LOAD_ADDR=$(KERNEL_LOAD_ADDR) \
		-f bin -o $@

# Kernel
kernel: $(KERNEL_BIN)
$(KERNEL_BIN): always $(OBJ)
	sed 's/$$(KERNEL_LOAD_ADDR)/$(KERNEL_LOAD_ADDR)/g' $(KERNEL_LD_ORG) > $(KERNEL_LD)
	$(LD) $(LDFLAGS) $(OBJ) -o $@
	rm $(KERNEL_LD) 

# Objects (Assembling)
$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(NASM) $< -f elf64 -o $@

# Objects (Compiling)
$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------------------------------

always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/kernel

run:
	qemu-system-x86_64 -drive format=raw,file=$(FLOPPY_BIN)

run_debug_bochs:
	sed 's/$$(FLOPPY_BIN)/$(FLOPPY_BIN)/g' $(BOCHS_CONFIG_ORG) > $(BOCHS_CONFIG)
	bochs -qf $(BOCHS_CONFIG)
	rm $(BOCHS_CONFIG)

clean:
	rm -rf $(BUILD_DIR)/*
