#ifndef COMMON_H
#define COMMON_H

#include <types.h>

typedef uint8_t bool;
#define true 1
#define false 0
#define NULL 0

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outl(uint16_t port, uint32_t value);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

void *memset(void *dest, char value, size_t count);
void *memcpy(void *dest, const void *src, size_t count);

/* This defines what the stack looks like after an ISR was running */
struct regs {
	unsigned int gs, fs, es, ds;      /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
	unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
}__attribute__((packed));

void kpanic(char* msg);

#endif
