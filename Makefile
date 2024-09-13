# Main Makefile

# Make Makefile Silent (without repeating @ before commands)
ifndef VERBOSE
.SILENT:
endif


SECTOR_SIZE := 512

KERNEL_LOAD_ADDR := 0xF000
KERNEL_STACK_START_ADDR := $(KERNEL_LOAD_ADDR)

### Directories
BOOT_DIR := boot
KERNEL_DIR := kernel
BUILD_DIR := build
LIBC_DIR := libc
OBJ_DIR := build/obj

### Files
# Boot
BOOT_S := $(BOOT_DIR)/boot.s
BOOT_BIN := $(BUILD_DIR)/boot.bin

# Bochs
BOCHS_CONFIG_ORG = ./bochs_config
BOCHS_CONFIG = ./bochs_config_temp

# Kernel
KERNEL_S := $(shell find $(KERNEL_DIR) -name '*.s') # Take all asm files in dir via $(wildcard $(KERNEL_DIR)/*.s)
# KERNEL_C := $(KERNEL_DIR)/kernel.c # Take all C files in dir via $(wildset c style compile flagscard $(KERNEL_DIR)/*.c)
KERNEL_C := $(shell find $(KERNEL_DIR) -name '*.c')
KERNEL_LD_ORG := $(KERNEL_DIR)/klink.ld
KERNEL_LD := $(BUILD_DIR)/klink_temp.ld
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BUILD_DIR)/kernel.bin
KERNEL_INCLUDE := $(KERNEL_DIR)/include

# Libc
LIBC_S := $(shell find $(LIBC_DIR) -name '*.s')
LIBC_C := $(shell find $(LIBC_DIR) -name '*.c')
LIBC_INCLUDE := $(LIBC_DIR)/include

# Floppy
FLOPPY_BIN := $(BUILD_DIR)/bios.img

### Sources & Objects (example: kernel/kernel_entry.s -> build/obj/kernel/kernel_entry.o)
SRC_S := $(KERNEL_S)
SRC_C := $(KERNEL_C) # $(LIBC_C)

OBJ_LIBC := $(patsubst %, $(OBJ_DIR)/%, $(LIBC_C:.c=.libc.o))
OBJ_LIBK := $(patsubst %, $(OBJ_DIR)/%, $(LIBC_C:.c=.libk.o)) $(patsubst %, $(OBJ_DIR)/%, $(LIBC_S:.s=.libk.o)) 
# OBJ_KERNEL := $(patsubst %, $(OBJ_DIR)/%, $(KERNEL_S:.s=.o)) $(patsubst %, $(OBJ_DIR)/%, $(KERNEL_C:.c=.o))
KERNEL_OBJ := $(patsubst %, $(OBJ_DIR)/%, $(SRC_S:.s=.o)) \
		$(patsubst %, $(OBJ_DIR)/%, $(SRC_C:.c=.o)) \
		$(OBJ_LIBK)

### Constants
SHELL := /bin/bash
CC := x86_64-elf-gcc
DD := dd
NASM := nasm
LD := x86_64-elf-ld

INCLUDES := -I$(LIBC_INCLUDE) -I$(KERNEL_INCLUDE)
MISC_FLAGS = -DKERNEL_STACK_START_ADDR=$(KERNEL_STACK_START_ADDR) -DCURRENT_YEAR=$(shell $(SHELL) -c "date -u +%Y")
ifdef DEBUG
MISC_FLAGS += -DDEBUG=\"DEBUG\"
endif
CFLAGS := -ffreestanding -m64 -masm=intel -Wall -O0 -g -Wextra -std=c11 -mpreferred-stack-boundary=4 -mstackrealign $(INCLUDES) $(MISC_FLAGS)
NASMFLAGS := -f elf64 -g
LDFLAGS := -T $(KERNEL_LD) $(INCLUDES)
LIBC_FLAGS := $(CFLAGS) -D__is_libc
LIBK_FLAGS := $(CFLAGS) -D__is_libk

# -----------------------------------------------

.PHONY: all build boot always run clean

all: build

# Build Disk (Floppy Image)
build: $(FLOPPY_BIN)
$(FLOPPY_BIN): kernel boot
	# Write zeroes to disk
	$(DD) if=/dev/zero of=$@ conv=notrunc,fsync count=65536
	# Write bootloader to disk
	$(DD) if=$(BOOT_BIN) of=$@ conv=notrunc,fsync
	# Write kernel to disk
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
		-DKERNEL_STACK_START_ADDR=$(KERNEL_STACK_START_ADDR) \
		-f bin -o $@

# Kernel
kernel: $(KERNEL_BIN)

$(KERNEL_BIN): $(KERNEL_ELF)
	objcopy -O binary $(KERNEL_ELF) $@

$(KERNEL_ELF): always $(KERNEL_OBJ)
	echo "Linking Kernel..."
	sed 's/$$(KERNEL_LOAD_ADDR)/$(KERNEL_LOAD_ADDR)/g' $(KERNEL_LD_ORG) > $(KERNEL_LD) && sync
	$(LD) $(LDFLAGS) $(KERNEL_OBJ) -o $@
	rm $(KERNEL_LD)

# Objects (Assembling)
$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(NASM) $< $(NASMFLAGS) -o $@

# Objects (Compiling)
$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Libk Objects (Assembling)
$(OBJ_DIR)/%.libk.o: %.s
	mkdir -p $(dir $@)
	$(NASM) $< $(NASMFLAGS) -o $@

# Libk Object (Compiling)
$(OBJ_DIR)/%.libk.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(LIBK_FLAGS) -c $< -o $@

# ------------------------------------------------

always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/kernel

run:
	qemu-system-x86_64 -drive format=raw,file=$(FLOPPY_BIN) -smp cores=4 -m 4G -nic user,model=virtio -no-reboot 
#-d int,cpu,cpu_reset

run_debugger: 
	qemu-system-x86_64 -drive format=raw,file=$(FLOPPY_BIN) -d int,cpu_reset -s -S 

run_debug_bochs:
	sed 's#$$(FLOPPY_BIN)#$(FLOPPY_BIN)#g' $(BOCHS_CONFIG_ORG) > $(BOCHS_CONFIG) && sync
	bochs -qf $(BOCHS_CONFIG)
	rm -f $(BOCHS_CONFIG)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *_tempwhy
