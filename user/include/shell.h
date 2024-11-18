// Shell

#pragma once
#ifndef SHELL_H
#define SHELL_H

#include <types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>

#define SHELL_NAME "bioshell"

#define PROMPT "> "

#define MAX_CMDS_CACHE 0x10
#define MAX_DIR_NAME 0x20
#define MAX_DIRS 0x20
#define MAX_CMD_STR 0x10
#define MAX_CMDS 0x100

#define ARGV_DELIM " "

struct tty {
    char **cache[MAX_CMDS_CACHE]; // argv cache (previous commands' argv)
    uint64_t cargc [MAX_CMDS_CACHE]; // argc cache (previous commands' argc)
    int ci; // cache current index

    char cwd[MAX_DIRS][MAX_DIR_NAME];

    bool alive;
};

// main shell tty
struct tty tty1;

void shell_init(void);
// shell destructor
void shell_fini(void);

void print_prompt(void);
/**
 * @brief Get command argv from user
 * 
 * @return int - command index in cache
 */
int get_cmd(struct tty*);
void parse_cmd(int argc, char *argv[]);

// shell functions

// command not found
void shcmd_notfound(int argc, char *argv[]);

// Echo string
void echo(int argc, char *argv[]);

// Print formatted string (linux like)
void shcmd_printf(int argc, char *argv[]);

// Print working directory
void pwd(int argc, char *argv[]);

// Shutdown
void shcmd_shutdown(int argc, char *argv[]);

// Exit
void shcmd_exit(int argc, char *argv[]);

// Calculator
void shcmd_bc(int argc, char *argv[]);

typedef void (*shcmd)(int argc, char *argv[]);

// Notice! Indices of shcmd_str eand shcmd_table must match!
static const char shcmd_str[MAX_CMDS][MAX_CMD_STR] = {
    "echo",
    "printf",
    "pwd",
    "shutdown",
    "exit",
    "bc",
};

static const shcmd shcmd_table[MAX_CMDS] = {
    echo,
    shcmd_printf,
    pwd,
    shcmd_shutdown,
    shcmd_exit,
    shcmd_bc,
};

#endif
