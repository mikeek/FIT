/**
 * Example of ptrace(2) usage.
 *
 * Source:
 * http://stackoverflow.com/questions/7514837/why-this-ptrace-programe-always-saying-syscall-returned-38/7522990#7522990
 * Author: Matthew Slattery, 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h> // struct user_regs_struct
#include <sys/syscall.h> // __NR_* constants
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>

#include <stdbool.h>
#include "logger.h"

#ifndef __x86_64
#error Target architecture of the source code is x86_64
#endif

pid_t child_pid;
static int out_type = OUT_NONE;
static FILE *fd;

void child(char *argv[], char *envp[])
{
	/* Request tracing by parent: */
	ptrace(PTRACE_TRACEME, 0, NULL, NULL);

	/* Stop before doing anything, giving parent a chance to catch the exec: */
	kill(getpid(), SIGSTOP);

	/* Now exec: */
	execve(argv[0], argv, envp);
	//    execl("/bin/ls", "ls", NULL);
}

struct syscall_status {
	unsigned long syscall;
	int status;
};

/// syscall_stack BEGIN
/**
 * system call stack -- each system call may be ptrace_stopped multiple times,
 * normally 2-times: before_enter, after_exit
 */
#define syscall_stack_capacity 100
struct syscall_status syscall_stack[syscall_stack_capacity];
int syscall_stack_top = -1;

struct syscall_status *top_syscall()
{
	return syscall_stack_top < 0 ? NULL : &syscall_stack[syscall_stack_top];
}

void push_syscall(unsigned long syscall)
{
	if (top_syscall() && (top_syscall()->syscall == syscall))
		top_syscall()->status += 1;
	else {
		syscall_stack_top++;
		assert(syscall_stack_top < syscall_stack_capacity);
		syscall_stack[syscall_stack_top].syscall = syscall;
		syscall_stack[syscall_stack_top].status = 1;
	}
}

void pop_syscall()
{
	syscall_stack_top--;
	assert(syscall_stack_top >= -1);
}

/// syscall_stack END

void handle_call(struct user_regs_struct *regs)
{
	long syscall = regs->orig_rax;

	push_syscall(syscall);

	if (top_syscall()->status < 2) {
		return;
	}

	switch (syscall) {
	case __NR_close:
	{
		struct arg_s arg[] = {
			{NUMBER, "fd"}};
		log_syscall(out_type, fd, __NR_close, child_pid, "close", 1, arg, regs);
		break;
	}
	case __NR_fchdir:
	{
		struct arg_s arg[] = {
			{NUMBER, "fd"}};
		log_syscall(out_type, fd, __NR_fchdir, child_pid, "fchdir", 1, arg, regs);
		break;
	}
	case __NR_fchown:
	{
		struct arg_s arg[] = {
			{NUMBER, "fd"},
			{UNUMBER, "owner"},
			{UNUMBER, "group"}};
		log_syscall(out_type, fd, __NR_fchown, child_pid, "fchown", 3, arg, regs);
		break;
	}
	case __NR_chdir:
	{
		struct arg_s arg[] = {
			{STRING, "path"}};
		log_syscall(out_type, fd, __NR_chdir, child_pid, "chdir", 1, arg, regs);
		break;
	}
	case __NR_rmdir:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"}};
		log_syscall(out_type, fd, __NR_rmdir, child_pid, "rmdir", 1, arg, regs);
		break;
	}
	case __NR_unlink:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"}};
		log_syscall(out_type, fd, __NR_rmdir, child_pid, "unlink", 1, arg, regs);
		break;
	}
	case __NR_chroot:
	{
		struct arg_s arg[] = {
			{STRING, "path"}};
		log_syscall(out_type, fd, __NR_chroot, child_pid, "chroot", 1, arg, regs);
		break;
	}
	case __NR_rename:
	{
		struct arg_s arg[] = {
			{STRING, "oldpath"},
			{STRING, "newpath"}};
		log_syscall(out_type, fd, __NR_chroot, child_pid, "rename", 2, arg, regs);
		break;
	}
	case __NR_chown:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"},
			{UNUMBER, "owner"},
			{UNUMBER, "group"}};
		log_syscall(out_type, fd, __NR_chown, child_pid, "chown", 3, arg, regs);
		break;
	}
	case __NR_getcwd:
	{
		struct arg_s arg[] = {
			{STRING, "buf"},
			{UNUMBER, "size"}};
		log_syscall(out_type, fd, __NR_getcwd, child_pid, "getcwd", 2, arg, regs);
		break;
	}
		/* ---symbolic--- */
	case __NR_open:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"},
			{ SYMBOLIC, "flags"},
			{SYMBOLIC, "mode"}};
		log_syscall(out_type, fd, __NR_open, child_pid, "open", 3, arg, regs);
		break;
	}
	case __NR_access:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"},
			{SYMBOLIC, "mode"}};
		log_syscall(out_type, fd, __NR_access, child_pid, "access", 2, arg, regs);
		break;
	}
	case __NR_mkdir:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"},
			{SYMBOLIC, "mode"}};
		log_syscall(out_type, fd, __NR_mkdir, child_pid, "mkdir", 2, arg, regs);
		break;
	}
	case __NR_chmod:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"},
			{SYMBOLIC, "mode"}};
		log_syscall(out_type, fd, __NR_chmod, child_pid, "chmod", 2, arg, regs);
		break;
	}
	case __NR_fchmod:
	{
		struct arg_s arg[] = {
			{NUMBER, "fd"},
			{SYMBOLIC, "mode"}};
		log_syscall(out_type, fd, __NR_fchmod, child_pid, "fchmod", 2, arg, regs);
		break;
	}
		/* ---struct--- */
	case __NR_stat:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"},
			{STRUCT, "buf"}};
		log_syscall(out_type, fd, __NR_stat, child_pid, "stat", 2, arg, regs);
		break;
	}
	case __NR_lstat:
	{
		struct arg_s arg[] = {
			{STRING, "pathname"},
			{STRUCT, "buf"}};
		log_syscall(out_type, fd, __NR_lstat, child_pid, "lstat", 2, arg, regs);
		break;
	}
	case __NR_getdents:
	{
		struct arg_s arg[] = {
			{ UNUMBER, "fd"},
			{STRUCT, "dirp"},
			{UNUMBER, "count"}};
		log_syscall(out_type, fd, __NR_getdents, child_pid, "getdents", 3, arg, regs);
		break;
	}
	case __NR_getdents64:
	{
		struct arg_s arg[] = {
			{UNUMBER, "fd"},
			{STRUCT, "dirp"},
			{UNUMBER, "count"}};
		log_syscall(out_type, fd, __NR_getdents64, child_pid, "getdents64", 3, arg, regs);
		break;
	}
	default:
		break;
	}

	pop_syscall();
}

void parent()
{
	int status;

	//    printf("|---------+------------------|\n");
	//    printf("| syscall | retcode          |\n");
	//    printf("|---------+------------------|\n");

	while (1) {
		/* Wait for child status to change: */
		wait(&status);

		if (WIFEXITED(status)) {
			//            printf("|---------+------------------|\n");
			//            printf("Child exit with status %d\n", WEXITSTATUS(status));
			break;
			;
		}
		if (WIFSIGNALED(status)) {
			//            printf("|---------+------------------|\n");
			//            printf("Child exit due to signal %d\n", WTERMSIG(status));
			break;
			;
		}
		if (!WIFSTOPPED(status)) {
			//            printf("|---------+------------------|\n");
			//            printf("wait() returned unhandled status 0x%x\n", status);
			break;
			;
		}
		if (WSTOPSIG(status) == SIGTRAP) {
			/* Note that there are *three* reasons why the child might stop
			 * with SIGTRAP:
			 *  1) syscall entry
			 *  2) syscall exit
			 *  3) child calls exec
			 */

			struct user_regs_struct regs;
			ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

			//            printf("| %7ld | %16ld |\n", (long)regs.orig_rax, (long)regs.rax);

			handle_call(&regs);

		} else {
			// printf("Child stopped due to signal %d\n", WSTOPSIG(status));
			//            printf("|     N/A | Signal %2d        |\n", WSTOPSIG(status));
		}
		fflush(stdout);

		/* Resume child, requesting that it stops again on syscall enter/exit
		 * (in addition to any other reason why it might stop):
		 */
		ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
	}
	log_end(out_type, fd);
}

void usage(const char *name)
{
	fprintf(stderr, "usage: %s [-x <file>.xml | -p <file>.pl] /path/to/command [args]\n", name);
}

int main(int argc, char *argv[], char *envp[])
{
	if (argc < 2) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	int index = 1;
	if (!strcmp(argv[1], "-x")) {
		if (argc < 4) {
			usage(argv[0]);
			return EXIT_FAILURE;
		}
		out_type = OUT_XML;
	} else if (!strcmp(argv[1], "-p")) {
		if (argc < 4) {
			usage(argv[0]);
			return EXIT_FAILURE;
		}
		out_type = OUT_PROLOG;
	}

	if (out_type != OUT_NONE) {
		index = 3;
		fd = fopen(argv[2], "w");
		if (fd == NULL) {
			fprintf(stderr, "Cannot open file %s\n", argv[2]);
			return EXIT_FAILURE;
		}
	} else {
		fd = stdout;
	}

	log_begin(out_type, fd, argv[index], argc - index - 1, &argv[index + 1]);
	child_pid = fork();

	if (child_pid == 0) {
		child(&argv[index], envp);
	} else {
		parent();
	}
	return 0;
}
