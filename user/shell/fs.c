// Shell File System related functions
#include <shell.h>

char *preprocess_path(const char *original_string) {
    if (!original_string || *original_string == '\0') {
        return NULL; // Invalid input
    }
    size_t len = strlen(original_string);
    char *ret_str = malloc(len + 1);
    if (!ret_str) {
        return NULL; // Allocation failed
    }
    char *current = ret_str;
    int last_was_slash = 0;

    while (*original_string && !isspace(*original_string)) {
        if (*original_string == '/') {
            if (!last_was_slash) { // Only add if last wasn't a slash
                *current++ = '/';
            }
            last_was_slash = 1;
        } else {
            *current++ = *original_string;
            last_was_slash = 0;
        }
        original_string++;
    }
    *current = '\0'; // Null-terminate

    // Remove trailing separators (but keep root `/` intact)
    while (current > ret_str + 1 && *(current - 1) == '/') {
        *(--current) = '\0';
    }
    return ret_str;
}

void shcmd_pwd(int argc, char *argv[]) {
    printf("%s", tty0.curr_wd);
    if (argv && argc) // argv exists (pwd command called)
        printf("\n");
}

void shcmd_cat(int argc, char *argv[]) {

    char* path_pre_processed = NULL;
    if(argc <= 1){
        printf("Path to cat wasn't provided");
    }
    else if(*(argv[1]) == '/' ){
        path_pre_processed = preprocess_path(argv[1]);
    }
    else{
        char* joined = malloc(strlen(tty0.curr_wd)+strlen(argv[1])+2);
        memcpy(joined, tty0.curr_wd, strlen(tty0.curr_wd)+1);
        joined[strlen(tty0.curr_wd)] = '/';
        strcat(joined, argv[1]);
        path_pre_processed = preprocess_path(joined);
        free(joined);
    }


    switch (path_exists(path_pre_processed)) {
        case 2:
        {
            int fd = open(path_pre_processed, O_RDONLY);
            size_t size = lseek(fd, 0, SEEK_END);
            char* buff = malloc(size);
            lseek(fd, 0, SEEK_SET);
            read(fd, buff, size);
            close(fd);
            printf("%s\n",buff);
            break;
        }
        case 0:{
            printf("The path doesn't exist\n");
            break;
        }
        default:{
            printf("The path exists but is a dir, So can't cat this path.\n");
            break;
        }
    }
    free(path_pre_processed);

}

void shcmd_ls(int argc, char *argv[]) {

    char* path_pre_processed = NULL;
    if(argc <= 1){
        path_pre_processed = preprocess_path(tty0.curr_wd);
    }
    else if(*(argv[1]) == '/' ){
        path_pre_processed = preprocess_path(argv[1]);
    }
    else{
        char* joined = malloc(strlen(tty0.curr_wd)+strlen(argv[1])+2);
        memcpy(joined, tty0.curr_wd, strlen(tty0.curr_wd)+1);
        joined[strlen(tty0.curr_wd)] = '/';
        strcat(joined, argv[1]);
        path_pre_processed = preprocess_path(joined);
        free(joined);
    }


    switch (path_exists(path_pre_processed)) {
        case 1:
        {
            linkedListNode* list = list_dir(path_pre_processed);

            while(list){
                printf("%s\n",list->data);
                list = list->next;
            }
            break;
        }
        case 0:{
            printf("The path doesn't exist!\n");
            break;
        }
        default:{
            printf("The path exists but isn't a dir!\n");
            break;
        }
    }
    free(path_pre_processed);

}

void shcmd_cd(int argc, char *argv[]) {
    if(argc <= 1){
        printf("Path to change work directory to wasn't provided\n");
    }

    char* path_pre_processed = NULL;
    if(*(argv[1]) == '/' ){
        path_pre_processed = preprocess_path(argv[1]);
    }
    else{
        char* joined = malloc(strlen(tty0.curr_wd)+strlen(argv[1])+2);
        memcpy(joined, tty0.curr_wd, strlen(tty0.curr_wd)+1);
        joined[strlen(tty0.curr_wd)] = '/';
        strcat(joined, argv[1]);
        path_pre_processed = preprocess_path(joined);
        free(joined);
    }


    switch (path_exists(path_pre_processed)) {
        case 1:
        {
            size_t length = strlen(path_pre_processed);
            memcpy(tty0.curr_wd, path_pre_processed, length+1);
            tty0.curr_wd[length] = 0;
            break;
        }
        case 2:{
            printf("The path exists but isn't a dir\n");
            break;
        }
        default:{
            printf("The path doesn't exist\n");
            break;
        }
    }
    free(path_pre_processed);

}

