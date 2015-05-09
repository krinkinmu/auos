ARCH        ?= x86

CWD         := $(CURDIR)
KERN_DIR    := $(CWD)
ARCH_DIR    := $(KERN_DIR)/arch/$(ARCH)
INCLUDES    := -I$(KERN_DIR)/include -I$(KERN_DIR)/include/$(ARCH)
BASE_LFLAGS := -nostdlib -nostdinc
BASE_CFLAGS := -ffreestanding -m32 -g -O2 $(INCLUDES) -std=c99 \
	-Wall -Wextra -Werror -std=c99 -pedantic
BASE_AFLAGS := -DASM_FILE -m32 -g $(INCLUDES)

export CC          ?= clang
export LD          ?= ld
export OBJCOPY     ?= objcopy
export MAKE        ?= make
export CFLAGS      += $(BASE_CFLAGS)
export AFLAGS      += $(BASE_AFLAGS)
export LFLAGS      += $(BASE_LFLAGS)

.PHONY: all
all: image

include makefile.mk
export OBJECTS     := $(addprefix $(KERN_DIR)/, $(C_SOURCES:.c=.o))

image:
	$(MAKE) --directory=$(ARCH_DIR) image

.PHONY: clean
clean:
	rm -f *.o *.d
	$(MAKE) --directory=$(ARCH_DIR) clean
