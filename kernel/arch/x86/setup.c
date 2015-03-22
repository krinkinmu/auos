#include "multiboot.h"
#include "io.h"

void main(void *bootstrap)
{
	(void)bootstrap;

	init_io();
	puts("Hello, World!");
	while (1);
}
