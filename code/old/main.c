#include "die.h"
#include "rs_context.h"
int main()
{
	struct rs_context ctx;

	if (rs_context_init(&ctx)) {
		die("couldn't init context");
	}

	if (rs_context_start(&ctx)) {
		die("context eventloop error");
	}

	rs_context_teardown(&ctx);
	return 0;
}