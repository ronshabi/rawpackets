#include "rs_context.h"

int main()
{
	struct rs_context ctx;
	rs_context_init(&ctx);
	rs_context_start(&ctx);
	rs_context_teardown(&ctx);
	return 0;
}