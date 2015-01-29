/* 
 * File:   logger.c
 * Author: Michal Kozubik, xkozub03
 *
 * Created on 2. květen 2014
 */


#define _GNU_SOURCE

#include "logger.h"
#include <fcntl.h>
#include <stdbool.h>
#include <sys/user.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

/* opravneni pro open */
#define OPEN_PERMS {S_IRWXU, "S_IRWXU"}, {S_IRUSR, "S_IRUSR"}, {S_IWUSR, "S_IWUSR"},\
					{S_IXUSR, "S_IXUSR"}, {S_IRWXG, "S_IRWXG"}, {S_IRGRP, "S_IRGRP"},\
					{S_IWGRP, "S_IWGRP"}, {S_IXGRP, "S_IXGRP"}, {S_IRWXO, "S_IRWXO"},\
					{S_IROTH, "S_IROTH"}, {S_IWOTH, "S_IWOTH"}, {S_IXOTH, "S_IXOTH"}

#define OPEN_PERMS_CNT 12

/* timestamp pro logovani do *.pl */
static int timestamp = 1;

/* ziskani ukazatele na parametr volani */
unsigned long long int *arg_ptr(struct user_regs_struct *regs, int arg)
{
	switch (arg) {
	case 0: return &(regs->rdi);
	case 1: return &(regs->rsi);
	case 2: return &(regs->rdx);
	case 3: return &(regs->r10);
	case 4: return &(regs->r8);
	case 5: return &(regs->r9);
	default: return NULL;
	}
}

/* ziskani hodnoty parametru funkce */
unsigned long long int arg_val(struct user_regs_struct *regs, int arg)
{
	switch (arg) {
	case 0: return regs->rdi;
	case 1: return regs->rsi;
	case 2: return regs->rdx;
	case 3: return regs->r10;
	case 4: return regs->r8;
	case 5: return regs->r9;
	default: return 0;
	}
}

/* tisk stringu z pameti */
void print_str(FILE *fd, int pid, unsigned long long int reg, int out_type)
{
	long word;
	char *data = (void *) &word;
	unsigned long glob_idx = 0;
	unsigned i = 0;
	bool more = true;

	while (more) {
		word = ptrace(PTRACE_PEEKDATA, pid, (void *) (reg + glob_idx), 0);

		for (i = 0; i < sizeof (word); ++i) {
			if (data[i] == '\0') {
				more = false;
				break;
			}
			if (isprint(data[i])) {
				fputc(data[i], fd);
			} else if (out_type == OUT_PROLOG) {
				fprintf(fd, "\\x%02x\\", data[i]);
			} else {
				fprintf(fd, "\\x%02x", data[i]);
			}
		}
		glob_idx += i;
	}
}

/* tisk symbolic vyznamu hodnoty */
void print_symbolic(FILE *fd, unsigned long long int val, int flags_cnt,
	struct sym_pair *flags, bool first)
{
	int i;
	for (i = 0; i < flags_cnt; ++i) {
		if (val & flags[i].value) {
			if (first) {
				fprintf(fd, "%s", flags[i].name);
				first = false;
			} else {
				fprintf(fd, "|%s", flags[i].name);
			}
		}
	}
}

/* tisk prvku struktury */
void print_items(FILE *fd, int indent, int cnt, struct arg_s *args)
{
	int i;
	for (i = 0; i < cnt; ++i) {
		fprintf(fd, "%*s<item name=\"%s\">\n", indent, " ", args[i].name);
		fprintf(fd, "%*s  <value>%d</value>\n", indent, " ", args[i].type);
		fprintf(fd, "%*s</item>\n", indent, " ");
	}
}

/* tisk struktury timespec */
void print_timespec(FILE *fd, int indent, struct timespec *ts)
{
	fprintf(fd, "%*s<struct name=\"timespec\">\n", indent, " ");
	fprintf(fd, "%*s  <item name=\"tv_nsec\">\n", indent, " ");
	fprintf(fd, "%*s    <value>%ld</value>\n", indent, " ", ts->tv_nsec);
	fprintf(fd, "%*s  </item>\n", indent, " ");
	fprintf(fd, "%*s  <item name=\"tv_sec\">\n", indent, " ");
	fprintf(fd, "%*s    <value>%ld</value>\n", indent, " ", ts->tv_sec);
	fprintf(fd, "%*s  </item>\n", indent, " ");
	fprintf(fd, "%*s</struct>\n", indent, " ");
}

/* tisk stat/lstat struktury */
void print_stat_struct(FILE *fd, int pid, unsigned long long int val)
{
	int indent = 8;
	struct stat sst;
	struct stat *st = &sst;
	unsigned loc_idx = 0;
	long word;
	while (loc_idx < sizeof (struct stat)) {
		word = ptrace(PTRACE_PEEKDATA, pid, (void *) (val + loc_idx), 0);
		printf("readed %ld\n", word);
		memmove(((char *) st) + loc_idx, &word, sizeof (word));
		loc_idx += sizeof (word);
	}

	/* priprava tisku jednotlivych polozek */
	struct arg_s args[] = {
		{st->st_dev, "st_dev"},
		{st->st_ino, "st_ino"},
		{st->st_mode, "st_mode"},
		{st->st_nlink, "st_nlink"},
		{st->st_uid, "st_uid"},
		{st->st_gid, "st_gid"},
		{st->st_rdev, "st_rdev"},
		{st->st_size, "st_size"},
		{st->st_blksize, "st_blksize"},
		{st->st_blocks, "st_blocks"}};

	fprintf(fd, "%*s<struct name=\"stat\">\n", indent, " ");
	print_items(fd, indent + 2, 10, args);

	/* tisk podstruktur */
	fprintf(fd, "%*s  <item name=\"st_atim\">\n", indent, " ");
	print_timespec(fd, indent + 4, &(st->st_atim));
	fprintf(fd, "%*s  </item>\n", indent, " ");

	fprintf(fd, "%*s  <item name=\"st_mtim\">\n", indent, " ");
	print_timespec(fd, indent + 4, &(st->st_mtim));
	fprintf(fd, "%*s  </item>\n", indent, " ");

	fprintf(fd, "%*s  <item name=\"st_ctim\">\n", indent, " ");
	print_timespec(fd, indent + 4, &(st->st_ctim));
	fprintf(fd, "%*s  </item>\n", indent, " ");

	fprintf(fd, "%*s</struct>\n", indent, " ");
}

/* tisk struktury linux_dirent */
void print_linux_dirent(FILE *fd, dirstruct *lindir, int indent)
{
	fprintf(fd, "%*s<struct name=\"linux_dirent\">\n", indent, " ");

	fprintf(fd, "%*s  <item name=\"d_ino\">\n", indent, " ");
	fprintf(fd, "%*s    <value>%lu</value>\n", indent, " ", lindir->d_ino);
	fprintf(fd, "%*s  </item>\n", indent, " ");

	fprintf(fd, "%*s  <item name=\"d_off\">\n", indent, " ");
	fprintf(fd, "%*s    <value>%ld</value>\n", indent, " ", lindir->d_off);
	fprintf(fd, "%*s  </item>\n", indent, " ");

	fprintf(fd, "%*s  <item name=\"d_reclen\">\n", indent, " ");
	fprintf(fd, "%*s    <value>%d</value>\n", indent, " ", lindir->d_reclen);
	fprintf(fd, "%*s  </item>\n", indent, " ");

	int type = *((char *) ((char *) lindir) + lindir->d_reclen - 1);
	fprintf(fd, "%*s  <item name=\"d_type\">\n", indent, " ");
	fprintf(fd, "%*s    <value>%d</value>\n", indent, " ", type);
	fprintf(fd, "%*s    <symbolic>", indent, " ");

	switch (type) {
	case DT_FIFO:
		fprintf(fd, "DT_FIFO");
		break;
	case DT_CHR:
		fprintf(fd, "DT_CHR");
		break;
	case DT_DIR:
		fprintf(fd, "DT_DIR");
		break;
	case DT_BLK:
		fprintf(fd, "DT_BLK");
		break;
	case DT_REG:
		fprintf(fd, "DT_REG");
		break;
	case DT_LNK:
		fprintf(fd, "DT_LNK");
		break;
	case DT_SOCK:
		fprintf(fd, "DT_SOCK");
		break;
	case DT_WHT:
		fprintf(fd, "DT_WHT");
		break;
	default:
		fprintf(fd, "DT_UNKNOWN");
		break;
	}

	fprintf(fd, "</symbolic>\n");

	fprintf(fd, "%*s  </item>\n", indent, " ");

	fprintf(fd, "%*s  <item name=\"d_name\">\n", indent, " ");
	fprintf(fd, "%*s    <value>%s</value>\n", indent, " ", (char *) (lindir->d_name));
	fprintf(fd, "%*s  </item>\n", indent, " ");

	fprintf(fd, "%*s</struct>\n", indent, " ");
}

/* tisk pole struktur u getdents[64] */
void print_getdents_arr(FILE *fd, int pid, unsigned long long int val, int readed)
{
	if (readed <= 0) {
		return;
	}
	int indent = 8;
	fprintf(fd, "%*s<array>\n", indent, " ");

	long word, sofdir = sizeof (dirstruct);
	int glob_idx = 0, loc_idx = 0;

	dirstruct *ptr;

	char *tmpmem = calloc(1, readed);
	while (glob_idx < readed) {
		loc_idx = 0;
		/* ziskani struktury z pameti potomka */
		while (loc_idx < sofdir) {
			word = ptrace(PTRACE_PEEKDATA, pid, (void *) (val + glob_idx + loc_idx), 0);
			memmove(tmpmem + glob_idx + loc_idx, &word, sizeof (word));
			loc_idx += sizeof (word);
		}
		/* ziskani zbytku dat (name) dle d_reclen */
		ptr = (dirstruct *) (tmpmem + glob_idx);
		while (loc_idx < ptr->d_reclen) {
			word = ptrace(PTRACE_PEEKDATA, pid, (void *) (val + glob_idx + loc_idx), 0);
			memmove(tmpmem + glob_idx + loc_idx, &word, sizeof (word));
			loc_idx += sizeof (word);
		}
		/* tisk struktury */
		print_linux_dirent(fd, ptr, indent + 2);
		glob_idx += ptr->d_reclen;
	}

	free(tmpmem);
	fprintf(fd, "%*s</array>\n", indent, " ");
}

/* tisk uvodnich informaci ve formatu xml */
void log_begin_xml(FILE *fd, const char *binary, int argc, char **args)
{
	int i;
	fprintf(fd, "<syscalltrace>\n");
	fprintf(fd, "  <process>\n");
	fprintf(fd, "    <binary>%s</binary>\n", binary);
	if (argc > 0) {
		fprintf(fd, "    <arguments>\n");
		for (i = 0; i < argc; ++i) {
			fprintf(fd, "      <arg>%s</arg>\n", args[i]);
		}
		fprintf(fd, "    </arguments>\n");
	} else {
		fprintf(fd, "    <arguments/>\n");
	}
	fprintf(fd, "  </process>\n");
	fprintf(fd, "  <trace>\n");
}

/* tisk uvodnich informaci ve formatu prolog */
void log_begin_prolog(FILE *fd, const char *binary, int argc, char **args)
{
	int i;
	fprintf(fd, "process('%s', '", binary);
	for (i = 0; i < argc; ++i) {
		fprintf(fd, " %s", args[i]);
	}
	fprintf(fd, "').\n");
}

/* tisk uvodnich informaci v obycejnem formatu */
void log_begin_none(FILE *fd, const char *binary, int argc, char **args)
{
	int i;
	fprintf(fd, "Process: %s", binary);
	for (i = 0; i < argc; ++i) {
		fprintf(fd, " %s", args[i]);
	}
	fprintf(fd, "\n");
}

/* tisk ukoncujicich informaci ve formatu xml */
void log_end_xml(FILE *fd)
{
	fprintf(fd, "  </trace>\n");
	fprintf(fd, "</syscalltrace>\n");
}

/* tisk informaci o volani ve formatu xml */
void log_syscall_xml(FILE *fd, int id, int pid, const char *name, int argc,
	struct arg_s *args, struct user_regs_struct *regs)
{
	int i;
	unsigned long long int val;
	fprintf(fd, "    <syscall id=\"%d\" pid=\"%d\">\n", id, pid);
	fprintf(fd, "      <name>%s</name>\n", name);

	for (i = 0; i < argc; ++i) {
		val = arg_val(regs, i);
		fprintf(fd, "      <arg name=\"%s\">\n", args[i].name);
		switch (args[i].type) {
		case NUMBER:
			fprintf(fd, "        <value>%lld</value>\n", val);
			break;
		case UNUMBER:
			fprintf(fd, "        <value>%llu</value>\n", val);
			break;
		case POINTER:
			if (val == 0) {
				fprintf(fd, "        <value>NULL</value>\n");
			} else {
				fprintf(fd, "        <value>0x%llx</value>\n", val);
			}
			break;
		case STRUCT:
			switch (id) {
			case __NR_stat:
			case __NR_lstat:
				fprintf(fd, "        <value>%lld</value>\n", val);
				print_stat_struct(fd, pid, val);
				break;
			case __NR_getdents:
			case __NR_getdents64:
				print_getdents_arr(fd, pid, val, regs->rax);
				break;
			}
			break;
		case SYMBOLIC:
			fprintf(fd, "        <value>%lld</value>\n", val);
			fprintf(fd, "        <symbolic>");
			switch (id) {
			case __NR_open:
			{
				if (i == 1) {
					bool first = true;
					if (!(val & 0x3)) {
						fprintf(fd, "O_RDONLY");
						first = false;
					}
					struct sym_pair sp[] = {
						{O_RDONLY, "O_RDONLY"},
						{O_WRONLY, "O_WRONLY"},
						{O_RDWR, "O_RDWR"},
						{O_CLOEXEC, "O_CLOEXEC"},
						{O_CREAT, "O_CREAT"},
						{O_DIRECTORY, "O_DIRECTORY"},
						{O_EXCL, "O_EXCL"},
						{O_NOCTTY, "O_NOCTTY"},
						{O_NOFOLLOW, "O_NOFOLLOW"},
						{O_TRUNC, "O_TRUNC"},
						{O_APPEND, "O_APPEND"},
						{O_ASYNC, "O_ASYNC"},
						{O_DIRECT, "O_DIRECT"},
						{O_DSYNC, "O_DSYNC"},
						{O_LARGEFILE, "O_LARGEFILE"},
						{O_NOATIME, "O_NOATIME"},
						{O_NONBLOCK, "O_NONBLOCK"},
						{O_NDELAY, "O_NDELAY"},
						{O_PATH, "O_PATH"} //, {O_TMPFILE, "O_TMPFILE"} <- undeclared
					};
					print_symbolic(fd, val, 19, sp, first);
				} else {
					struct sym_pair sp[] = {OPEN_PERMS};
					print_symbolic(fd, val, OPEN_PERMS_CNT, sp, true);
				}
				break;
			}
			case __NR_access:
			{
				if (val == F_OK) {
					fprintf(fd, "F_OK");
				} else {
					struct sym_pair sp[] = {
						{R_OK, "R_OK"},
						{W_OK, "W_OK"},
						{X_OK, "X_OK"}
					};
					print_symbolic(fd, val, 3, sp, true);
				}
				break;
			}
			case __NR_mkdir:
			{
				struct sym_pair sp[] = {OPEN_PERMS};
				print_symbolic(fd, val, OPEN_PERMS_CNT, sp, true);
				break;
			}
			case __NR_chmod:
			case __NR_fchmod:
			{
				struct sym_pair sp[] = {
					{S_ISUID, "S_ISUID"},
					{S_ISGID, "S_ISGID"},
					{S_ISVTX, "S_ISVTX"},
					{S_IRUSR, "S_IRUSR"},
					{S_IWUSR, "S_IWUSR"},
					{S_IXUSR, "S_IXUSR"},
					{S_IRGRP, "S_IRGRP"},
					{S_IWGRP, "S_IWGRP"},
					{S_IXGRP, "S_IXGRP"},
					{S_IROTH, "S_IROTH"},
					{S_IWOTH, "S_IWOTH"},
					{S_IXOTH, "S_IXOTH"}
				};
				print_symbolic(fd, val, 12, sp, true);
				break;
			}
			}
			fprintf(fd, "</symbolic>\n");
			break;
		case STRING:
			fprintf(fd, "        <value>0x%llx</value>\n", val);
			fprintf(fd, "        <data>");
			print_str(fd, pid, val, OUT_XML);
			fprintf(fd, "</data>\n");
			break;
		}
		fprintf(fd, "      </arg>\n");
	}

	fprintf(fd, "      <retcode>%lld</retcode>\n", regs->rax);
	fprintf(fd, "    </syscall>\n");
}

/* tisk informaci o volani ve formatu prolog */
void log_syscall_pl(FILE *fd, int id, int pid, const char *name, int argc,
	struct arg_s *args, struct user_regs_struct *regs)
{
	int i;
	unsigned long long int val;
	fprintf(fd, "syscall(%d, %d, %d, %lld, '%s'", timestamp++, id, pid, regs->rax, name);

	for (i = 0; i < argc; ++i) {
		val = arg_val(regs, i);
		if (args[i].type == STRING) {
			fprintf(fd, ", '");
			print_str(fd, pid, val, OUT_PROLOG);
			fprintf(fd, "'");
		} else {
			fprintf(fd, ", %lld", val);
		}
	}

	for (; i < 6; ++i) {
		fprintf(fd, ", 0");
	}
	fprintf(fd, ").\n");
}

/* tisk informaci o volani v obycejnem formatu */
void log_syscall_none(FILE *fd, int id, int pid, const char *name, int argc,
	struct arg_s *args, struct user_regs_struct *regs)
{
	int i;
	unsigned long long int val;
	fprintf(fd, "ID | PID | name\n");
	fprintf(fd, "%3d|%5d| %s\n\n", id, pid, name);
	fprintf(fd, "      retval: %lld\n", regs->rax);
	for (i = 0; i < argc; ++i) {
		val = arg_val(regs, i);
		fprintf(fd, "       arg %d: ", i + 1);
		switch (args[i].type) {
		case NUMBER:
		case STRUCT:
			fprintf(fd, "%lld\n", (long long) val);
			break;
		case UNUMBER:
			fprintf(fd, "%llu\n", val);
			break;
		case POINTER:
			if (val == 0) {
				fprintf(fd, "NULL\n");
			} else {
				fprintf(fd, "0x%lx\n", (unsigned long) val);
			}
			break;
		case SYMBOLIC:
		case STRING:
			fprintf(fd, "\"");
			print_str(fd, pid, val, OUT_NONE);
			fprintf(fd, "\"\n");
			break;
		}
	}
	fprintf(fd, "--------------\n");
	fflush(fd);
}

/* tisk informaci o volani */
void log_syscall(int type, FILE *fd, int id, int pid, const char *name, int argc,
	struct arg_s *args, struct user_regs_struct *regs)
{
	switch (type) {
	case OUT_NONE:
		log_syscall_none(fd, id, pid, name, argc, args, regs);
		break;
	case OUT_XML:
		log_syscall_xml(fd, id, pid, name, argc, args, regs);
		break;
	case OUT_PROLOG:
		log_syscall_pl(fd, id, pid, name, argc, args, regs);
		break;
	}
}

/* tisk uvodnich informaci */
void log_begin(int type, FILE *fd, const char *binary, int argc, char **args)
{
	switch (type) {
	case OUT_NONE:
		log_begin_none(fd, binary, argc, args);
		break;
	case OUT_XML:
		log_begin_xml(fd, binary, argc, args);
		break;
	case OUT_PROLOG:
		log_begin_prolog(fd, binary, argc, args);
		break;
	}
}

/* tisk ukoncujicich informaci */
void log_end(int type, FILE *fd)
{
	if (type == OUT_XML) {
		log_end_xml(fd);
	}
}
