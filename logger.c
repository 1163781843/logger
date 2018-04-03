/*!
 * Copyright (c) 2017-03-03, pxie@grandstream.cn
 * All rights reserved.
 * filename: testlog.c
 *
 * Current Version: 1.0
 * Author: pxie@grandstream.cn
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <regex.h>

#include "logger.h"

#define RAII_VAR(vartype, varname, initval, dtor)									\
	auto void _dtor_ ## varname (vartype *v);										\
	void _dtor_ ## varname (vartype *v) {											\
		if (*v != NULL) {															\
			dtor(*v);																\
			(*v) = NULL;															\
		}																			\
	}																				\
	vartype varname    __attribute__((cleanup(_dtor_ ## varname))) = (initval);		\

static struct timeval tv_now(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);

	return t;
}

static void system_time(char *sys_tv)
{
	time_t tt;
	tt = time(NULL);
	struct tm *t = localtime(&tt);
	struct timeval tv = tv_now();

	snprintf(sys_tv, 256 - 1, "[%4d-%02d-%02d %02d:%02d:%02d:%03ld]", t->tm_year+1900,
		t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec / 1000);
}

static int get_tid(void)
{
	return syscall(SYS_gettid);;
}

static int regex_match_file_name(const char *file, char **result)
{
	regex_t reg;
	int nmatch = 1;
	regmatch_t pmatch[1] = {0};

	RAII_VAR(char *, err_buf, malloc(sizeof(char) * 1024), free);
	RAII_VAR(char *, match, malloc(sizeof(char) * 1024), free);

	int err = 0;

	err = regcomp(&reg, "([a-z]|[A-Z]|[0-9]|[_])+\\.[c|(cpp)|h]+$", REG_EXTENDED);
	if (err < 0) {
		memset(err_buf, 0, sizeof(char) * 1024);
		regerror(err, &reg, err_buf, sizeof(err_buf) - 1);
		printf("Compile regex pattern failed because of '%s'!", err_buf);
		return -1;
	}

	err = regexec(&reg, file, nmatch, pmatch, 0);
	if (err == REG_NOMATCH) {
		printf("Not match");
		regfree(&reg);
		return -1;
	} else if (err) {
		regerror(err, &reg, err_buf, sizeof(err_buf) - 1);
		printf("Match regex pattern failed because of '%s'!", err_buf);
		regfree(&reg);
		return -1;
	}

	int len = pmatch[0].rm_eo - pmatch[0].rm_so;
	if(len) {
		memset(match, 0, sizeof(char) * 1024);
		memcpy(match, file + pmatch[0].rm_so, len);
		*result = strdup(match);
	}
	regfree(&reg);

	return 0;
}

void __LOG(int level, const char * file, int line, const char * format, ...)
{
	RAII_VAR(char *, sys_tv, malloc(sizeof(char) * 256), free);

	char *log_buffer = NULL;
	size_t length = 0;

	va_list ap;
	va_start(ap, format);
	length = vasprintf(&log_buffer, format, ap);
	va_end(ap);

	char *file_name = NULL;

	system_time(sys_tv);
	RAII_VAR(char *, find_file, NULL, free);

	if (file) {
		file_name = strrchr(file, '/');
	}

	char *level_str = NULL;
	switch (level) {
		case LOG_ERR:
			level_str = "\033[31;1mERR\33[0m";
			break;
		case LOG_WAR:
			level_str = "\033[32;31;1mWAR\33[0m";
			break;
		case LOG_NOT:
			level_str = "\033[33;1mNOT\33[0m";
			break;
		case LOG_DEB:
			level_str = "\033[32;1mDEB\33[0m";
			break;
		case LOG_VEB:
			level_str = "\033[32mVEB\33[0m";
			break;
		default:
			level_str = "\033[32;1mDEB\33[0m";
			break;
	}

	fprintf(stderr, "%s %s [%05d]   -- %s:%d  %s\n",
		sys_tv, level_str, get_tid(), file_name ? ++file_name : file, line, log_buffer);

	log_buffer ? free(log_buffer) : NULL;
}
