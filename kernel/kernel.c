// Kernel Main File

// Source: https://wiki.osdev.org/Creating_a_64-bit_kernel

#define VGA_ADDR (volatile char *)0xB8000
#define VGA_COLOR_WHITE 7
#define VGA_EXTENT 80 * 25

#define STYLE_WB 0x0F

typedef struct __attribute__((packed)) {
    char character;
    char style;
} vga_char;

volatile vga_char *SCREEN = (vga_char*) VGA_ADDR;

void clearwin(){
    vga_char clear_char = {
        .character=' ',
        .style=STYLE_WB
    };

    for(unsigned int i = 0; i < VGA_EXTENT; i++){
        SCREEN[i] = clear_char;
    }
}

void putstr(const char *str){
    for(unsigned int i = 0; str[i] != '\0'; i++){
        if (i >= VGA_EXTENT)
            break;

        vga_char temp = {
            .character=str[i],
            .style=STYLE_WB
        };

        SCREEN[i] = temp;
    }
}

int kmain(){
    // clearwin();
    const char *welcome_msg = "Kernel!";
    putstr(welcome_msg);

    return 0;
}