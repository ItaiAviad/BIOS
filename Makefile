# Main Makefile

# Make Makefile Silent (without repeating @ before commands)
ifndef VERBOSE
.SILENT:
endif


SECTOR_SIZE := 512

KERNEL_LOAD_ADDR := 0xF000
KERNEL_STACK_START_ADDR := $(KERNEL_LOAD_ADDR)
USER_LOAD_ADDR := 0x400F000

### Directories
BOOT_DIR := boot
KERNEL_DIR := kernel
USER_DIR := user
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

# User
USER_S := $(shell find $(USER_DIR) -name '*.s')
USER_C := $(shell find $(USER_DIR) -name '*.c')
USER_LD_ORG := $(USER_DIR)/ulink.ld
USER_LD := $(BUILD_DIR)/ulink_temp.ld
USER_ELF := $(BUILD_DIR)/user.elf
USER_BIN := build/user.bin
USER_INCLUDE := $(USER_DIR)/include

# Libc
LIBC_S := $(shell find $(LIBC_DIR) -name '*.s')
LIBC_C := $(shell find $(LIBC_DIR) -name '*.c')
LIBC_INCLUDE := $(LIBC_DIR)/include

# Floppy
FLOPPY_BIN := $(BUILD_DIR)/bios.img

### Sources & Objects (example: kernel/kernel_entry.s -> build/obj/kernel/kernel_entry.o)
KERNEL_SRC_S := $(KERNEL_S)
KERNEL_SRC_C := $(KERNEL_C) # $(LIBC_C)

USER_SRC_S := $(USER_S)
USER_SRC_C := $(USER_C)

OBJ_LIBC := $(patsubst %, $(OBJ_DIR)/%, $(LIBC_C:.c=.libc.o))
OBJ_LIBK := $(patsubst %, $(OBJ_DIR)/%, $(LIBC_C:.c=.libk.o)) $(patsubst %, $(OBJ_DIR)/%, $(LIBC_S:.s=.libk.o)) 
# OBJ_KERNEL := $(patsubst %, $(OBJ_DIR)/%, $(KERNEL_S:.s=.o)) $(patsubst %, $(OBJ_DIR)/%, $(KERNEL_C:.c=.o))
KERNEL_OBJ := $(patsubst %, $(OBJ_DIR)/%, $(KERNEL_SRC_S:.s=.o)) \
		$(patsubst %, $(OBJ_DIR)/%, $(KERNEL_SRC_C:.c=.o)) \
		$(OBJ_LIBK)
USER_OBJ := $(patsubst %, $(OBJ_DIR)/%, $(USER_SRC_S:.s=.o)) \
		$(patsubst %, $(OBJ_DIR)/%, $(USER_SRC_C:.c=.o))
		# $(OBJ_LIBC)

### Constants
SHELL := /bin/bash
CC := x86_64-elf-gcc
DD := dd
NASM := nasm
LD := x86_64-elf-ld

INCLUDES := -I$(LIBC_INCLUDE) -I$(KERNEL_INCLUDE)
# MISC_FLAGS = -DCURRENT_YEAR=$(shell date --utc | awk '{print $$7}')
# MISC_FLAGS += -DTIMEZONE=\"$(shell date --utc | awk '{print $$6}')\"
MISC_FLAGS = -DKERNEL_STACK_START_ADDR=$(KERNEL_STACK_START_ADDR) -DCURRENT_YEAR=$(shell $(SHELL) -c "date -u +%Y")
ifdef DEBUG
MISC_FLAGS += -DDEBUG=\"DEBUG\"
endif
CFLAGS := -ffreestanding -m64 -masm=intel -Wall -g -Wextra $(INCLUDES) $(MISC_FLAGS)
NASMFLAGS := -f elf64 -g
LDFLAGS_KERNEL := -T $(KERNEL_LD) $(INCLUDES)
LDFLAGS_USER := -T $(USER_LD) $(INCLUDES)
LIBC_FLAGS := $(CFLAGS) -D__is_libc
LIBK_FLAGS := $(CFLAGS) -D__is_libk

# -----------------------------------------------

.PHONY: all build boot always run clean

all: build

# Build Disk (Floppy Image)
build: $(FLOPPY_BIN)
$(FLOPPY_BIN): user kernel boot
	# Write zeroes to disk
	$(DD) if=/dev/zero of=$@ conv=notrunc,fsync count=65536
	# Write bootloader to disk
	$(DD) if=$(BOOT_BIN) of=$@ conv=notrunc,fsync
	# Write kernel to disk
	$(DD) if=$(KERNEL_BIN) of=$@ bs=1 seek=$$(stat -c %s $(BOOT_BIN)) conv=notrunc,fsync
	USER_SEEK=$$((($$(stat -c %s $(BOOT_BIN)) + $$(stat -c %s $(KERNEL_BIN))) / $(SECTOR_SIZE) * $(SECTOR_SIZE) + $(SECTOR_SIZE))); \
	$(DD) if=$(USER_BIN) of=$@ bs=1 seek=$$((USER_SEEK)) conv=notrunc,fsync
	FILE_SIZE=$$(stat -c %s $@); \
	PADDING=$$(( $(SECTOR_SIZE) - FILE_SIZE % $(SECTOR_SIZE) )); \
	$(DD) if=/dev/zero of=$@ bs=1 seek=$$FILE_SIZE count=$$PADDING conv=notrunc,fsync; \
	$(DD) if=/dev/zero of=$@ bs=$(SECTOR_SIZE) seek=$$(($$FILE_SIZE + $$PADDING)) count=2048 conv=notrunc,fsync

# Bootloader
boot: $(BOOT_BIN)
$(BOOT_BIN): always kernel user
	$(NASM) $(BOOT_S) -I $(BOOT_DIR)  \
		-DSECTOR_SIZE=$(SECTOR_SIZE) \
		-DKERNEL_SIZE_IN_SECTORS=$$(($(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s $(KERNEL_BIN)) + $(SECTOR_SIZE) -1 ) / $(SECTOR_SIZE)))'))) \
		-DTOTAL_SIZE_IN_SECTORS=$$(($(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s $(KERNEL_BIN)) + $(SECTOR_SIZE) -1 ) / $(SECTOR_SIZE)))') + $(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s $(USER_BIN)) + $(SECTOR_SIZE) -1 ) / $(SECTOR_SIZE)))'))) \
		-DKERNEL_LOAD_ADDR=$(KERNEL_LOAD_ADDR) \
		-DKERNEL_STACK_START_ADDR=$(KERNEL_STACK_START_ADDR) \
		-DUSER_LOAD_ADDR=$(USER_LOAD_ADDR) \
		-DUSER_SIZE=$$(($(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s $(USER_BIN)) + $(SECTOR_SIZE) -1 ) / $(SECTOR_SIZE) * $(SECTOR_SIZE)))'))) \
		-DUSER_SEEK_FROM_KERNEL_LOAD_ADDR=$$(($(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s $(KERNEL_BIN)) + $(SECTOR_SIZE) -1 ) / $(SECTOR_SIZE) * $(SECTOR_SIZE)))')))\
		-f bin -o $@

# Kernel
kernel: $(KERNEL_BIN)

$(KERNEL_BIN): $(KERNEL_ELF)
	objcopy -O binary $(KERNEL_ELF) $@

$(KERNEL_ELF): always $(KERNEL_OBJ)
	echo "Linking Kernel..."
	sed 's/$$(KERNEL_LOAD_ADDR)/$(KERNEL_LOAD_ADDR)/g' $(KERNEL_LD_ORG) > $(KERNEL_LD) && sync
	$(LD) $(LDFLAGS_KERNEL) $(KERNEL_OBJ) -o $@
	rm $(KERNEL_LD)

# User
user: $(USER_BIN)

$(USER_BIN): $(USER_ELF)
	objcopy -O binary $(USER_ELF) $@

$(USER_ELF): always $(USER_OBJ)
	echo "Linking User..."
	sed 's/$$(USER_LOAD_ADDR)/$(USER_LOAD_ADDR)/g' $(USER_LD_ORG) > $(USER_LD) && sync
	$(LD) $(LDFLAGS_USER) $(USER_OBJ) -o $@
	rm $(USER_LD)



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

# Libc Objects (Assembling)
$(OBJ_DIR)/%.libc.o: %.s
	mkdir -p $(dir $@)
	$(NASM) $< $(NASMFLAGS) -o $@

# Libc Object (Compiling)
$(OBJ_DIR)/%.libc.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(LIBC_FLAGS) -c $< -o $@

# ------------------------------------------------

always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/kernel

run:
	qemu-system-x86_64 -m 8G -hda $(FLOPPY_BIN) -drive id=disk,file=disk.img,if=none  -device ahci,id=ahci  -device ide-hd,drive=disk,bus=ahci.0 -d int,cpu_reset  -no-reboot -D log.txt
run_debugger: 
	qemu-system-x86_64 -m 8G -hda $(FLOPPY_BIN) -drive id=disk,file=disk.img,if=none  -device ahci,id=ahci  -device ide-hd,drive=disk,bus=ahci.0 -d int,cpu_reset  -no-reboot -s -S

run_debug_bochs:
	sed 's#$$(FLOPPY_BIN)#$(FLOPPY_BIN)#g' $(BOCHS_CONFIG_ORG) > $(BOCHS_CONFIG) && sync
	bochs -qf $(BOCHS_CONFIG)
	rm -f $(BOCHS_CONFIG)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *_tempwhy
