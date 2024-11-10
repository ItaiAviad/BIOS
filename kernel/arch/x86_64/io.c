#include <arch/x86_64/io.h>
#include <stdio.h>

keyboard_t keyboard = {
    .buffer_head = 0,
    .buffer_tail = 0,
    
    .caps = false,
    .lshift = false,
    .lctrl = false,
    .lalt = false,
    .altgr = false,
    .rctrl = false,
    .rshift = false,
};

bool is_special_char(char c) {
    return !(c != '\b' && c != '\r' && c != 0x1B && c != '\t' && c != '\n' && c != EOF && c != '\0');
}

void buffer_put(unsigned char s) { 
    // Check for special keys
    special_key_press(s);

    unsigned char uc = scs1_to_ascii(s, keyboard.lshift | keyboard.rshift, keyboard.caps);

    // Ctrl+Backspace or Ctrl+Del    
    // if ((keyboard.lctrl || keyboard.rctrl) && (uc == '\b' || uc == 0x0C)) {
    //     if (uc == '\b') { // Ctrl+Backspace
    //         for (int i = keyboard.buffer_head; i >= 0 && keyboard.buffer[i] != ' '; i--) {
    //             if (!is_special_char(keyboard.buffer[i]))
    //                 buffer_put_c('\b');
    //         }
    //     }
    //     return;
    // }

    // Invalid input
    if (keyboard.lctrl || keyboard.rctrl || keyboard.lalt || keyboard.altgr)
        return;

    // Add char to buffer
    if (uc)
        buffer_put_c(uc);
}

void buffer_put_c(unsigned char c) {
    size_t next = (keyboard.buffer_head + 1) % BUFFER_SIZE;
    if (next != keyboard.buffer_tail) { // Ensure buffer is not full
        keyboard.buffer[keyboard.buffer_head] = c;
        keyboard.buffer_head = next;
    }
}

unsigned char buffer_get(void) {
    if (keyboard.buffer_head == keyboard.buffer_tail) {
        return -1; // Buffer empty
    }
    unsigned char c = keyboard.buffer[keyboard.buffer_tail];
    keyboard.buffer_tail = (keyboard.buffer_tail + 1) % BUFFER_SIZE;
    return c;
}

int buffer_is_empty(void) {
    return keyboard.buffer_head == keyboard.buffer_tail;
}

void special_key_press(uint16_t scan_code) {
    switch (scan_code) {
        case 0x3A: // CapsLock Pressed
            keyboard.caps = !keyboard.caps;
            // outb(PS2_KEYBOARD_PORT_CMD_REG, 0xED);
            // io_wait();
            // outb(PS2_KEYBOARD_PORT_DATA, 0x0);
            // io_wait();
            break;
        case 0xBA: // CapsLock Released
            // keyboard.caps = false;
            break;
        
        case 0x2A: // Left Shift Pressed
            keyboard.lshift = true;
            break;
        case 0xAA: // Left Shift Released
            keyboard.lshift = false;
            break;

        case 0x1D: // Left Ctrl Pressed
            keyboard.lctrl = true;
            break;
        case 0x9D: // Left Ctrl Released
            keyboard.lctrl = false;
            keyboard.rctrl = false;
            break;

        case 0x38: // Left Alt Pressed
            keyboard.lalt = true;
            break;
        case 0xB8: // Left Alt Released
            keyboard.lalt = false;
            keyboard.altgr = false;
            break;

        case 0xE0: // Right Alt (Alt Gr) Pressed
            keyboard.lalt = true;
            keyboard.altgr = true;

            keyboard.rctrl = true;
            break;

        case 0x3B: // Right Ctrl Pressed
            keyboard.rctrl = true;
            break;
        case 0xBB: // Right Ctrl Released
            keyboard.rctrl = false;
            break;
        
        case 0x36: // Right Shift Pressed
            keyboard.rshift = true;
            break;
        case 0xB6: // Right Shift Released
            keyboard.rshift = false;
            break;
        default:
            break;
    }
}

char wait_key() {
    irq_clear_mask(IRQ_KEYBOARD);
    
    printf("IN WAIT KEY\n");
    while (buffer_is_empty()) {}

    irq_set_mask(IRQ_KEYBOARD);

    return buffer_get();
}
