// Shell File System related functions
#include <shell.h>

void shcmd_pwd(int argc, char *argv[]) {
    char cwd[MAX_DIRS * MAX_DIR_NAME];
    memset(cwd, 0x0, sizeof(cwd));

    // concatenate cwd directories
    for (int i = 0; i < MAX_DIRS && tty0.cwd[i][0] != '\0'; i++) {
        memcpy(cwd + strlen(cwd), tty0.cwd[i], MAX_DIR_NAME);
        memcpy(cwd + strlen(cwd), "/", 1);
    }
    
    printf("%s", cwd);
    if (argv && argc) // argv exists (pwd command called)
        printf("\n");
}
