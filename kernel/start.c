void start_kernel(void *bootstrap)
{
	void setup_arch(void *bootstrap);

	setup_arch(bootstrap);
	while (1);
}
