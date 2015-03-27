void setup_arch(void *bootstrap);

void start_kernel(void *bootstrap)
{
	setup_arch(bootstrap);
	while (1);
}
