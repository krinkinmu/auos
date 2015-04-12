#include <kernel/console.h>
#include <kernel/utility.h>
#include <arch/memory.h>

static const size_t ROWS = 24;
static const size_t COLS = 80;
static const char ATTR = 7;
static char *const VMEM = (char *)(PAGE_OFFSET + 0xB8000);

static size_t row;
static size_t col;

static void newline(void)
{
	const size_t shift = (col + row * COLS) * 2;

	for (size_t i = 0; i != COLS - col; ++i) {
		*(VMEM + shift + i) = 0;
		*(VMEM + shift + i + 1) = ATTR;
	}

	col = 0;
	row = (row + 1) % ROWS;
}

static void putchar(char c)
{
	const size_t shift = (col + row * COLS) * 2;

	if (c == '\n') {
		newline();
		return;
	}

	*(VMEM + shift) = c & 0xFF;
	*(VMEM + shift + 1) = ATTR;

	if (++col == COLS)
		newline();
}

static void write(const char *buf, size_t size)
{
	for (size_t i = 0; i != size; ++i)
		putchar(buf[i]);
}

static struct console vga_console = {
	.write = &write
};

void init_vga_console(void)
{
	/* fill video memory with zeroes */
	memset(VMEM, 0, ROWS * COLS * 2);
	register_console(&vga_console);
}
