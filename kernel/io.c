#include <kernel/utility.h>
#include <kernel/console.h>
#include <kernel/kernel.h>
#include <kernel/io.h>

#include <stdarg.h>

void putchar(int c)
{
	struct list_head *it = consoles.next;
	const char ch = c;

	while (it != &consoles) {
		const struct console *const console =
					container_of(it, struct console, link);

		console->write(&ch, 1);
		it = it->next;
	}
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
			utoa(buffer, value, (c == 'x' ? 16 : 10));
			puts_no_newline(buffer);
		} else if (c == 'd') {
			int value = va_arg(vararg_lst, int);
			itoa(buffer, value, (c == 'x' ? 16 : 10));
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
