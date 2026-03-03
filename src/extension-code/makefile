CROSS   ?= arm-none-eabi-
CC      = $(CROSS)gcc
LD      = $(CROSS)ld
OCPY    = $(CROSS)objcopy
ODUMP   = $(CROSS)objdump
SZ      = $(CROSS)size
NM      = $(CROSS)nm
MKDIR   = mkdir

CFLAGS +=   -nostdlib \
			-ffreestanding \
			-march=armv4t \
			-mtune=arm7tdmi \
			-mthumb-interwork -mthumb\
			-ffunction-sections \
			-fmerge-all-constants \
			-fno-builtin \
			-fno-jump-tables \
			-Os
			
LDFLAGS += -T extension.ld --print-memory-usage

SRCS = src/init.c \
       src/track.c \
	   src/track_config.c \
	   src/netmd_factory.c \
	   src/menu_resources.c \
	   src/cfw_config.c \
	   src/cfw_menu.c \
	   src/menu.c \
	   src/oled_config.c \
	   src/moved_data.c \
	   src/input.c

INCLUDES += ./

TARGET_NAME=extension

CFLAGS += $(foreach i,$(INCLUDES),-I$(i)) -DFW_${TARGET_VERSION}=1
OBJS = $(patsubst %.c,build/objs/%.o,$(SRCS))

all: build/app.bin
	wc -c build/app.bin
	cp build/app.bin build/${TARGET_NAME}.bin
	$(NM) build/app.elf > build/${TARGET_NAME}.sym

build:
	mkdir -p build

build/objs:
	mkdir -p build/objs/

build/objs/%.o: %.c build/objs
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

build/app.bin: build/app.elf build/app.lst
	$(OCPY) $< $@ -O binary
	$(SZ) $<

build/app.lst: build/app.elf build
	$(ODUMP) -D $< > $@

build/app.elf: $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@

clean:
	rm -rf build/objs build/app.* build/${TARGET_NAME}.bin build/${TARGET_NAME}.elf
