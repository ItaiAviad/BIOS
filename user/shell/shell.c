#include <shell.h>
#include <kernel.h>
    
struct tty tty1 = {
    .ci = 0,
    .alive = true
};

void shell_init(void) {
    // Clear tty
    tty_init();

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
    pwd(0, NULL);
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

void shcmd_notfound(int argc, char *argv[]) {
    if (argc && argv)
        printf("%s: %s: command not found\n", SHELL_NAME, argv[0]);
}

void echo(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
    if (!argc || !argv)
        return;
    if (argc == 1 || !argv[1] || !strlen(argv[1]))
        return (void) printf("\n");

    char *str = malloc(BUFFER_SIZE);
    int offset = 1;
    printf("%s\n", strjoin(str, argc - offset, argv + offset, ARGV_DELIM));
    free(str);
}

void shcmd_printf(int argc, char *argv[]) {
    // format
    char* format = argv[1];
    if (!format)
        return;
    
    // string
    if (!strcmp(format, "%s")) {
        if (!argv[2] || !strlen(argv[2]))
            return;
        
        char *str = malloc(BUFFER_SIZE);
        int offset = 2;
        printf(format, strjoin(str, argc - offset, argv + offset, ""));
        free(str);

        printf("\n");
        return;
    }

    if (strcmp(format, "%d") && strcmp(format, "%x")) {
        if (*format == '%')
            return (void)printf("%s: invalid conversion specification\n", format);
        return (void)printf("%s\n", format);
    }

    // number
    char* s = argv[2];
    if (!s)
        return (void)printf("%d\n", 0x0);
    s = trimwhitespace(s);

    if (!isnumber(s))
        return (void)printf("%s: expected a numeric value\n", s);
    
    printf(format, atoi(s));
    printf("\n");
}

void pwd(int argc, char *argv[]) {
    char cwd[MAX_DIRS * MAX_DIR_NAME];
    memset(cwd, 0x0, sizeof(cwd));

    // concatenate cwd directories
    for (int i = 0; i < MAX_DIRS && tty1.cwd[i][0] != '\0'; i++) {
        memcpy(cwd + strlen(cwd), tty1.cwd[i], MAX_DIR_NAME);
        memcpy(cwd + strlen(cwd), "/", 1);
    }
    
    printf("%s", cwd);
    if (argv && argc) // argv exists (pwd command called)
        printf("\n");
}

void shcmd_shutdown(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
    shell_fini();
    shutdown();
}

void shcmd_exit(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
    tty1.alive = false;
}
