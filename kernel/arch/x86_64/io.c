#include <arch/x86_64/io.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>

keyboard_t keyboard = {
    .buffer_head = 0,
    .buffer_tail = 0,
    
    .ks = {
        .caps = false,
        .lshift = false,
        .lctrl = false,
        .lalt = false,
        .altgr = false,
        .rctrl = false,
        .rshift = false,

        .up = false,
        .down = false,
        .left = false,
        .right = false
    }
};

bool is_special_char(char c) {
    return !(c != '\b' && c != '\r' && c != 0x1B && c != '\t' && c != '\n' && c != EOF && c != '\0');
}

void buffer_clear(void) {
    keyboard.buffer_head = 0;
    keyboard.buffer_tail = 0;
    memset(keyboard.buffer, 0, BUFFER_SIZE);
}

int clear_trailing_space(int start) {
    int cnt = 0;
    if (start <= 0)
        return cnt;

    for (int i = start - 1; i >= 0 && keyboard.buffer[i] == ' '; i--) {
        cnt++;
        buffer_put_c('\b');
    }
    return cnt;
}

void buffer_put(unsigned char s) { 
    // Check for special keys
    special_key_press(s);

    unsigned char uc = scs1_to_ascii(s, keyboard.ks.lshift | keyboard.ks.rshift, keyboard.ks.caps);

    // Ctrl+Backspace || Ctrl+Del || Ctrl+w
    if ((keyboard.ks.lctrl || keyboard.ks.rctrl) && !(keyboard.ks.lshift || keyboard.ks.rshift) && (uc == '\b' || uc == 0x0C || uc == 'w')) {
        if (uc == '\b' || uc == 'w') { // Ctrl+Backspace || Ctrl+w
            // Clear trailing spaces
            int start = (int)keyboard.buffer_head;
            int tspaces = clear_trailing_space(start);

            start -= tspaces;
            start--;
            if (start < 0)
                return;

            // '\b' current word
            for (int i = start; i >= 0 && keyboard.buffer[i] != ' '; i--) {
                start--;
                if (!is_special_char(keyboard.buffer[i]))
                    buffer_put_c('\b');
            }
            
            // Clear trailing spaces of previous word (leave one)
            clear_trailing_space(start);
        }
        return;
    }

    // Invalid input
    if (keyboard.ks.lctrl || keyboard.ks.rctrl || keyboard.ks.lalt || keyboard.ks.altgr)
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

    if (c != '\b' || keyboard.buffer_tail <= 1)
        return c;

    // Shift next chars left
    // Example:
    // q w e r t y \b \b -> q w e r t \b
    // char tmp[BUFFER_SIZE];
    cli();
    memset(keyboard.tmp, 0x0, BUFFER_SIZE);
    memcpy(keyboard.tmp, keyboard.buffer + keyboard.buffer_tail, BUFFER_SIZE - (keyboard.buffer_tail + 1));
    memcpy(keyboard.buffer + keyboard.buffer_tail - 2, keyboard.tmp, BUFFER_SIZE - (keyboard.buffer_tail + 1));
    keyboard.buffer_head -= 2;
    keyboard.buffer_tail -= 2;
    sti();

    return c;
}

int buffer_is_empty(void) {
    return keyboard.buffer_head == keyboard.buffer_tail;
}

void special_key_press(uint16_t scan_code) {
    switch (scan_code) {
        case 0x3A: // CapsLock Pressed
            keyboard.ks.caps = !keyboard.ks.caps;
            // outb(PS2_KEYBOARD_PORT_CMD_REG, 0xED);
            // io_wait();
            // outb(PS2_KEYBOARD_PORT_DATA, 0x0);
            // io_wait();
            break;
        case 0xBA: // CapsLock Released
            // keyboard.caps = false;
            break;
        
        case 0x2A: // Left Shift Pressed
            keyboard.ks.lshift = true;
            break;
        case 0xAA: // Left Shift Released
            keyboard.ks.lshift = false;
            break;

        case 0x1D: // Left Ctrl Pressed
            keyboard.ks.lctrl = true;
            break;
        case 0x9D: // Left Ctrl Released
            keyboard.ks.lctrl = false;
            keyboard.ks.rctrl = false;
            break;

        case 0x38: // Left Alt Pressed
            keyboard.ks.lalt = true;
            break;
        case 0xB8: // Left Alt Released
            keyboard.ks.lalt = false;
            keyboard.ks.altgr = false;
            break;

        case 0xE0: // Right Alt (Alt Gr) Pressed
            keyboard.ks.lalt = true;
            keyboard.ks.altgr = true;

            keyboard.ks.rctrl = true;
            break;

        case 0x3B: // Right Ctrl Pressed
            keyboard.ks.rctrl = true;
            break;
        case 0xBB: // Right Ctrl Released
            keyboard.ks.rctrl = false;
            break;
        
        case 0x36: // Right Shift Pressed
            keyboard.ks.rshift = true;
            break;
        case 0xB6: // Right Shift Released
            keyboard.ks.rshift = false;
            break;

        
        
        default:
            break;
    }
}

unsigned char wait_key() {
    irq_clear_mask(IRQ_KEYBOARD);
    
    while (buffer_is_empty()) {}
    unsigned char c = buffer_get();

    irq_set_mask(IRQ_KEYBOARD);

    return c;
}
