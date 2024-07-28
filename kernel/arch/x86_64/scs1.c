#include <arch/x86_64/scs1.h>

bool scs1_is_press(uint16_t scan_code) {
    return scan_code < PS2_KEYBOARD_SCS1_RLS_IDX;
}

char scs1_to_ascii(uint16_t scan_code, bool shift, bool caps) {
    switch (scan_code) {
        case 0x01: return 0x1B; // Escape
        case 0x02: return shift ? '!' : '1';
        case 0x03: return shift ? '@' : '2';
        case 0x04: return shift ? '#' : '3';
        case 0x05: return shift ? '$' : '4';
        case 0x06: return shift ? '%' : '5';
        case 0x07: return shift ? '^' : '6';
        case 0x08: return shift ? '&' : '7';
        case 0x09: return shift ? '*' : '8';
        case 0x0A: return shift ? '(' : '9';
        case 0x0B: return shift ? ')' : '0';
        case 0x0C: return shift ? '_' : '-';
        case 0x0D: return shift ? '+' : '=';
        case 0x0E: return '\b'; // Backspace
        case 0x0F: return '\t'; // Tab
        case 0x10: return (shift || caps) ? 'Q' : 'q';
        case 0x11: return (shift || caps) ? 'W' : 'w';
        case 0x12: return (shift || caps) ? 'E' : 'e';
        case 0x13: return (shift || caps) ? 'R' : 'r';
        case 0x14: return (shift || caps) ? 'T' : 't';
        case 0x15: return (shift || caps) ? 'Y' : 'y';
        case 0x16: return (shift || caps) ? 'U' : 'u';
        case 0x17: return (shift || caps) ? 'I' : 'i';
        case 0x18: return (shift || caps) ? 'O' : 'o';
        case 0x19: return (shift || caps) ? 'P' : 'p';
        case 0x1A: return shift ? '{' : '[';
        case 0x1B: return shift ? '}' : ']';
        case 0x1C: return '\n'; // Enter
        case 0x1D: return 0;    // Left Control
        case 0x1E: return (shift || caps) ? 'A' : 'a';
        case 0x1F: return (shift || caps) ? 'S' : 's';
        case 0x20: return (shift || caps) ? 'D' : 'd';
        case 0x21: return (shift || caps) ? 'F' : 'f';
        case 0x22: return (shift || caps) ? 'G' : 'g';
        case 0x23: return (shift || caps) ? 'H' : 'h';
        case 0x24: return (shift || caps) ? 'J' : 'j';
        case 0x25: return (shift || caps) ? 'K' : 'k';
        case 0x26: return (shift || caps) ? 'L' : 'l';
        case 0x27: return shift ? ':' : ';';
        case 0x28: return shift ? '"' : '\'';
        case 0x29: return shift ? '~' : '`';
        case 0x2A: return 0;    // Left Shift
        case 0x2B: return shift ? '|' : '\\';
        case 0x2C: return (shift || caps) ? 'Z' : 'z';
        case 0x2D: return (shift || caps) ? 'X' : 'x';
        case 0x2E: return (shift || caps) ? 'C' : 'c';
        case 0x2F: return (shift || caps) ? 'V' : 'v';
        case 0x30: return (shift || caps) ? 'B' : 'b';
        case 0x31: return (shift || caps) ? 'N' : 'n';
        case 0x32: return (shift || caps) ? 'M' : 'm';
        case 0x33: return shift ? '<' : ',';
        case 0x34: return shift ? '>' : '.';
        case 0x35: return shift ? '?' : '/';
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
