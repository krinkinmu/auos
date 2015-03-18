static const unsigned ROWS = 80;
static const unsigned COLS = 24;
static const char ATTR = 7;
static char * const VMEM = (char *)0xB8000;

static void * memset(void *ptr, int value, unsigned long size);
static void newline(void);
static int putchar(int c);
static int puts(const char *asciz);

static unsigned row;
static unsigned col;

static void * memset(void *ptr, int value, unsigned long size)
{
	const char *end = (char *)ptr + size;

        for (char *begin = ptr; begin != end; ++begin)
		*begin = value;

	return ptr;
}

static void newline(void)
{
	col = 0;
	row = (row + 1) % ROWS;
}

static int putchar(int c)
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

static int puts(const char *asciz)
{
	while (*asciz)
		putchar(*asciz++);
	putchar('\n');
	return 0;
}

void main(void)
{
	/* fill video memory with zeroes */
	memset(VMEM, 0, ROWS * COLS * 2);
	puts("Hello, World!");
	while (1);
}
