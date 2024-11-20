#include <shell.h>

void cuph() {
    if (tty0.active) { // In Shell mode => Previous command
        traverse_cache(-1);
    }
    // IN VIM EDITOR => Move cursor up
}

void clefth() {

}

void crighth() {

}

void cdownh() {
    if (tty0.active) { // In Shell mode => Next command
        traverse_cache(1);
    }
}
