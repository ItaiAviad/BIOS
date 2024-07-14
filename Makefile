# Project's make file

SECTOR_SIZE := 512

# Directories
BOOT_DIR := boot
KERNEL_DIR := kernel
BUILD_DIR := build
OBJ_DIR := build/obj

# Files
BOOT_S := $(BOOT_DIR)/boot.s
BOOT_BIN := $(BUILD_DIR)/boot.bin

KERNEL_S := $(KERNEL_DIR)/kernel_entry.s
KERNEL_C := $(KERNEL_DIR)/kernel.c
KERNEL_LD_ORG := $(KERNEL_DIR)/klink_org.ld
KERNEL_LD := $(BUILD_DIR)/klink.ld
KERNEL_BIN := $(BUILD_DIR)/kernel.bin

FLOPPY_BIN := $(BUILD_DIR)/bios.img

# Constants
SHELL := /bin/bash
CC := @x86_64-elf-gcc
CFLAGS := -ffreestanding -m64 -masm=intel
DD := @dd
NASM := @nasm
LD := @x86_64-elf-ld
LDFLAGS := -T $(KERNEL_LD)

SRC_S := $(KERNEL_S)
SRC_C := $(KERNEL_C)

# KERNEL_SOURCES_ASM = $(KERNEL_DIR)/entry.s 
# KERNEL_SOURCES_CPP = $(wildcard $(KERNEL_DIR)/*.cpp) # Can be replaced with file1.cpp file2.cpp ...
# KERNEL_SOURCES = $(KERNEL_SOURCES_ASM) $(KERNEL_SOURCES_CPP)
# KERNEL_OBJECTS_CPP = $(patsubst $(KERNEL_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES_CPP))
# KERNEL_OBJECTS_ASM = $(patsubst $(KERNEL_DIR)/%.s,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES_ASM))

# Objects (example: kernel/kernel_entry.s -> build/obj/kernel/kernel_entry.o)
OBJ := $(patsubst %, $(OBJ_DIR)/%, $(SRC_S:.s=.o)) $(patsubst %, $(OBJ_DIR)/%, $(SRC_C:.c=.o))

KERNEL_LOAD_ADDR := 0x8C00

# -----------------------------------------------

.PHONY: all build boot always run clean

all: build

# Build Disk (Floppy Image)
build: $(FLOPPY_BIN)
$(FLOPPY_BIN): kernel boot
	$(DD) conv=notrunc bs=512 seek=0 of=$@ if=/dev/zero count=5620
	$(DD) conv=notrunc bs=512 seek=0 of=$@ if=$(BOOT_BIN)
	$(DD) conv=notrunc bs=512 seek=3 of=$@ if=$(KERNEL_BIN) count=1

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
	sed 's/$(KERNEL_LOAD_ADDR)/$$(KERNEL_LOAD_ADDR)/g $(KERNEL_LD_ORG)' > $(KERNEL_LD)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

# Objects (Assembling)
$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(NASM) $< -f elf64 -o $@

# Objects (Compiling)
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------------------------------

always:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/kernel

run:
	@qemu-system-x86_64 -drive format=raw,file=$(FLOPPY_BIN)

run_debug_bochs:
	@bochs -qf bochs_config

clean:
	@rm -rf $(BUILD_DIR)/*
