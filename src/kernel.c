#include <common.h>
#include <multiboot.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <timer.h>
#include <keyboard.h>
#include <mm/frames.h>
#include <mm/paging.h>
#include <mm/heap.h>

/* Hardware text mode color constants. */
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
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c)
{
	if (c == 10)
	{
		terminal_column = 0;
	}

	else
	{
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH)
		{
			terminal_column = 0;
		}
	}

	if (++terminal_row == VGA_HEIGHT)
	{
		for (uint32_t y = 0; y < VGA_HEIGHT - 1; y++)
		{
			for (uint32_t x = 0; x <= VGA_WIDTH; x++)
			{
				terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y+1) * VGA_WIDTH + x];
			}
		}
		terminal_row--;
	}
}

void terminal_printhex(char key)
{
	/*terminal_putchar((char) ((0xF000 & num) >> 12) + 48);
	terminal_putchar((char) ((0x0F00 & num) >> 8) + 48);
	terminal_putchar((char) ((0x00F0 & num) >> 4) + 48);
	terminal_putchar((char) (0x000F & num) + 48);*/

	char value[2] = "00";
	char *hex = "0123456789ABCDEF";

	value[0] = hex[(key & 0xF0) >> 4];
	value[1] = hex[key & 0x0F];
	terminal_putchar(value[0]);
	terminal_putchar(value[1]);
}

void terminal_printhex16(uint16_t key)
{
	terminal_printhex((key & 0xFF00) >> 8);
	terminal_printhex(key & 0x00FF);
}

void terminal_printhex32(uint32_t key)
{
	terminal_printhex16((key & 0xFFFF0000) >> 16);
	terminal_printhex16(key & 0x0000FFFF);
}

void terminal_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}

uint32_t memupper_size;
extern uint32_t endkernel;

void kernel_main(multiboot_info_t* mbd, unsigned int magic)
{
	/* Initialize terminal interface */
	terminal_initialize();

	//Get contiguous memory size in kibibytes
	if (mbd->flags)
	{
		memupper_size = mbd->mem_upper;
	}

	terminal_writestring("INITIALIZING GDT");
	gdt_install();
	terminal_writestring("INITIALIZING IDT");
	idt_install();
	timer_install();
	keyboard_install();
	terminal_writestring("INITIALIZING IRQ");
	irq_install();
        set_typematic(0b01100000);
	terminal_writestring("INITIALIZING MEMORY MANAGER");
	init_paging(0x100000);//4 GB

	__asm__ __volatile__ ("sti");

	for(;;);
}
