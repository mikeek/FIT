/* 
 * File:   logger.h
 * Author: Michal Kozubik, xkozub03
 *
 * Created on 2. květen 2014
 */

#ifndef LOGGER_H
#define	LOGGER_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/user.h>

enum {
	NUMBER,
	UNUMBER,
	POINTER,
	SYMBOLIC,
	STRING,
	STRUCT
};

typedef struct linux_dirent {
	long d_ino;
	off_t d_off;
	unsigned short d_reclen;
	char d_name[];
} dirstruct;

struct sym_pair {
	unsigned long long int value;
	char *name;
};

struct arg_s {
	int type;
	char *name;
};

enum {
	OUT_NONE,
	OUT_XML,
	OUT_PROLOG
};

unsigned long long int *arg_ptr(struct user_regs_struct *regs, int arg);

unsigned long long int arg_val(struct user_regs_struct *regs, int arg);

void log_syscall(int type, FILE *fd, int id, int pid, const char *name, int argc,
		struct arg_s *args, struct user_regs_struct *regs);

void log_begin(int type, FILE *fd, const char *binary, int argc, char **args);

void log_end(int type, FILE *fd);

#endif	/* LOGGER_H */

