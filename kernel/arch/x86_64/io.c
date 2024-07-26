#include <arch/x86_64/io.h>
#include <stdio.h>

bool scs1_is_press(uint16_t scan_code) {
    return scan_code < PS2_KEYBOARD_SCS1_RLS_IDX;
}

void special_key_press(uint16_t scan_code) {
    switch (scan_code) {
        case 0x3A: // CapsLock
            printf("CapsLock");
            // outb(PS2_KEYBOARD_PORT_CMD_REG, 0xED);
            // io_wait();
            // outb(PS2_KEYBOARD_PORT_DATA, 0x0);
            // io_wait();
            return;
        case 0x2A: // Left Shift
            printf("LShift");
            return;
        default:
            return;
    }
}

char wait_key() {
    uint16_t uc = 0;
    // irq_clear_mask(IRQ_KEYBOARD);
    
    // Wait for valid key press
    do {
        uc = inb(PS2_KEYBOARD_PORT_DATA);
        if (!scs1_to_ascii(uc) && scs1_is_press(uc)) {
            special_key_press(uc);
            while (scs1_is_press(inb(PS2_KEYBOARD_PORT_DATA))) {}
        }
    } while (!(scs1_is_press(uc) && scs1_to_ascii(uc)));

    // Wait for key release
    while (scs1_is_press(inb(PS2_KEYBOARD_PORT_DATA))) {}

    // Delay (TODO!)

    // irq_set_mask(IRQ_KEYBOARD);
    pic_send_eoi(IRQ_KEYBOARD);
    pic_send_eoi(0x1);

    return scs1_to_ascii(uc);
}

char scs1_to_ascii(uint16_t scan_code) {
    switch (scan_code) {
        case 0x01: return 0x1B; // Escape
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';
        case 0x0C: return '-';
        case 0x0D: return '=';
        case 0x0E: return '\b'; // Backspace
        case 0x0F: return '\t'; // Tab
        case 0x10: return 'q';
        case 0x11: return 'w';
        case 0x12: return 'e';
        case 0x13: return 'r';
        case 0x14: return 't';
        case 0x15: return 'y';
        case 0x16: return 'u';
        case 0x17: return 'i';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x1A: return '[';
        case 0x1B: return ']';
        case 0x1C: return '\n'; // Enter
        case 0x1D: return 0;    // Left Control
        case 0x1E: return 'a';
        case 0x1F: return 's';
        case 0x20: return 'd';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x27: return ';';
        case 0x28: return '\'';
        case 0x29: return '`';
        case 0x2A: return 0;    // Left Shift
        case 0x2B: return '\\';
        case 0x2C: return 'z';
        case 0x2D: return 'x';
        case 0x2E: return 'c';
        case 0x2F: return 'v';
        case 0x30: return 'b';
        case 0x31: return 'n';
        case 0x32: return 'm';
        case 0x33: return ',';
        case 0x34: return '.';
        case 0x35: return '/';
        case 0x36: return 0;    // Right Shift
        case 0x37: return '*';  // Keypad *
        case 0x38: return 0;    // Left Alt
        case 0x39: return ' ';
        case 0x3A: return 0;    // CapsLock
        case 0x3B: return 0;    // F1
        case 0x3C: return 0;    // F2
        case 0x3D: return 0;    // F3
        case 0x3E: return 0;    // F4
        case 0x3F: return 0;    // F5
        case 0x40: return 0;    // F6
        case 0x41: return 0;    // F7
        case 0x42: return 0;    // F8
        case 0x43: return 0;    // F9
        case 0x44: return 0;    // F10
        case 0x45: return 0;    // NumberLock
        case 0x46: return 0;    // ScrollLock
        case 0x47: return '7';  // Keypad 7
        case 0x48: return '8';  // Keypad 8
        case 0x49: return '9';  // Keypad 9
        case 0x4A: return '-';  // Keypad -
        case 0x4B: return '4';  // Keypad 4
        case 0x4C: return '5';  // Keypad 5
        case 0x4D: return '6';  // Keypad 6
        case 0x4E: return '+';  // Keypad +
        case 0x4F: return '1';  // Keypad 1
        case 0x50: return '2';  // Keypad 2
        case 0x51: return '3';  // Keypad 3
        case 0x52: return '0';  // Keypad 0
        case 0x53: return '.';  // Keypad .
        case 0x57: return 0;    // F11
        case 0x58: return 0;    // F12
        default: return 0;      // Undefined
    }
}
