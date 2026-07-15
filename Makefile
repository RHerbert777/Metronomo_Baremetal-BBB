CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

LIBGCC = $(shell arm-none-eabi-gcc -print-libgcc-file-name)

CFLAGS = -mcpu=cortex-a8 -O2 -g -Wall -Iinc

LDFLAGS = -T src/memmap.ld

SRCS = src/startup.s \
$(wildcard src/*.c)

OBJS = $(addprefix obj/, $(notdir $(SRCS:.c=.o)))
OBJS := $(OBJS:.s=.o)

TARGET = bin/app_baremetal

all: $(TARGET).bin

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.s
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBGCC)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f obj/*.o bin/*.dump bin/*.elf bin/*.bin

deploy: $(TARGET).bin
	cp $(TARGET).bin /srv/tftpboot/download.bin
	@echo "Arquivo enviado para o tftpboot"