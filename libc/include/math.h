#pragma once
#ifndef _MATH_H
#define _MATH_H 1

#include <sys/cdefs.h>



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return base**exp (base to the power of exp)
 * 
 * @param base 
 * @param exp 
 * @return int 
 */
int pow(int, int);
int min(int, int);
int max(int, int);
int upper_divide(int numerator, int denominator);
#ifdef __cplusplus
}
#endif

#endif
