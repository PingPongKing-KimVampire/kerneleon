IMG_NAME = image/myos.iso
BIN_NAME = kernel/myos.bin

C_SRCS = $(wildcard kernel/src/*.c)
AS_SRCS = $(wildcard kernel/src/*.s)
LD_SRC = ./kernel/linker.ld

C_OBJS = $(C_SRCS:.c=.o)
AS_OBJS = $(AS_SRCS:.s=.o)

AS = i686-elf-as
CC = i686-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector
LDFLAGS = -ffreestanding -O2 -nostdlib -lgcc

all : $(IMG_NAME)

boot : $(IMG_NAME)
	qemu-system-i386 -cdrom $(IMG_NAME)

bin : $(BIN_NAME)

$(IMG_NAME) : bin
	docker compose up --build

$(BIN_NAME) : $(C_OBJS) $(AS_OBJS)
	$(CC) -T $(LD_SRC) -o $(BIN_NAME) $(LDFLAGS) $(C_OBJS) $(AS_OBJS)

%.o : %.c
	$(CC) -c $^ -o $@ $(CFLAGS)

%.o : %.s
	$(AS) $^ -o $@

clean :
	rm -f $(C_OBJS) $(AS_OBJS)

fclean : clean
	rm -f $(BIN_NAME) $(IMG_NAME)