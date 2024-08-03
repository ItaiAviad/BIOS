#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool print(const char* data, size_t length) {
    const unsigned char* bytes = (const unsigned char*) data;
    for (size_t i = 0; i < length; i++)
        if (putchar(bytes[i]) == EOF)
            return false;
    return true;
}

int printf(const char* format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;
        
        // Printing literal text (in format)
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
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;
        // Printing parameters
		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(str, len))
				return -1;
			written += len;
        } else if (*format == 'd') {
            format++;
            int i = va_arg(parameters, int);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            char str[32 + 1];
            itoa(i, str, 10);
            size_t len = strlen(str);
            if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
            if (!print(str, len))
				return -1;
			written += len;
        } else if (*format == 'x') {
            format++;
            int i = va_arg(parameters, int);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            char str[32 + 2 + 1]; // +1 - for '\0', +2 - for '0x'
            char tmp[32 + 1];
            itoa(i, tmp, 16); // Int to string
            // Add '0x'
            memmove(str + 2, tmp, strlen(tmp) + 1);
            str[0] = '0';
            str[1] = 'x';
            // print(tmp, strlen(tmp));
            size_t len = strlen(str);
            if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
            if (!print(str, len))
				return -1;
			written += len;
        } else if (*format == 'b') {
            format++;
            unsigned int i = va_arg(parameters, unsigned int);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            char str[32 * CHAR_BIT + 2 + 1]; // +1 for '\0', +2 for '0b'
            char *ptr = str + 32 * CHAR_BIT + 2;
            *ptr = '\0';
            do {
                *--ptr = (i & 1) ? '1' : '0';
                i >>= 1;
            } while (i > 0);
            *--ptr = 'b';
            *--ptr = '0';
            size_t len = strlen(ptr);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(ptr, len))
                return -1;
            written += len;
        } else { // Unsupported format (Continue printing literal text)
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}
