# Project's make file

SECTOR_SIZE = 512

SHELL = /bin/bash
CXX = g++
ASM = nasm
CXXFLAGS = -m64 -masm=intel

FLOPPY_BIN = ./floppy.img
BOOT_DIR = ./boot
KERNEL_DIR = ./kernel
BUILD_DIR = ./build

BOOT_S = $(BOOT_DIR)/boot.s
BOOT_BIN = $(BUILD_DIR)/boot.bin

KERNEL_MEM_ADDR = 0x8C00
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
KERNEL_SOURCES_ASM = $(KERNEL_DIR)/entry.s 
KERNEL_SOURCES_CPP = $(wildcard $(KERNEL_DIR)/*.cpp) # Can be replaced with file1.cpp file2.cpp ...
KERNEL_SOURCES = $(KERNEL_SOURCES_ASM) $(KERNEL_SOURCES_CPP)
KERNEL_OBJECTS_CPP = $(patsubst $(KERNEL_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES_CPP))
KERNEL_OBJECTS_ASM = $(patsubst $(KERNEL_DIR)/%.s,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES_ASM))
KERNEL_OBJECTS = $(KERNEL_OBJECTS_ASM) $(KERNEL_OBJECTS_CPP)

.PHONY: all floppy_image bootloader always run clean

# Floppy Image
floppy_image: $(FLOPPY_BIN)
	dd if=$(BOOT_BIN) of=$(FLOPPY_BIN) conv=notrunc,fsync
	dd if=/dev/zero of=$(FLOPPY_BIN) bs=1 seek=$(shell $(SHELL) -c 'echo $$(stat -c %s $(FLOPPY_BIN))') count=$(shell $(SHELL) -c 'echo $$(($$(stat -c %s ./floppy.img) % 512))') conv=notrunc,fsync
	dd if=$(KERNEL_BIN) of=$(FLOPPY_BIN) bs=1 seek=$(shell $(SHELL) -c 'echo $$(stat -c %s $(BOOT_BIN))') conv=notrunc,fsync
	
$(FLOPPY_BIN): bootloader kernel
	dd if=/dev/zero of=$(FLOPPY_BIN) bs=$(SECTOR_SIZE) count=2880 conv=fsync	
	
#	dd if=./build/tmp.bin of=$(FLOPPY_BIN) bs=512 seek=1 conv=notrunc - add a bin file named tmp.bin to the 2nd sector in the disk. 

# Bootloader
bootloader: $(BOOT_BIN)
$(BOOT_BIN): $(KERNEL_BIN) 
	$(ASM) $(BOOT_S) -I $(BOOT_DIR) -DSECTOR_SIZE=$(SECTOR_SIZE) -DKERNEL_SIZE_IN_SECTORS=$(shell $(SHELL) -c 'echo $$(( ( $$(stat -c %s ./build/kernel.bin) + 512 -1 ) / 512))') -DKERNEL_LOAD_ADDR=$(KERNEL_MEM_ADDR) -f bin -o $(BOOT_BIN)
#	$(ASM) ./boot/tmp.s -I $(BOOT_DIR) -f bin -o ./build/tmp.bin - compile an asm file named tmp.s


kernel: $(KERNEL_BIN)

$(KERNEL_BIN): $(KERNEL_OBJECTS)
	ld -Ttext $(KERNEL_MEM_ADDR) -o $(KERNEL_ELF) $(KERNEL_OBJECTS)
	objcopy -R .note -R .comment -S -O binary $(KERNEL_ELF) $(KERNEL_BIN)

$(KERNEL_OBJECTS_CPP): $(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp always
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL_OBJECTS_ASM): $(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.s always
	$(ASM) $< -f elf64 -o $@

always:
	mkdir -p $(BUILD_DIR)

run:
	qemu-system-x86_64 -drive format=raw,file=$(FLOPPY_BIN)

run_debug_bochs:
	bochs -qf bochs_config

clean:
	rm -rf $(BUILD_DIR)/*
