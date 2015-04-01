#ifndef __UTILITY_H__
#define __UTILITY_H__

void *memset(void *ptr, int value, unsigned long size);
void *memcpy(void *ptr, const void *src, unsigned long size);
void ultoa(char *buf, unsigned long d, int base);
void ltoa(char *buf, long d, int base);

#endif /*__UTILITY_H__*/
