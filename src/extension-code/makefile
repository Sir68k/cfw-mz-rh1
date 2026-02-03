GCC_PATH = /Applications/ArmGNUToolchain/14.2.rel1/arm-none-eabi/bin

CC      = $(GCC_PATH)/arm-none-eabi-gcc
LD      = $(GCC_PATH)/arm-none-eabi-ld
OCPY    = $(GCC_PATH)/arm-none-eabi-objcopy
ODUMP   = $(GCC_PATH)/arm-none-eabi-objdump
SZ      = $(GCC_PATH)/arm-none-eabi-size
NM      = $(GCC_PATH)/arm-none-eabi-nm 
MKDIR   = mkdir

CFLAGS +=   -nostdlib \
			-ffreestanding \
			-march=armv4t \
			-mtune=arm7tdmi \
			-mthumb-interwork -mthumb\
			-ffunction-sections \
			-Os
			
LDFLAGS += -T cxd2687.ld
SRCS = init.c track.c menu.c netmd.c
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
