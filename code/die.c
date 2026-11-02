#include "die.h"

#include <stdnoreturn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

noreturn void die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "FATAL: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}