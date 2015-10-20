#include "ioport.h"

enum PIT_CHANNEL {
	PIT_CH0 = 0,
	PIT_CH1 = 1 << 6,
	PIT_CH2 = 1 << 7
};

enum PIT_ACCESS {
	PIT_LATCH = 0,
	PIT_LOBYTE = 1 << 4,
	PIT_HIBYTE = 1 << 5,
};

enum PIT_MODE {
	PIT_ONE_SHOT = 0,
	PIT_HW_ONE_SHOT = 1 << 1,
	PIT_RATE = 1 << 2,
	PIT_SQ_WAVE = (1 << 1) | (1 << 2),
	PIT_STROBE = 1 << 3,
	PIT_HW_STROBE = (1 << 3) | (1 << 1)
};

static void pit_write_cmd(unsigned char cmd)
{
	static const unsigned short PIT_CMD_PORT = 0x43;

	out8(PIT_CMD_PORT, cmd);
}

static unsigned short pit_data_port(int channel)
{
	static const unsigned short PIT_CH0_DATA_PORT = 0x40;
	static const unsigned short PIT_CH1_DATA_PORT = 0x41;
	static const unsigned short PIT_CH2_DATA_PORT = 0x42;

	switch (channel) {
	case PIT_CH0:
		return PIT_CH0_DATA_PORT;
	case PIT_CH1:
		return PIT_CH0_DATA_PORT;
	case PIT_CH2:
		return PIT_CH0_DATA_PORT;
	}
	return 0;
}

static void pit_write_reload(unsigned short reload, int channel, int access)
{
	const unsigned short port = pit_data_port(channel);

	if (access & PIT_LOBYTE) out8(port, reload & 0xFF);
	if (access & PIT_HIBYTE) out8(port, (reload >> 8) & 0xFF);
}

static void pit_set_rate(int channel, unsigned short reload)
{
	const int access = PIT_LOBYTE | PIT_HIBYTE;

	pit_write_cmd(channel | access | PIT_RATE);
	pit_write_reload(reload, channel, access);
}

static unsigned long pit_get_reload(unsigned long msecs)
{
	static const unsigned long FREQ = 1193182ul;

	return (msecs * FREQ) / 1000ul;
}

void pit_set_channel0_rate(unsigned long msecs)
{ pit_set_rate(PIT_CH0, pit_get_reload(msecs)); }
