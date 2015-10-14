CC ?= gcc
LD ?= ld

all: mbr multiboot

multiboot: multiboot.o hello_32.o multiboot.ld
	ld -s -nostdlib -nostdinc -T multiboot.ld -o $@ multiboot.o hello_32.o
	strip -R .eh_frame -R .comment -R .shstrtab $@

mbr: mbr.o hello_32.o mbr.ld
	ld -s -T mbr.ld -o $@ mbr.o hello_32.o

multiboot.o: multiboot.S
	$(CC) -m32 -c $^ -o $@

mbr.o: mbr.S
	$(CC) -m32 -c $^ -o $@

hello_32.o: hello.c
	$(CC) -m32 -fno-zero-initialized-in-bss -ffreestanding -Os -c $^ -o $@

.PHONY: clean
clean:
	rm -f mbr multiboot mbr.o hello_32.o multiboot.o
