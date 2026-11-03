#include "utils.h"

#include <stdio.h>

void hexdump(uint8_t *buf, size_t buf_size, size_t column_width)
{
	static const char *hexdigits = "0123456789abcdef";
	printf("--- dump %p (%zu)---\n", buf, buf_size);

	for (size_t i = 0; i < buf_size; ++i) {
		uint8_t byte = buf[i];
		char msn = hexdigits[byte >> 4];
		char lsn = hexdigits[byte & 0xF];

		putchar(msn);
		putchar(lsn);

		if ((i + 1) % column_width == 0) {
			putchar('\n');
		} else {
			putchar(' ');
		}
	}

	if (buf_size % column_width != 0) {
		putchar('\n');
	}
}