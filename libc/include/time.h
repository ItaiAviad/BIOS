#pragma once
#ifndef _TIME_H
#define _TIME_H 1

#include <sys/cdefs.h>


#include <types.h>
#include <arch/x86_64/io.h>

#ifndef TIMEZONE
#define TIMEZONE "UTC"
#endif
#ifndef CURRENT_YEAR
#define CURRENT_YEAR 2024 // Change this each year!
#endif

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get current UNIX time in seconds (since 1970-01-01 00:00:00)
 * 
 * @return uint64_t seconds
 */
uint64_t time();
/**
 * @brief Print current date. Format: <weekday> <d> <m_name> <y> <h>:<m>:<s> <timezone>
 * 
 */
void date();

#ifdef __cplusplus
}
#endif

#endif
