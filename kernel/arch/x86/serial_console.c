#include <kernel/kernel.h>
#include <kernel/console.h>
#include <arch/ioport.h>

#define SERIAL_PORT_IO_BASE 0x3f8
#define REG_DATA            (SERIAL_PORT_IO_BASE)
#define REG_DLL             (SERIAL_PORT_IO_BASE)
#define REG_IER             (SERIAL_PORT_IO_BASE + 1)
#define REG_DLH             (SERIAL_PORT_IO_BASE + 1)
#define REG_FCR             (SERIAL_PORT_IO_BASE + 2)
#define REG_LCR             (SERIAL_PORT_IO_BASE + 3)
#define REG_LSR             (SERIAL_PORT_IO_BASE + 5)

#define FCR_EFIFO           BITU(0)
#define FCR_14BYTES         (BITU(6) | BITU(7))
#define LCR_8BIT            (BITU(0) | BITU(1))
#define LCR_DLAB            BITU(7)
#define LSR_TX_READY        BITU(5)

static void putchar(char c)
{
	while (!(inb(REG_LSR) & LSR_TX_READY));
	outb(REG_DATA, c);
}

static void write(const char *buf, unsigned long size)
{
	for (unsigned i = 0; i != size; ++i)
		putchar(buf[i]);
}

static struct console serial_console = {
	.write = write
};

static void init_polling_mode(void)
{
	/* disable all interrupts */
	outb(REG_IER, 0);
	/* enable access to frequency divisor */
	outb(REG_LCR, LCR_DLAB);
	/* set 9600 baud rate (timer frequency is 115200, divide it by 12) */
	outb(REG_DLL, 0x0C);
	outb(REG_DLH, 0x00);
	/* set frame format (8 bit, no parity, 1 stop bit) and drop DLAB */
	outb(REG_LCR, LCR_8BIT);
	/* enable 14 byte length FIFO buffer */
	outb(REG_FCR, FCR_EFIFO | FCR_14BYTES);
}

void init_serial_console(void)
{
	init_polling_mode();
	register_console(&serial_console);
}
