#include <common.h>

void outb(uint16_t port, uint8_t value){
	asm volatile ("outb %1, %0" : : "d" (port), "a" (value));
}

void outw(uint16_t port, uint16_t value){
	asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
}

void outl(uint16_t port, uint32_t value){
	asm volatile ("outl %1, %0" : : "dN" (port), "a" (value));
}

uint8_t inb(uint16_t port){
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

uint16_t inw(uint16_t port){
	uint16_t ret;
	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

uint32_t inl(uint16_t port){
	uint32_t ret;
	asm volatile ("inl %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

void *memset(void *dest, char value, size_t count)
{
	char *tmp = (char*)dest;
	for (count; count != 0; count--)
		*tmp++ = value;

	return dest;
}


void *memcpy(void *dest, const void *src, size_t count)
{
	char *dest_tmp = (char*)dest;
	const char *src_tmp = (const char*)src;

	for (count; count != 0; count--)
		*dest_tmp++ = *src_tmp++;
	return dest;
}

void kpanic(char *msg)
{
	terminal_writestring("KERNEL PANIC: ");
	terminal_writestring(msg);
	__asm__ __volatile__ ("cli");
	for(;;);
}
