// Shell

#pragma once
#ifndef SHELL_H
#define SHELL_H

#include <types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>

#define SHELL_NAME "bioshell"

#define PROMPT "> "

#define MAX_CMDS_CACHE 0x10
#define MAX_PATH_SIZE 0x100
#define MAX_DIRS 0x20
#define MAX_CMD_STR 0x40
#define MAX_CMDS 0x100

#define ARGV_DELIM " "


struct tty {
    char **cache[MAX_CMDS_CACHE]; // argv cache (previous commands' argv)
    uint64_t cargc [MAX_CMDS_CACHE]; // argc cache (previous commands' argc)
    int64_t ci; // current index

    int64_t tci; // temp/traverse current index (for traversing cache)

    char curr_wd[MAX_PATH_SIZE];

    bool alive;
    bool active;
};
// main shell tty - shell mode
struct tty tty0;


// shell constructor
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
void shcmd_echo(int argc, char *argv[]);

// Print formatted string (linux like)
void shcmd_printf(int argc, char *argv[]);

// Print working directory
void shcmd_pwd(int argc, char *argv[]);

// Shutdown
void shcmd_shutdown(int argc, char *argv[]);

// Exit
void shcmd_exit(int argc, char *argv[]);

// Basic Calculator
void shcmd_bc(int argc, char *argv[]);

// Clear tty
void shcmd_clear(int argc, char *argv[]);

// nslookup
void shcmd_nslookup(int argc, char *argv[]);

// Change dir
void shcmd_cd(int argc, char *argv[]);

typedef void (*shcmd)(int argc, char *argv[]);

// Notice! Indices of shcmd_str eand shcmd_table must match!
static const char shcmd_str[MAX_CMDS][MAX_CMD_STR] = {
    "echo",
    "printf",
    "pwd",
    "shutdown",
    "exit",
    "bc",
    "clear",
    "nslookup",
    "cd"
};

static const shcmd shcmd_table[MAX_CMDS] = {
    shcmd_echo,
    shcmd_printf,
    shcmd_pwd,
    shcmd_shutdown,
    shcmd_exit,
    shcmd_bc,
    shcmd_clear,
    shcmd_nslookup,
    shcmd_cd
};


void traverse_cache(int offset);

// Extended Keycodes Handlers

// Cursor Up Handler
void cuph();

// Cursor Left Handler
void clefth();

// Cursor Right Handler
void crighth();

// Cursor Down Handler
void cdownh();

#endif
