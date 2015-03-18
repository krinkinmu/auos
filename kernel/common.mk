-include $(OBJECTS:.o=.d)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

%.o: %.S
	$(CC) $(AFLAGS) -MMD -c $< -o $@
