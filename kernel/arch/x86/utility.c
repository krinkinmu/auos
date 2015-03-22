void * memset(void *ptr, int value, unsigned long size)
{
	const char *end = (char *)ptr + size;

        for (char *begin = ptr; begin != end; ++begin)
		*begin = value;

	return ptr;
}
