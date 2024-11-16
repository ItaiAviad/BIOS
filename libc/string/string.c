#include <string.h>

char *strchr(const char *s, int c) {
    // Iterate through the string
    while (*s) {
        // If the current char matches the target, return its pointer
       if (*s == (char)c) return (char *)s;
        // Move to the next char
        s++;
    }
    // If target char is null terminator, return pointer to null terminator
    return c ? NULL : (char *)s;
}


char *strtok(char *str, const char *delim) {
    static char *start = NULL, *end;

    // If a new string is provided, update the start pointer
    if (str) start = str;

    // If there are no more tokens, return NULL
    if (!start) return NULL;

    end = start;
    // Find the end of the current token
    while (*end && !strchr(delim, *end)) end++;

    // If the token is empty, return NULL
    if (end == start) { start = NULL; return NULL; }

    // If we've reached the end of the string, return the last token
    if (!*end) { char *token = start; start = NULL; return token; }

    // Null-terminate the token and update the start pointer for the next token
    *end++ = 0;
    char *token = start;
    start = end;

    return token;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strcat(char *dst, char *src) {
    char *dst_save = dst;

    // Move to end of dst
    while (*dst != '\0')
        dst++;
    // concat src to dst
    while (*src != '\0')
        *(dst++) = *(src++);
    // null-terminate dst
    *dst = '\0';

    return dst_save;
}

char *strjoin(char *dst, int argc, char *argv[], const char *delim) {
    char *dst_save = dst;
    char *src = NULL;

    while (*dst != '\0')
        dst++;

    // concat with argv and delim
    for (int i = 0; i < argc - 1; i++) {
        // concat argv[i]
        src = argv[i];
        while (*src != '\0')
            *(dst++) = *(src++);

        // concat delim
        src = (char*) delim;
        while (*src != '\0')
            *(dst++) = *(src++);
    }
    src = argv[argc - 1];
    while (*src != '\0')
        *(dst++) = *(src++);

    // null-terminate dst
    *dst = '\0';

    return dst_save;
}
