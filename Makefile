

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

# Bootloader
bootloader: $(BOOT_BIN)
$(BOOT_BIN): always
	nasm $(BOOT_S) -f bin -o $(BOOT_BIN)

always:
	mkdir -p $(BUILD_DIR)

run:
	qemu-system-x86_64 -drive format=raw,file=$(FLOPPY_BIN)

clean:
	rm -rf $(BUILD_DIR)/*