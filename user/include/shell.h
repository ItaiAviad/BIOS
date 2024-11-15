// Shell

#pragma once
#ifndef SHELL_H
#define SHELL_H

#include <types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>

#define PROMPT "> "

#define COMMAND_SIZE 0x100
#define MAX_COMMANDS 0x10
#define MAX_DIR_NAME 0x20
#define MAX_DIRS 0x20

#define CMD_DELIM " "

struct tty {
    char cmds[MAX_COMMANDS][COMMAND_SIZE];

    int csize; // command count
    char cwd[MAX_DIRS][MAX_DIR_NAME];
};

void shell_init(void);
void print_prompt(struct tty*);
void get_cmd(struct tty*);
void parse_cmd(struct tty*);

// shell commands
#define EHCO "echo"
#define PRINTF "printf"
#define PWD "pwd"
#define SHUTDOWN "shutdown"

// shell functions
/**
 * @brief Print formatted string (linux like)
 * 
 * @param c command (PRINTF) 
 * @param cmd remaining string
 */
void shell_printf(char* c, char* cmd);
void pwd(struct tty*, bool);

#endif
