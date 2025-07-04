# Main Makefile

# b => Build
# r => Run
# n => Network

# Make Makefile Silent (without repeating @ before commands)
ifndef VERBOSE
.SILENT:
endif


SECTOR_SIZE := 512

KERNEL_LOAD_ADDR := 0x10000
KERNEL_STACK_START_ADDR := 0xF000
KERNEL_VBASE := $(shell echo $$((0x800000 + $(KERNEL_LOAD_ADDR)))) # 4MB - Kernel binary VA
PROC_BIN_ADDR := 0x4000000

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
USER_LOAD_ADDR = 0x4000000# (1024 * 1024 * 0x40)
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

OBJ_LIBC := $(patsubst %, $(OBJ_DIR)/%, $(LIBC_C:.c=.libc.o)) # $(patsubst %, $(OBJ_DIR)/%, $(LIBC_S:.s=.libc.o))
OBJ_LIBK := $(patsubst %, $(OBJ_DIR)/%, $(LIBC_C:.c=.libk.o)) $(patsubst %, $(OBJ_DIR)/%, $(LIBC_S:.s=.libk.o))
# OBJ_KERNEL := $(patsubst %, $(OBJ_DIR)/%, $(KERNEL_S:.s=.o)) $(patsubst %, $(OBJ_DIR)/%, $(KERNEL_C:.c=.o))
KERNEL_OBJ := $(patsubst %, $(OBJ_DIR)/%, $(KERNEL_SRC_S:.s=.o)) \
		$(patsubst %, $(OBJ_DIR)/%, $(KERNEL_SRC_C:.c=.o)) \
		$(OBJ_LIBK)
USER_OBJ := $(patsubst %, $(OBJ_DIR)/%, $(USER_SRC_S:.s=.o)) \
		$(patsubst %, $(OBJ_DIR)/%, $(USER_SRC_C:.c=.o)) \
		$(OBJ_LIBC)

# Disks
DISK = disk.img
DISK2 = disk2.img
MOUNT_POINT = ./fscreate

### Constants
SHELL := /bin/bash
CC := x86_64-elf-gcc
DD := dd
NASM := nasm
LD := x86_64-elf-ld

INCLUDES := -I$(LIBC_INCLUDE) -I$(KERNEL_INCLUDE) -I$(USER_INCLUDE)
# MISC_FLAGS = -DCURRENT_YEAR=$(shell date --utc | awk '{print $$7}')
# MISC_FLAGS += -DTIMEZONE=\"$(shell date --utc | awk '{print $$6}')\"
MISC_FLAGS = -DKERNEL_LOAD_ADDR=$(KERNEL_LOAD_ADDR) -DKERNEL_STACK_START_ADDR=$(KERNEL_STACK_START_ADDR) \
			-DKERNEL_VBASE=$(KERNEL_VBASE) \
			-DPROC_BIN_ADDR=$(PROC_BIN_ADDR) \
			-DUSER_LOAD_ADDR=$(USER_LOAD_ADDR) -DCURRENT_YEAR=$(shell $(SHELL) -c "date -u +%Y")
ifdef DEBUG
MISC_FLAGS += -DDEBUG=\"DEBUG\"
endif
CFLAGS := -ffreestanding -m64 -march=x86-64 -masm=intel -nostdlib -Wall -g -Wextra -O0 -mno-red-zone $(INCLUDES) $(MISC_FLAGS)
NASMFLAGS := -f elf64 -g -DUSER_LOAD_ADDR=$(USER_LOAD_ADDR)
LDFLAGS_KERNEL := -T $(KERNEL_LD) $(INCLUDES)
LDFLAGS_USER := -T $(USER_LD) $(INCLUDES)
LIBC_FLAGS := $(CFLAGS) -D__is_libc
LIBK_FLAGS := $(CFLAGS) -D__is_libk

# -----------------------------------------------

.PHONY: all build boot always clean

all: build

# Build Disk (Floppy Image)
build: $(FLOPPY_BIN)
$(FLOPPY_BIN): kernel boot user
	# Write zeroes to disk
	$(DD) if=/dev/zero of=$@ conv=notrunc,fsync count=65536
	# Writer bootloader to disk
	$(DD) if=$(BOOT_BIN) of=$@ conv=notrunc,fsync
	# Write kernel to disk
	$(DD) if=$(KERNEL_BIN) of=$@ bs=1 seek=$$(stat -c %s $(BOOT_BIN)) conv=notrunc,fsync
	sync
	FILE_SIZE=$$(stat -c %s $@); \
	PADDING=$$(( $(SECTOR_SIZE) - FILE_SIZE % $(SECTOR_SIZE) )); \
	$(DD) if=/dev/zero of=$@ bs=1 seek=$$FILE_SIZE count=$$PADDING conv=notrunc,fsync; \
	# $(DD) if=/dev/zero of=$@ bs=$(SECTOR_SIZE) seek=$$(($$FILE_SIZE + $$PADDING)) count=2048 conv=notrunc,fsync

	# Write user code to disk

	$(DD) if=/dev/zero of=$(DISK) count=2048

		# # Create filesystem on disk
		mkdir -p $(MOUNT_POINT)
		
		mkfs.ext2 $(DISK)

		sudo mount $(DISK) $(MOUNT_POINT)

		sudo chmod 777 $(MOUNT_POINT)

		sudo cp $(USER_ELF) $(MOUNT_POINT)/user_prog

		sudo mkdir $(MOUNT_POINT)/test

		sudo sh -c 'echo "Hi, If you are reading this it means that everything works!!! :)" > $(MOUNT_POINT)/test/test_file.txt'

		sudo umount $(MOUNT_POINT)

# Bootloader
boot: $(BOOT_BIN)
$(BOOT_BIN): always kernel
	$(NASM) $(BOOT_S) -I $(BOOT_DIR)  \
		-DSECTOR_SIZE=$(SECTOR_SIZE) \
		-DKERNEL_SIZE_IN_SECTORS=$$(($(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s $(KERNEL_BIN)) + $(SECTOR_SIZE) -1 ) / $(SECTOR_SIZE)))'))) \
		-DKERNEL_SIZE=$$( $(SHELL) -c 'stat -c %s $(KERNEL_BIN)' ) \
		-DTOTAL_SIZE_IN_SECTORS=$(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s $(KERNEL_BIN)) + $(SECTOR_SIZE) -1 ) / $(SECTOR_SIZE)))')\
		-DKERNEL_LOAD_ADDR=$(KERNEL_LOAD_ADDR) \
		-DKERNEL_VBASE=$(KERNEL_VBASE) \
		-DKERNEL_STACK_START_ADDR=$(KERNEL_STACK_START_ADDR) \
		-f bin -o $@

# Kernel
kernel: $(KERNEL_BIN)
$(KERNEL_BIN): $(KERNEL_ELF)
	objcopy -O binary $(KERNEL_ELF) $@
	sync

$(KERNEL_ELF): always $(KERNEL_OBJ)
	echo "Linking Kernel..."
	sed 's/$$(KERNEL_VBASE)/$(KERNEL_VBASE)/g' $(KERNEL_LD_ORG) > $(KERNEL_LD) && sync
	$(LD) $(LDFLAGS_KERNEL) $(KERNEL_OBJ) -o $@
	rm $(KERNEL_LD)

# User
user: $(USER_BIN)
$(USER_BIN): $(USER_ELF)
	objcopy -O binary $(USER_ELF) $@

$(USER_ELF): always $(USER_OBJ)
	echo "Linking User..."
	sed 's/$$(PROC_BIN_ADDR)/$(PROC_BIN_ADDR)/g' $(USER_LD_ORG) > $(USER_LD) && sync
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

# Build Clean
bclean:
	rm -rf $(BUILD_DIR)/*
	rm -f *_tempwhy

# -----------------------------------------------
# Network
# See: https://wiki.qemu.org/Documentation/Networking
#      https://gist.github.com/extremecoders-re/e8fd8a67a515fee0c873dcafc81d811c
#      https://wiki.osdev.org/RTL8139

NET := biosnet0
VND := rtl8139# Virtual Network Device
BRIDGE := br0
TAP := tap0
WLAN0 := wlan0
ETHIF := enp0s20f0u2u1c2 # (ethernet interface)
# interface:=$(shell ip addr | awk '/state UP/ {print $$2}' | head -n 1 | awk '{print substr($$0, 1, length($$0)-1)}')
# ETHIF := $(interface) # (ethernet interface)

# Network Setup
nsetup:
	sudo brctl addbr $(BRIDGE)
	-sudo brctl addif $(BRIDGE) $(ETHIF)
	# -sudo brctl addif $(BRIDGE) $(WLAN0)

	# tap
	sudo tunctl -t $(TAP) -u root
	sudo ip link set $(TAP) promisc on
	sudo brctl addif $(BRIDGE) $(TAP)

	# -sudo ifconfig $(ETHIF) up
	sudo ifconfig $(TAP) up
	sudo ifconfig $(BRIDGE) up

	# dhcp
	sudo dhclient $(BRIDGE)

	brctl show

# Network Clean
nclean:
	-sudo ip link set $(ETHIF) promisc off
	-sudo ip link set $(ETHIF) up

	-sudo brctl delif $(BRIDGE) $(TAP)
	-sudo tunctl -d $(TAP)

	-sudo brctl delif $(BRIDGE) $(ETHIF)
	# -sudo brctl delif $(BRIDGE) $(WLAN0)

	-sudo ifconfig $(BRIDGE) down

	-sudo brctl delbr $(BRIDGE)

	# dhcp
	-sudo dhclient -r $(BRIDGE)

# ------------------------------------------------

always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/kernel

# Common QEMU command
QEMU_CMD = qemu-system-x86_64 -m 8G -hda $(FLOPPY_BIN) \
	-drive id=disk,file=$(DISK),if=none \
	-device ahci,id=ahci  -device ide-hd,drive=disk,bus=ahci.0 \
	-d int,cpu_reset,in_asm \
	-no-reboot -D log.txt\
	-monitor stdio \
	-machine kernel_irqchip=off


ron:
	echo "Running online..."
	$(QEMU_CMD) \
	-netdev tap,id=$(NET),ifname=$(TAP),script=no,downscript=no \
	-device $(VND),netdev=$(NET),id=$(VND),mac=de:ad:be:ef:12:34 \
	-object filter-dump,id=f1,netdev=$(NET),file=dump.dat

	# -d int,cpu_reset,in_asm,guest_errors \
	# -no-reboot -D log.txt

roff:
	echo "Running offline..."
	$(QEMU_CMD)

run_debugger: 
	sudo $(QEMU_CMD) -s -S

run_debug_bochs:
	sed 's#$$(FLOPPY_BIN)#$(FLOPPY_BIN)#g' $(BOCHS_CONFIG_ORG) > $(BOCHS_CONFIG) && sync
	bochs -qf $(BOCHS_CONFIG)
	rm -f $(BOCHS_CONFIG)

clean: bclean nclean
