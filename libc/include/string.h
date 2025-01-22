#pragma once
#include <types.h>
#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>


#include <arch/x86_64/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compare two memory blocks
 * 
 * @return int 
 */
int memcmp(const unsigned char* a, const unsigned char *b, size_t size);
/**
 * @brief Copy memory from src to dst
 * 
 * @param void* `dstptr`
 * @param void* `srcptr`
 * @param size_t 
 * @return void* 
 */
void* memcpy(void*, const void*, size_t);
/**
 * @brief Move memory from src to dst
 * 
 * @param void* `dstptr`
 * @param void* `srcptr`
 * @param size_t
 * @return void* 
 */
void* memmove(void*, const void*, size_t);
/**
 * @brief Fill memory with a value
 * 
 * @return void* 
 */
void* memset(void*, char, size_t);
/**
 * @brief Get length of a string (until `\0`)
 * 
 * @return size_t 
 */
size_t strlen(const char*);


/**
 * @brief Find the first occurrence of a character in a string
 * 
 * @param str 
 * @param c 
 * @return char* - pointer to first occurrence
 */
char *strchr(const char *str, int c);

/**
 * @brief Tokenize a string
 * 
 * @param str 
 * @param delim 
 * @return char* - pointer to next token
 */
char *strtok(char *str, const char *delim);

/**
 * @brief Compare two strings
 * 
 * @param s1 
 * @param s2 
 * @return int 
 */
int strcmp(const char *s1, const char *s2);

/**
 * @brief Concatenate two strings
 * 
 * @param dst 
 * @param src 
 * @return char* 
 */
char *strcat(char *dst, char *src);

/**
 * @brief Concatenate multiple strings with a delimiter into dst
 * 
 * @param dst 
 * @param argc 
 * @param argv 
 * @param delim 
 * @return char* 
 */
char *strjoin(char *dst, int argc, char *argv[], const char *delim);


#ifdef __cplusplus
}
#endif

#endif
