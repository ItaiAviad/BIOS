

BOOT_DIR = ./boot
BUILD_DIR = ./build

BOOT_S = $(BOOT_DIR)/boot.s
BOOT_BIN = $(BUILD_DIR)/boot.bin
FLOPPY_BIN = $(BUILD_DIR)/floppy.img



$(FLOPPY_BIN): $(BOOT_BIN)
	cp $(BOOT_BIN) $(FLOPPY_BIN)
	truncate -s 1440k $(FLOPPY_BIN)

$(BOOT_BIN): $(BOOT_S)
	nasm $(BOOT_S) -f bin -o $(BOOT_BIN)

run:
	qemu-system-x86_64 -drive format=raw,file=$(FLOPPY_BIN)

clean:
	rm $(BUILD_DIR)/*