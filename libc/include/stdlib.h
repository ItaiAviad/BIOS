#pragma once
#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

/**
 * @brief Convert an integer to a string
 * 
 * @param int number
 * @param char* str
 * @param int base (e.g.: 10, 16, 2)
 * 
 * @return char* str
 */
char* itoa(int, char*, int);
/**
 * @brief Convert string to int
 * 
 * @param nptr 
 * @return int 
 */
int atoi(const char *nptr);

#ifdef __cplusplus
}
#endif

#endif
