SRCS += main.c
SRCS += vector.c

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS = -g -O0 -Wall
CFLAGS += -Tstm32f407.ld
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -nostartfiles
CFLAGS += -I. -ICMSIS/Include -ICMSIS/Device/ST/STM32F4xx/Include
CFLAGS += -DSTM32F40_41xxx

.PHONY: toggle

all: toggle

toggle: toggle.elf

toggle.elf: $(SRCS)
		$(CC) $(CFLAGS) $^ -o $@
		$(OBJCOPY) -O binary toggle.elf toggle.bin

clean:
	rm -f toggle.elf toggle.bin
