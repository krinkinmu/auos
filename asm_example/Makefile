CC ?= gcc
LD ?= ld

LFLAGS := -s -nostdlib -nostdinc

all: hello

hello: hello.o hello.ld
	$(LD) $(LFLAGS) $< -T hello.ld -o $@

%.o: %.S
	$(CC) -c $^ -o $@

.PHONY: clean
clean:
	rm -f *.o hello
