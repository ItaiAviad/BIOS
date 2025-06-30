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

        .ext = false,

        .up = false,
        .down = false,
        .left = false,
        .right = false
    },
};

bool is_special_char(char c) {
    return !(c != '\b' && c != '\r' && c != 0x1B && c != '\t' && c != '\n' && c != EOF && c != '\0');
}

void buffer_clear(void) {
    keyboard.buffer_head = 0;
    keyboard.buffer_tail = 0;
    memset(keyboard.buffer, 0, BUFFER_SIZE);
}

int buffer_len(void) {
    return keyboard.buffer_tail;
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

    // Ignore extended byte press
    if (keyboard.ks.ext || keyboard.ks.up || keyboard.ks.left || keyboard.ks.down || keyboard.ks.right)
        return;
    
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

int buffer_get(void) {
    if (keyboard.buffer_head == keyboard.buffer_tail) {
        return EOF; // Buffer empty
    }
    int c = keyboard.buffer[keyboard.buffer_tail];
    keyboard.buffer_tail = (keyboard.buffer_tail + 1) % BUFFER_SIZE;

    if (c != '\b')
        return c;

    // Shift next chars left
    // Example:
    // q w e r t y \b \b -> q w e r t \b
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
    return keyboard.buffer_head == keyboard.buffer_tail && 
        !(keyboard.ks.up || keyboard.ks.down || keyboard.ks.left || keyboard.ks.right);
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

        case 0xE0: // Extended byte
            keyboard.ks.ext = true;
            break;

        // Right keys
        case 0x3B: // Right Ctrl Pressed
            if (keyboard.ks.ext) keyboard.ks.rctrl = true;
            keyboard.ks.ext = false;
            break;
        case 0xBB: // Right Ctrl Released
            if (keyboard.ks.ext) keyboard.ks.rctrl = false;
            keyboard.ks.ext = false;
            break;
        
        case 0x36: // Right Shift Pressed
            keyboard.ks.rshift = true;
            break;
        case 0xB6: // Right Shift Released
            keyboard.ks.rshift = false;
            break;

        // Cursor keys
        case 0x48: // Cursor Up Pressed
            if (keyboard.ks.ext) keyboard.ks.up = true;
            keyboard.ks.ext = false;
            break;
        case 0xC8: // Cursor Up Released
            if (keyboard.ks.ext) keyboard.ks.up = false;
            keyboard.ks.ext = false;
            break;
 
        case 0x4B: // Cursor Left Pressed
            if (keyboard.ks.ext) keyboard.ks.left = true;
            keyboard.ks.ext = false;
            break;
        case 0xCB: // Cursor Left Released
            if (keyboard.ks.ext) keyboard.ks.left = false;
            keyboard.ks.ext = false;
            break;

        case 0x4D: // Cursor Right Pressed
            if (keyboard.ks.ext) keyboard.ks.right = true;
            keyboard.ks.ext = false;
            break;
        case 0xCD: // Cursor Right Released
            if (keyboard.ks.ext) keyboard.ks.right = false;
            keyboard.ks.ext = false;
            break;

        case 0x50: // Cursor Down Pressed
            if (keyboard.ks.ext) keyboard.ks.down = true;
            keyboard.ks.ext = false;
            break;
        case 0xD0: // Cursor Down Released
            if (keyboard.ks.ext) keyboard.ks.down = false;
            keyboard.ks.ext = false;
            break;        
        
        default:
            break;
    }
}

int wait_key() {
    irq_clear_mask(IRQ_KEYBOARD);
    
    while (buffer_is_empty()) {}
    int c = buffer_get();

    irq_set_mask(IRQ_KEYBOARD);

    if (c != EOF)
        return c;

    if (keyboard.ks.up) {
        keyboard.ks.up = false;
        return CURSOR_UP;
    }
    if (keyboard.ks.down) {
        keyboard.ks.down = false;
        return CURSOR_DOWN;
    }
    if (keyboard.ks.left) {
        keyboard.ks.left = false;
        return CURSOR_LEFT;
    }
    if (keyboard.ks.right) {
        keyboard.ks.right = false;
        return CURSOR_RIGHT;
    }

    return EOF;
}
