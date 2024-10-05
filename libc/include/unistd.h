#pragma once
#ifndef _UNISTD_H
#define _UNISTD_H 1

#include <sys/cdefs.h>


#include <types.h>
#include <arch/x86_64/io.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sleep for a specified number of milliseconds
 * 
 * @param uint64_t delay (in milliseconds)
 */
void sleep(uint64_t ms);

#ifdef __cplusplus
}
#endif

#endif
