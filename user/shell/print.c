// Shell print related functions
#include <shell.h>

void shcmd_echo(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
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
