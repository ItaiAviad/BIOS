#pragma once
#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stdbool.h>
#include <types.h>
#include <stdarg.h>
#include <ctype.h>

#define EOF (-1)

typedef struct _iobuf
{
    char*   _ptr;
    int _cnt;
    char*   _base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char*   _tmpfname;
} FILE;

static __attribute__((__unused__))FILE stdin_file = { 0, 0, 0, 0, 0, 0, 0, 0 };
static __attribute__((__unused__)) FILE stdout_file = { 0, 0, 0, 0, 0, 0, 0, 0 };
static __attribute__((__unused__)) FILE stderr_file = { 0, 0, 0, 0, 0, 0, 0, 0 };
static __attribute__((__unused__)) FILE* stdin = &stdin_file;
static __attribute__((__unused__)) FILE* stdout = &stdout_file;
static __attribute__((__unused__)) FILE* stderr = &stderr_file;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Syscall wrapper to clear stdin buffer
 * 
 */
void stdin_clear();

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

char *fgets(char *str, int n, FILE *stream);

/**
 * @brief Trim whitespaces (only leading and trailing)
 * 
 * @param s 
 * @return char* 
 */
char *trimwhitespace(char *s);
char *rmwhitespace(char *s);
/**
 * @brief Trim whitespaces (all whitespaces (more than 1 consecutive))
 * 
 * @param s 
 * @return char* 
 */
char *ftrimwhitespace(char *s);


#ifdef __cplusplus
}
#endif

#endif
