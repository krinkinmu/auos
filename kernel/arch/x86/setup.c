#include "multiboot.h"
#include "io.h"

void main(void)
{
	init_io();
	puts("Hello, World!");
	while (1);
}
