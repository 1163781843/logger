/*!
 * Copyright (c) 2017-03-03, pxie@grandstream.cn
 * All rights reserved.
 * filename: testlog.h
 *
 * Current Version: 1.0
 * Author: pxie@grandstream.cn
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

enum {
	LOG_ERR = 1,
	LOG_WAR = 2,
	LOG_NOT = 3,
	LOG_DEB = 4,
	LOG_VEB = 5,
};

void __LOG(int level, const char *file, int line, const char *format, ...);

#define LOG(level, ...)														\
	do {																	\
		if (level) {														\
			__LOG(level, __FILE__, __LINE__, __VA_ARGS__);					\
		}																	\
	} while (0)

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#endif
