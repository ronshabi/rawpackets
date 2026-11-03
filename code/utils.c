#include "utils.h"

#include <stdio.h>

void hexdump(uint8_t *buf, size_t buf_size, size_t column_width, const char *before)
{
	static const char *hexdigits = "0123456789abcdef";
	// printf("--- dump %p (%zu)---\n", buf, buf_size);

	for (size_t i = 0; i < buf_size; ++i) {

		// FIXME: find a more elegant way to do this
		if (i == 0) {
			printf("%s", before);
		}
		
		uint8_t byte = buf[i];
		char msn = hexdigits[byte >> 4];
		char lsn = hexdigits[byte & 0xF];

		putchar(msn);
		putchar(lsn);

		if ((i + 1) % column_width == 0) {
			putchar('\n');
			printf("%s", before);
		} else {
			putchar(' ');
		}
	}

	if (buf_size % column_width != 0) {
		putchar('\n');
	}
}