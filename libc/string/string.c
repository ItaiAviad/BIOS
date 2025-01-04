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

char *strrchr(const char *str, int c) {
    const char *last_occurrence = NULL;

    // Iterate through the string
    while (*str) {
        if (*str == (char)c) {
            last_occurrence = str; // Update the pointer to the last occurrence
        }
        str++;
    }

    // Check if the character to search for is the null terminator
    if (c == '\0') {
        return (char *)str; // Return a pointer to the null terminator
    }

    return (char *)last_occurrence; // Return the last occurrence, or NULL if not found
}


char *strstr(const char *haystack, const char *needle) {
    // Handle special cases: empty needle
    if (!needle || *needle == '\0') {
        return (char *)haystack; // Return the start of haystack
    }

    // Handle special case: empty haystack
    if (!haystack || *haystack == '\0') {
        return NULL;
    }

    const char *h, *n;
    for (; *haystack != '\0'; haystack++) {
        // Start comparing from the current position in haystack
        h = haystack;
        n = needle;

        // Compare characters of needle and haystack
        while (*h == *n && *n != '\0') {
            h++;
            n++;
        }

        // If we've reached the end of the needle, we found a match
        if (*n == '\0') {
            return (char *)haystack;
        }
    }

    // No match found
    return NULL;
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
