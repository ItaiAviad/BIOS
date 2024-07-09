

BOOT_DIR = ./boot
BUILD_DIR = ./build

BOOT_S = $(BOOT_DIR)/boot.s
BOOT_BIN = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
FLOPPY_BIN = $(BUILD_DIR)/floppy.img

.PHONY: all floppy_image bootloader always run clean

# Floppy Image
floppy_image: $(FLOPPY_BIN)
$(FLOPPY_BIN): bootloader kernel
	dd if=/dev/zero of=$(FLOPPY_BIN) bs=512 count=2880
	dd if=$(BOOT_BIN) of=$(FLOPPY_BIN) conv=notrunc
#	dd if=./build/tmp.bin of=$(FLOPPY_BIN) bs=512 seek=1 conv=notrunc - add a bin file named tmp.bin to the 2nd sector in the disk. 

# Bootloader
bootloader: $(BOOT_BIN)
$(BOOT_BIN): always
	nasm $(BOOT_S) -I $(BOOT_DIR) -f bin -o $(BOOT_BIN)
#	nasm ./boot/tmp.s -I $(BOOT_DIR) -f bin -o ./build/tmp.bin - compile an asm file named tmp.s

always:
	mkdir -p $(BUILD_DIR)

run:
	qemu-system-x86_64 -drive format=raw,file=$(FLOPPY_BIN)

run_debug_bochs:
	bochs -qf bochs_config

clean:
	rm -rf $(BUILD_DIR)/*
