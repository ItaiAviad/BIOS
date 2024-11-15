#pragma once
#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>


#include <arch/x86_64/io.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);
void shutdown(void);

/**
 * @brief Convert an integer to a string
 * 
 * @param int64_t number
 * @param char* str
 * @param int base (e.g.: 10, 16, 2)
 * 
 * @return char* str
 */
char* itoa(int64_t, char*, int);
/**
 * @brief Convert string to int
 * 
 * @param nptr 
 * @return int 
 */
uint64_t atoi(const char *nptr);

#ifdef __cplusplus
}
#endif

#endif
