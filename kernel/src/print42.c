#include "strlen.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error                                                                         \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;
static bool is_init = false;

void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t *)0xB8000;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_setcolor(uint8_t color) { terminal_color = color; }

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_row++;
    terminal_column = 0;
    return;
  }
  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  if (++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT)
      terminal_row = 0;
  }
}

void terminal_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    terminal_putchar(data[i]);
  }
}

void printk(const char *data) {
  if (!is_init) {
    terminal_initialize();
    is_init = true;
  }
  terminal_write(data, strlen(data));
}

void printk_hex(uint32_t value){
	char *hex_string = "0123456789ABCDEF";
	char hex_result[8+2+1];
	hex_result[10] = '\0';
	hex_result[0] = '0';
	hex_result[1] = 'x';
	for(int i = 2; i < 10; i++)
		hex_result[i] = '0';
	int current_index = 9;
	while(value){
		hex_result[current_index] = hex_string[value % 16];
		value /=  16;
		current_index--;
	}
	printk(hex_result);
}

static void printk_dec_recursive(uint32_t value){
	if(value == 0)
		return;
	printk_dec_recursive(value / 10);
	char dec_result[2];
	dec_result[1] = '\0';
	dec_result[0] = value % 10 + '0';
	printk(dec_result);
}

void printk_dec(uint32_t value){
	if(value == 0){
		printk("0");
		return;
	}
	printk_dec_recursive(value);
}