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

