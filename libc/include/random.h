#pragma once
#ifndef _RANDOM_H
#define _RANDOM_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
