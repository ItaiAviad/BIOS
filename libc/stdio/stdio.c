#include <stdio.h>
#include <string.h>

int isdigit(int c) {
    return c >= '0' && c <= '9';
}

int isnumber(char* s) {
    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++)
        if (!isdigit(s[i]))
            return false;
    return true;
}

int isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

char *trimwhitespace(char *s) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*s)) s++;

    if (*s == 0)  // All spaces?
        return s;

    // Trim trailing space
    end = s + strlen(s) - 1;
    while(end > s && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return s;
}

char *rmwhitespace(char *s) {
    int i, j;
    for (i = 0, j = 0; s[i]; i++)
        if (!isspace(s[i]))
            s[j++] = s[i];
    s[j] = '\0';

    return s;
}

char *ftrimwhitespace(char *s) {
    int i = 0, j = 0;
    int sf = 0; // space flag

    while (s[i] != '\0') {
        if (s[i] != ' ') {
            s[j++] = s[i];
            sf = 0;  // Reset the space flag
        } else if (!sf) {  // Allow one space
            s[j++] = ' ';
            sf = 1;
        }
        i++;
    }
    s[j] = '\0';  // Null-terminate the modified string

    return s;
}

