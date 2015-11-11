#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stddef.h>

size_t serial_read(char *buf, size_t size);
void serial_setup(void);

#endif /*__SERIAL_H__*/

