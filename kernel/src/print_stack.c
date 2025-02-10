#include <stdint.h>
#include "print42.h"

void print_esp() {
    uint32_t esp;

    asm volatile (
        "mov %%esp, %0" : "=r"(esp) ::
    );
	printk("ESP: ");
	printk_hex(esp);
	printk("\n");
}

void print_stack_line(uint32_t pos, int print_length) {
	printk_hex(pos);
	printk(": ");
	for (int i = 0; i < print_length; i++)
	{
		printk_byte_by_hex(*(uint8_t *)(pos + i));
		printk(" ");
	}
	printk(" : ");
	
	char buffer[print_length+1];
	buffer[print_length] = 0;
	for (int i = 0; i < print_length; i++)
	{
		buffer[i] = '.';
		int value = *(uint8_t *)(pos + i);
		if(value >= 33 && value <= 126)
			buffer[i] = value;
	}
	printk(buffer);
	printk("\n");
}

void print_frame(int frame_number, uint32_t *ebp){
	printk("Frame #");
	printk_dec(frame_number);
	printk(" ");
	printk_hex((uint32_t)ebp);
	printk("\n");
}

uint32_t print_stack_recursive(uint32_t *ebp, int frame_number, int print_length, uint32_t esp) {
	if (!*ebp)
	{
		print_frame(frame_number, ebp);
		return (uint32_t)ebp;
	}

	uint32_t pos = print_stack_recursive((uint32_t *)*ebp, frame_number+1, print_length, esp);

	uint32_t end = (uint32_t)ebp;
	if (frame_number == 0)
		end = esp;
	while (end <= (uint32_t)pos)
	{
		print_stack_line(pos, print_length);
		pos -= print_length;
	}

	print_frame(frame_number, ebp);
	

	return pos;
}

void print_stack2(char ch, char ch2){
	uint32_t *ebp;
	uint32_t esp;
	char x = 'x';
	asm("movl %%ebp, %0" : "=r"(ebp) ::);
	asm("movl %%esp, %0" : "=r"(esp) ::);
	int print_length = 16;

	print_stack_recursive(ebp, 0, print_length, esp);
	print_esp();
}

void print_stack(){
	char q = 'Q';
	char z = 'Z';
	char anyany[4];
	anyany[0] = '1';
	anyany[1] = 'a';
	anyany[2] = '~';
	anyany[3] = '\0';
	print_stack2(q, z);
}
