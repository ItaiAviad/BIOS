#include <ctype.h>
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

int isxdigit(char c) {
    return (c >= '0' && c <= '9') || 
           (c >= 'a' && c <= 'f') || 
           (c >= 'A' && c <= 'F');
}

int tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}
