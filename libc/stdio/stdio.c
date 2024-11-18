#include <stdio.h>
#include <string.h>

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
