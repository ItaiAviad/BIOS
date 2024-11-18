#include <shell.h>
#include <kernel.h>

void shell_init(void) {
    // Clear tty
    tty_init();

    tty1.ci = 0;
    tty1.alive = true;

    memcpy(tty1.cwd[0], "dev", 3);

    while (tty1.alive) {
        print_prompt();
        int ci = get_cmd(&tty1);
        parse_cmd(tty1.cargc[ci], tty1.cache[ci]);
    }

    shell_fini();
}

void shell_fini(void) {
    // free all allocated commands cache memory
    for (int i = 0; i < MAX_CMDS_CACHE; i++) {
        if (!tty1.cache[i])
            continue;
        for (int j = 0; j < MAX_CMDS; j++) {
            if (!tty1.cache[i][j])
                break;
            free(tty1.cache[i][j]);
        }
        free(tty1.cache[i]);
    }
}

void print_prompt(void) {
    shcmd_pwd(0, NULL);
    printf(PROMPT);
}

int get_cmd(struct tty* tty) {
    char argv[ARG_MAX * ARG_MAX];
    memset(argv, 0x0, sizeof(argv));

    // get argv string
    fgets(argv, sizeof(argv), stdin);
    ftrimwhitespace(argv);

    // tokenize argv
    char *token = strtok(argv, ARGV_DELIM);
    int argc = tty->cargc[tty->ci] = 0x0;

    // Allocate argv (if needed)
    if (!(tty->cache[tty->ci]))
        tty->cache[tty->ci] = malloc(sizeof(char*) * ARG_MAX);

    while (token && argc < ARG_MAX) {
        char* ptr = tty->cache[tty->ci][argc];
        // Allocate argv[argc] (if needed)
        if (!ptr)
            ptr = malloc(ARG_MAX);
        tty->cache[tty->ci][argc] = ptr;

        // copy token to argv[argc]
        int len = min(strlen(token), ARG_MAX - 1);
        memcpy(ptr, token, len);
        ptr[len + 1] = '\0';

        argc++;

        token = strtok(NULL, ARGV_DELIM);
    }
    tty->cargc[tty->ci] = argc;

    int ci_before = tty->ci;
    tty->ci++;
    tty->ci %= MAX_CMDS_CACHE;

    return ci_before;
}

void parse_cmd(int argc, char *argv[]) { 
    if (!argc || !argv) // Empty command
        return;

    char* cmd = argv[0];

    // search cmd in commands str array
    for (int i = 0; i < MAX_CMDS; i++)
        if (shcmd_table[i] && !strcmp(cmd, shcmd_str[i]))
            return (void) shcmd_table[i](argc, argv);

    shcmd_notfound(argc, argv);
}
