#include <shell.h>
#include <kernel.h>

void shell_init(void) {
    // Clear tty
    tty_init();

    tty0.ci = 0;
    tty0.tci = 0;
    tty0.alive = true;
    tty0.active = true;

    memcpy(tty0.cwd[0], "dev", 3);

    while (tty0.alive) {
        print_prompt();
        int ci = get_cmd(&tty0); // get command and build argv + argc
        parse_cmd(tty0.cargc[ci], tty0.cache[ci]);
    }

    shell_fini();
}

void shell_fini(void) {
    // free all allocated commands cache memory
    for (int i = 0; i < MAX_CMDS_CACHE; i++) {
        if (!tty0.cache[i])
            continue;
        for (int j = 0; j < ARG_MAX; j++) {
            if (!tty0.cache[i][j])
                break;
            free(tty0.cache[i][j]);
        }
        free(tty0.cache[i]);
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

    int ci_before = tty->ci;
    if (!token)
        return ci_before;

    while (token && argc < ARG_MAX) {
        char* ptr = tty->cache[tty->ci][argc];
        // Allocate argv[argc] (if needed)
        if (!ptr)
            ptr = malloc(ARG_MAX);
        tty->cache[tty->ci][argc] = ptr;

        // copy token to argv[argc]
        int len = min(strlen(token), ARG_MAX - 1);
        memset(ptr, 0x0, strlen(ptr));
        memcpy(ptr, token, len);
        ptr[len + 1] = '\0';

        argc++;

        token = strtok(NULL, ARGV_DELIM);
    }
    tty->cargc[tty->ci] = argc;
    

    tty->ci++;
    tty->ci %= MAX_CMDS_CACHE;
    tty->tci = tty->ci;

    return ci_before;
}

void parse_cmd(int argc, char *argv[]) { 
    if (!argc || !argv) // Empty command
        return;

    char* cmd = argv[0];

    // search cmd in commands str array
    tty0.active = false;
    shcmd dst = NULL;
    for (int i = 0; i < MAX_CMDS; i++) {
        if (shcmd_table[i] && !strcmp(cmd, shcmd_str[i])) {
            dst = shcmd_table[i];
            break;
        }
    }
    if (!dst) dst = shcmd_notfound;

    dst(argc, argv);

    tty0.active = true;
}

void traverse_cache(int offset) {
    int tmptci = tty0.tci; // save tci

    tty0.tci += offset;
    // If negative
    if (tty0.tci < 0) tty0.tci = MAX_CMDS_CACHE + tty0.tci;
    tty0.tci %= MAX_CMDS_CACHE;

    if (tty0.tci == tty0.ci || // back to current command - not allowed
        (!tty0.cache[tty0.tci] || !tty0.cache[tty0.tci][0])) // invalid cache entry
        return (void) (tty0.tci = tmptci);
    
    // show cache
    // clear current buffer
    stdin_clear();

    // insert current cache
    char *dst = malloc(BUFFER_SIZE);
    stdin_insert(strjoin(dst, tty0.cargc[tty0.tci], tty0.cache[tty0.tci], ARGV_DELIM));
    free(dst);
}
