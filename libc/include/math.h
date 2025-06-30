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


#define MIN(a, b)         ((a) < (b) ? (a) : (b))
#define MAX(a, b)         ((a) < (b) ? (b) : (a))
#define UPPER_DIVIDE(n,d) (((n) + (d) - 1) / (d))

#ifdef __cplusplus
}
#endif

#endif
