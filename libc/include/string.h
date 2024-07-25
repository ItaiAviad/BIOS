#pragma once
#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compare two memory blocks
 * 
 * @return int 
 */
int memcmp(const void*, const void*, size_t);
/**
 * @brief Copy memory from src to dst
 * 
 * @param void* `dstptr`
 * @param void* `srcptr`
 * @param size_t 
 * @return void* 
 */
void* memcpy(void* __restrict, const void* __restrict, size_t);
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
void* memset(void*, int, size_t);
/**
 * @brief Get length of a string (until `\0`)
 * 
 * @return size_t 
 */
size_t strlen(const char*);

#ifdef __cplusplus
}
#endif

#endif
