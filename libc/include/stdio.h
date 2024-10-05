#pragma once
#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stdbool.h>
#include <types.h>
#include <stdarg.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Write a formatted output to stdout
 * 
 * @param 
 * @param ... 
 * @return int - number of characters written
 */
int printf(const char*, ...);
/**
 * @brief Write a character to stdout
 * 
 * @return int - ascii value
 */
int putchar(int);
/**
 * @brief Write a string to stdout
 * 
 * @return int - number of characters written
 */
int puts(const char*);
/**
 * @brief Read a character from stdin
 * 
 * @return int 
 */
int getchar();
/**
 * @brief Reads a string from stdin
 * 
 * @param str 
 * @return char* - pointer to string
 */
char* gets(char*);
/**
 * @brief `gets` with size limit
 * 
 * @return char* - pointer to string
 */
char* gets_s(char*, size_t);
/**
 * @brief Reads formatted input from stdin
 * 
 * @param format 
 * @param ... 
 * @return int - number of input items read
 */
int scanf(const char*, ...);
/**
 * @brief Reads formatted input from given string
 * 
 * @param str input string
 * @param format 
 * @param ... 
 * @return int - number of input items read
 */
int sscanf(const char *, const char*, ...);

#ifdef __cplusplus
}
#endif

#endif
