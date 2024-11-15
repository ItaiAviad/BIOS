#include <shell.h>
#include <kernel.h>

void shell_init(void) {
    // Clear tty
    tty_init();

    struct tty tty1 = {
        .csize = 0
    };

    memcpy(tty1.cwd[0], "dev", 3);

    while (1) {
        print_prompt(&tty1);
        get_cmd(&tty1);
        parse_cmd(&tty1);
    }
}

void print_prompt(struct tty* tty) {
    pwd(tty, false);
    printf(PROMPT);
}

void get_cmd(struct tty* tty) {
    fgets(tty->cmds[(tty->csize++) % MAX_COMMANDS], COMMAND_SIZE, stdin);
}

void parse_cmd(struct tty* tty) {
    char cmd[COMMAND_SIZE];
    memcpy(cmd, tty->cmds[(tty->csize - 1) % MAX_COMMANDS], COMMAND_SIZE);

    // command
    char* c = strtok(cmd, CMD_DELIM);
    if (!c)
        return;

    if (!strcmp(c, EHCO))
        printf("%s\n", cmd + strlen(c) + 1);
    else if (!strcmp(c, PRINTF))
        shell_printf(c, cmd);
    else if (!strcmp(c, PWD))
        pwd(tty, true);
    else if (!strcmp(c, SHUTDOWN))
        shutdown();
}

void shell_printf(char* c, char* cmd) {
    // format
    char* format = strtok(NULL, CMD_DELIM);
    if (!format)
        return;
    
    // string
    if (!strcmp(format, "%s")) {
        printf(format, rmwhitespace(cmd + strlen(c) + strlen(format) + 2));
        printf("\n");
        return;
    }

    if (strcmp(format, "%d") && strcmp(format, "%x")) {
        if (*format == '%')
            return (void)printf("%s: invalid conversion specification\n", format);
        return (void)printf("%s\n", format);
    }

    // number
    char* s = strtok(NULL, CMD_DELIM);
    if (!s)
        return (void)printf("%d\n", 0x0);
    s = trimwhitespace(s);

    if (!isnumber(s))
        return (void)printf("%s: expected a numeric value\n", s);
    
    printf(format, atoi(s));
    printf("\n");
}

void pwd(struct tty* tty, bool lf) {
    char cwd[MAX_DIRS * MAX_DIR_NAME];
    memset(cwd, 0x0, sizeof(cwd));

    // concatenate cwd directories
    for (int i = 0; i < MAX_DIRS && tty->cwd[i][0] != '\0'; i++) {
        memcpy(cwd + strlen(cwd), tty->cwd[i], MAX_DIR_NAME);
        memcpy(cwd + strlen(cwd), "/", 1);
    }
    
    printf("%s", cwd);
    if (lf)
        printf("\n");
}
