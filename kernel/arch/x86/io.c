#include "utility.h"
#include "io.h"

static const unsigned ROWS = 80;
static const unsigned COLS = 24;
static const char ATTR = 7;
static char * const VMEM = (char *)0xB8000;

static unsigned row;
static unsigned col;

static void newline(void)
{
	col = 0;
	row = (row + 1) % ROWS;
}

int putchar(int c)
{
	const unsigned shift = (col + row * COLS) * 2;

	if (c == '\n') {
		newline();
		return c;
	}

	*(VMEM + shift) = c & 0xFF;
	*(VMEM + shift + 1) = ATTR;

	if (++col == COLS)
		newline();

	return c;
}

int puts(const char *asciz)
{
	while (*asciz)
		putchar(*asciz++);
	putchar('\n');
	return 0;
}

void init_io()
{
	/* fill video memory with zeroes */
	memset(VMEM, 0, ROWS * COLS * 2);
}