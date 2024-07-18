#pragma once
#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

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

/**
 * @brief Set a seed for the random generator
 * 
 * @param seed 
 */
void srand(uint32_t seed);
/**
 * @brief Generate a random number
 * 
 * @return uint32_t 
 */
uint32_t rand(void);

/**
 * @brief Return base**exp (base to the power of exp)
 * 
 * @param base 
 * @param exp 
 * @return int 
 */
int pow(int base, int exp);

#ifdef __cplusplus
}
#endif

#endif
