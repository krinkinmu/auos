static const unsigned rows = 24;
static const unsigned cols = 80;
static const char attr = 7;

static volatile char *video_mem = (char *)0xb8000;
static unsigned row = 0;
static unsigned col = 0;

static void newline(void)
{
	unsigned i, off;

	row = (row + 1) % rows;
	col = 0;
	for (i = 0, off = (col + row * cols) * 2; i != cols; ++i) {
		*(video_mem + off++) = ' ';
		*(video_mem + off++) = attr;
	}
}

void myputchar(int c)
{
	const unsigned off = (col + row * cols) * 2;

	if (c == '\r') {
		col == 0;
		return;
	}

	if (c == '\n') {
		newline();
		return;
	}

	*(video_mem + off) = c;
	*(video_mem + off + 1) = attr;

	if (++col == cols)
		newline();
}

void myputs(const char *str)
{
	while (*str)
		myputchar(*str++);
}

void clrscr(void)
{
	unsigned i;

	for (i = 0; i != rows; ++i)
		myputs("\n");
}
