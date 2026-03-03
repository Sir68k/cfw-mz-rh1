ifdef DEBUG
	NO_ECHO :=
else
	NO_ECHO := @
endif

CROSS   ?= arm-none-eabi-
CC      = $(CROSS)gcc
LD      = $(CROSS)ld
OCPY    = $(CROSS)objcopy
ODUMP   = $(CROSS)objdump
SZ      = $(CROSS)size
NM      = $(CROSS)nm
MKDIR   = mkdir

CFLAGS +=   -nostdlib\
			-ffreestanding \
			-march=armv4t \
			-mtune=arm7tdmi \
			-mthumb-interwork \
			-ffunction-sections \
			-Os
			
LDFLAGS += -T updater.ld
SRCS = updater.c 
INCLUDES += ./

CFLAGS += $(foreach i,$(INCLUDES),-I$(i)) -DFW_${TARGET_VERSION}=1
OBJS = $(patsubst %.c,build/objs/%.o,$(SRCS))

all: build/updater.bin
	wc -c build/updater.bin

build:
	mkdir -p build

build/objs:
	mkdir -p build/objs/

build/objs/%.o: %.c build/objs
	@echo "Compiling $<"
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

build/updater.bin: build/updater.elf build/updater.lst
	$(OCPY) $< $@ -O binary
	$(SZ) $<

build/updater.lst: build/updater.elf build
	$(ODUMP) -D $< > $@

build/updater.elf: $(OBJS)
	@echo "Linking $@"
	$(LD) $^ $(LDFLAGS) -o $@

clean:
	rm -rf build/objs build/updater.*