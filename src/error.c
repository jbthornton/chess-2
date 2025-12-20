#include "error.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void error(const char* msg, ...){
	fprintf(stderr, "\x1b[31mERR: ");

	va_list ap;
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);

	fprintf(stderr, "\x1b[0m");
	exit(EXIT_FAILURE);
}

void warning(const char* msg, ...){
	fprintf(stderr, "\x1b[33m");

	va_list ap;
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);

	fprintf(stderr, "\x1b[0m");
}
