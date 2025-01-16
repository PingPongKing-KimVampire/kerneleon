#include <stdint.h>
#include "print42.h"

struct stackframe{
	struct stackframe* ebp;
	uint32_t eip;
};

void print_stack_trace_baby(){
	struct stackframe *stk;
	asm("movl %%ebp, %0" : "=r"(stk) ::);
	printk("Stack Trace: \n");
	int stackframe_number = 0;
	while(stk){
		printk("Frame #");
		printk_dec(stackframe_number);
		printk(" ");
		printk_hex(stk->eip);
		printk(" ");
		printk_hex((uint32_t)stk->ebp);
		printk("\n");
		stk = stk->ebp;
		stackframe_number ++;
	}
}