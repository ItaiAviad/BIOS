
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
KERNEL_LD := $(KERNEL_DIR)/klink.ld
KERNEL_BIN := $(BUILD_DIR)/kernel.bin

FLOPPY_BIN := $(BUILD_DIR)/bios.img

# Constants
CC := x86_64-elf-gcc
CFLAGS := -ffreestanding
DD := dd
NASM := nasm
LD := x86_64-elf-ld
LDFLAGS := -T $(KERNEL_LD)

SRC_S := $(KERNEL_S)
SRC_C := $(KERNEL_C)

# Objects (example: kernel/kernel_entry.s -> build/obj/kernel/kernel_entry.o)
OBJ := $(patsubst %, $(OBJ_DIR)/%, $(SRC_S:.s=.o)) $(patsubst %, $(OBJ_DIR)/%, $(SRC_C:.c=.o))

# -----------------------------------------------

.PHONY: all build boot always run clean

all: build

# Build Disk (Floppy Image)
build: $(FLOPPY_BIN)
$(FLOPPY_BIN): boot kernel
	$(DD) conv=notrunc bs=512 seek=0 of=$@ if=/dev/zero count=5620
	$(DD) conv=notrunc bs=512 seek=0 of=$@ if=$(BOOT_BIN)
	$(DD) conv=notrunc bs=512 seek=3 of=$@ if=$(KERNEL_BIN) count=1

# Bootloader
boot: $(BOOT_BIN)
$(BOOT_BIN): always
	$(NASM) $(BOOT_S) -I $(BOOT_DIR) -f bin -o $@

# Kernel
kernel: $(KERNEL_BIN)
$(KERNEL_BIN): always $(OBJ)
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
