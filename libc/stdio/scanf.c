#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>

#if defined(__is_libk)
#include <arch/x86_64/io.h>
#endif

char* gets_s(char* str, size_t size) {
    // Clear buffer before new input
    stdin_clear();

    int c = 0;
    uint32_t i = 0;
    do {
        c = getchar();
        if (c == CAN) { // cancel all input, clear buffer and printf backspaces
            for (unsigned int i = 0; i < strlen(str); ++i)
                printf("%c", '\b');
            i = 0;
            memset(str, 0, strlen(str));
            continue;
        }
        if (c >= KEYCODES_EXT_START) // Ignore extended keycodes: see `getchar()`
            continue;
        if (c == 0x1B) // Ignore ESC
            continue;
        if (c == '\b') // Limit Backspaces
        {
            if (i > 0) {
                i--;
                str[i] = '\0';
            }
            else if (i == 0 && str[i] != '\0') {
                str[i] = '\0';
            }
            else
                continue;
        }
        if (i >= size - 1) // Only Backspace allowed after limit reached
            continue;
        
        printf("%c", (char)c);

        if (c != '\n' && c != EOF && c != '\0' && c != '\b') // Only store valid chars
            str[i++] = c;

    } while (c != '\n' && c != EOF && c != '\0');

    if (c == EOF || i == 0)
        return NULL;

    for (i = i; i < size; i++)
        str[i] = '\0';
    return str;
}

char *fgets(char *str, int n, __attribute__((unused)) FILE *stream) {
    char* ret = NULL;
// #if defined(__is_libk)
    ret = gets_s(str, n);
// #else
    // ret = (char*) syscall(sys_fgets, str, n, stream);
// #endif
    return ret;
}

char* gets(char* str) {
    return gets_s(str, INT32_MAX);
}

/**
 * @brief Check if char is valid.
 * Valid char: any char except '\\b', '\\r', esc, '\\n', '\\t', ' ', '\\0', EOF
 * 
 * @param c 
 * @return true 
 * @return false 
 */
bool is_valid_char(char c) {
    return (c >= '!' && c <= '~');
    // return c != '\b' && c != '\r' && c != 0x1B && c != '\t' && c != ' ' && c != '\n' && c != EOF && c != '\0';
}

/**
 * @brief Get a valid char from `str`, maximum length `size`.
 * Valid char: any char except '\\b', '\\r', esc, '\\n', '\\t', ' ', '\\0', EOF
 * 
 * @param str 
 * @param size 
 * @return int 
 */
int get_next_valid_char_idx(const char* str, size_t size) {
    char c = 0;
    int i = 0;
    do
    {
        if (i >= (int)size)
            return -1;
        c = str[i++];
    } while (!is_valid_char(c));
    return i - 1;
}

/**
 * @brief Get a valid string from `str`, maximum length `len`, and store it in `dst` (null terminated).
 * Valid string:  string without special characters (including spaces).
 * E.g.: "HW" - valid, "H W" - invalid (two valid strings), "HW\\n" - invalid (only "HW" is valid)
 * 
 * 
 * @param str pointer to input string
 * @param len maximum length
 * @param dst pointer to output string
 * @return int - next str index (after valid string taken)
 */
int get_next_valid_string(const char* str, size_t len, char* dst) {
    const char* base_str = str;

    bool is_next_valid = false;
    uint32_t i = 0;
    for (i = 0; i < len; i++) {
        int idx = get_next_valid_char_idx(str, len); // Get next valid char index
        if (idx == -1)
            break;
        dst[i] = str[idx];
        // Check if str[idx+1] is valid (if not, break)
        is_next_valid = is_valid_char(str[idx + 1]);
        str = str + idx + 1; // +1 to skip current char
        if (!is_next_valid)
            break;
    }

    // Skip remaining valid chars (of current valid string)
    while (is_valid_char(*str))
        str++;

    // If i == 0, then no valid string was found, keep the original string
    // If i > 0, fill remaining with `\0` (only if len > 1 -> not char)
    if (i != 0 && len > 1)
        dst[i + 1] = '\0';
        // for (uint32_t j = i; j < len; j++)
    
    return (int) (str - base_str);
}

int sscanf_args(const char* str, const char* restrict format, va_list args) {
    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;
        
        // Avoiding literal text (in format)
        if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			format += amount;
			continue;
		}

        format++;
		if (*format == 'c') {
			format++;
			char* c = (char*) va_arg(args, char*);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
            int next_idx = get_next_valid_string(str, 1, c);
			// Only increment written if value written to dst (str incremented)
            str += next_idx;
			written += next_idx > 0 ? 1 : 0;
		} else if (*format == 's') { // Read string with maximum length
			format++;
			char* dst = va_arg(args, char*);
			size_t len = strlen(dst);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
            int next_idx = get_next_valid_string(str, strlen(str), dst);
            // Only increment written if value written to dst (str incremented)
            str += next_idx;
			written += next_idx > 0 ? 1 : 0;
        } else if (*format == 'd') { // Read string and convert to int
            format++;
			int* dst = va_arg(args, int*);
            char dst_str[BUFFER_SIZE];
            itoa(*dst, dst_str, 10);
            int next_idx = get_next_valid_string(str, strlen(str), dst_str);
            *dst = atoi(dst_str);
            // Only increment written if value written to dst (str incremented)
			str += next_idx;
			written += next_idx > 0 ? 1 : 0;
        }
	}

	return written;
}

int sscanf(const char* str, const char* restrict format, ...) {
    va_list args;
    va_start(args, format);

    int written = sscanf_args(str, format, args);

    va_end(args);

    return written;
}

int scanf(const char* restrict format, ...) {
    int res;

    va_list args;
    va_start(args, format);

    #if defined(__is_libk)

    char buf[BUFFER_SIZE];
    gets_s(buf, BUFFER_SIZE);
    res = sscanf_args(buf, format, args);

    #else 
    // Syscall
    res = fvsyscall(sys_scanf, (uint64_t) format, args);
    #endif

    va_end(args);

    return res;
}
