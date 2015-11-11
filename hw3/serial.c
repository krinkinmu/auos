#include "serial.h"
#include "ioport.h"
#include "defs.h"
#include "idt.h"

#define COM_PORT   0x3F8
#define COM_IRQ    4
#define CLOCK_FREQ 115200
#define UART_SPEED 9600
#define CLOCK_FDIV (CLOCK_FREQ / UART_SPEED)

#define DLB_REG                0
#define DHB_REG                1
#define TX_DATA_REG            0
#define RX_DATA_REG            0

#define IE_REG                 1
#define ENABLE_RX_DATA         BITU(0)
#define ENABLE_TX_DATA         BITU(1)
#define ENABLE_LINE_STATUS     BITU(2)
#define ENABLE_MODEM_STATUS    BITU(3)

#define II_REG                 2
#define PENDING_INTERRUPT_MASK BITU(0)
#define INTERRUPT_PENDING      0
#define INTERRUPT_TYPE_MASK    (BITU(1) | BITU(2))
#define MODEM_STATUS_CHANGED   0
#define TX_INTERRUPT           BITU(1)
#define RX_INTERRUPT           BITU(2)
#define LINE_STATUS_CHANGED    (BITU(1) | BITU(2))

#define FC_REG                 2
#define ENABLE_FIFO            BITU(0)
#define CLEAR_RX_BUFFER        BITU(1)
#define CLEAR_TX_BUFFER        BITU(2)
#define FIFO_BUFFER_1          0
#define FIFO_BUFFER_4          BITU(6)
#define FIFO_BUFFER_8          BITU(7)
#define FIFO_BUFFER_14         (BITU(6) | BITU(7))

#define LC_REG                 3
#define DATA_BITS_5            0
#define DATA_BITS_6            BITU(0)
#define DATA_BITS_7            BITU(1)
#define DATA_BITS_8            (BITU(0) | BITU(1))
#define STOP_BITS_1            0
#define STOP_BITS_2            BITU(2)
#define PARITY_NONE            0
#define PARITY_ODD             BITU(3)
#define PARITY_EVEN            (BITU(3) | BITU(4))
#define DLAB                   BITU(7)

#define LS_REG                 5
#define RX_DATA                BITU(0)
#define TX_READY               BITU(5)

#define RING_BUFFER_SIZE 4096

struct ring_buffer {
	char buffer[RING_BUFFER_SIZE];
	unsigned begin, end, size;
};

static int ring_buffer_empty(const struct ring_buffer *buffer)
{ return buffer->size == 0; }

static int ring_buffer_full(const struct ring_buffer *buffer)
{ return buffer->size == RING_BUFFER_SIZE; }

static unsigned ring_buffer_size(const struct ring_buffer *buffer)
{ return buffer->size; }

static void ring_buffer_putchar(struct ring_buffer *buffer, int data)
{
	buffer->buffer[buffer->end] = data;
	buffer->end = (buffer->end + 1) % RING_BUFFER_SIZE;
	buffer->size++;
}

static int ring_buffer_getchar(struct ring_buffer *buffer)
{
	const int data = buffer->buffer[buffer->begin];

	buffer->begin = (buffer->begin + 1) % RING_BUFFER_SIZE;
	buffer->size--;
	return data;
}

static struct ring_buffer rx_buffer;

static void serial_irq_handler(int intno)
{
	int status = in8(COM_PORT + LS_REG);

	while (status & RX_DATA) {
		const int data = in8(COM_PORT + RX_DATA_REG);

		if (!ring_buffer_full(&rx_buffer))
			ring_buffer_putchar(&rx_buffer, data);
		status = in8(COM_PORT + LS_REG);
	};
}

size_t serial_read(char *buf, size_t size)
{
	unsigned long flags = irqsave();
	unsigned count = ring_buffer_size(&rx_buffer);
	unsigned i;

	if (count > size)
		count = size;

	for (i = 0; i != count; ++i)
		*buf++ = ring_buffer_getchar(&rx_buffer);
	irqrestore(flags);

	return count;
}

void serial_setup(void)
{
	rx_buffer.begin = rx_buffer.end = rx_buffer.size = 0;

	out8(COM_PORT + IE_REG, 0);
	out8(COM_PORT + LC_REG, DLAB);

	out8(COM_PORT + DLB_REG, CLOCK_FDIV & 0xFF);
	out8(COM_PORT + DHB_REG, (CLOCK_FDIV & 0xFF00) >> 8);

	out8(COM_PORT + LC_REG, DATA_BITS_8 | PARITY_NONE | STOP_BITS_1);
	out8(COM_PORT + FC_REG, ENABLE_FIFO | FIFO_BUFFER_14);

	setup_irq_handler(COM_IRQ, serial_irq_handler);
	irq_unmask(COM_IRQ);
	out8(COM_PORT + IE_REG, ENABLE_RX_DATA);
}
