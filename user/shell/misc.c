// Shell miscellaneous functions
#include <shell.h>

void shcmd_notfound(int argc, char *argv[]) {
    if (argc && argv)
        printf("%s: %s: command not found\n", SHELL_NAME, argv[0]);
}

void shcmd_shutdown(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
    shell_fini();
    shutdown();
}

void shcmd_exit(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
    tty1.alive = false;
}

void shcmd_clear(int argc, char *argv[]) {
    if (!argc || !argv)
        return;

    tty_init();
}
