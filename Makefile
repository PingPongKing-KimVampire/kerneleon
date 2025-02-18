IMG_NAME = image/myos.iso
BIN_NAME = kernel/myos.bin

C_SRCS = $(wildcard kernel/src/*.c)
AS_SRCS = $(wildcard kernel/src/*.s)
LD_SRC = ./kernel/linker.ld

C_OBJS = $(C_SRCS:.c=.o)
AS_OBJS = $(AS_SRCS:.s=.o)

AS = i386-elf-as
CC = i386-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -O0 -fno-omit-frame-pointer -fno-inline -Wall -Wextra -fno-stack-protector -mpreferred-stack-boundary=2 
LDFLAGS = -ffreestanding -O0 -nostdlib -lgcc

all : $(IMG_NAME)

boot : $(IMG_NAME)
	qemu-system-i386 -m 4096M -cdrom $(IMG_NAME)

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