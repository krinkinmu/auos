#include <stdarg.h>
#include "utility.h"
#include "io.h"

static const unsigned ROWS = 24;
static const unsigned COLS = 80;
static const char ATTR = 7;
static char * const VMEM = (char *)0xB8000;

static unsigned row;
static unsigned col;

static void newline(void)
{
	col = 0;
	row = (row + 1) % ROWS;
}

void putchar(int c)
{
	const unsigned shift = (col + row * COLS) * 2;

	if (c == '\n') {
		newline();
		return;
	}

	*(VMEM + shift) = c & 0xFF;
	*(VMEM + shift + 1) = ATTR;

	if (++col == COLS)
		newline();
}

static void puts_no_newline(const char *asciz)
{
	while (*asciz)
		putchar(*asciz++);
}

void puts(const char *asciz)
{
	puts_no_newline(asciz);
	putchar('\n');
}

void printf(const char *fmt, ...)
{
	char buffer[20];
	int c;

	va_list vararg_lst;
	va_start(vararg_lst, fmt);

	while ((c = *fmt++) != 0) {
		if (c != '%') {
			putchar(c);
			continue;
		}

		c = *fmt++;
		if (c == 'u' || c == 'x') {
			unsigned value = va_arg(vararg_lst, unsigned);
			ultoa(buffer, value, (c == 'x' ? 16 : 10));
			puts_no_newline(buffer);
		} else if (c == 'd') {
			int value = va_arg(vararg_lst, int);
			ltoa(buffer, value, (c == 'x' ? 16 : 10));
			puts_no_newline(buffer);
		} else if (c == 's') {
			puts_no_newline(va_arg(vararg_lst, const char *));
		} else if (c == 'c') {
			putchar(va_arg(vararg_lst, int));
		} else {
			putchar(c);
		}
	}

	va_end(vararg_lst);
}

void init_io()
{
	/* fill video memory with zeroes */
	memset(VMEM, 0, ROWS * COLS * 2);
}
