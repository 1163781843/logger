AUTOMAKE_OPTIONS=foreign

dist_include_HEADERS = logger.h

lib_LTLIBRARIES = liblogger.la
liblogger_la_SOURCES = logger.c
liblogger_la_CFLAGS = $(AM_CFLAGS)
liblogger_la_LDFLAGS = -version-info 1:1 $(AM_LDFLAGS) -no-undefined -export-dynamic
