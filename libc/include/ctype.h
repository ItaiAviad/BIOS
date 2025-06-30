#pragma once
#ifndef _CTYPE_H
#define _CTYPE_H 1

#include <sys/cdefs.h>
#include <types.h>
#include <stdbool.h>

int isxdigit(char c);
int isdigit(int c);
int isnumber(char* s);
int isspace(int c);
int tolower(int c);

#endif
