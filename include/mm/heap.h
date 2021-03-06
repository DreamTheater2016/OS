#ifndef __MM__HEAP_H
#define __MM__HEAP_H

#include <common.h>

uint32_t heap_start;
uint32_t heap_end;
uint32_t heap_size;

struct mem_block
{
	bool used;

	struct mem_block *prev;
	struct mem_block *next;

	uint32_t size;
};

struct mem_block *first;

void kheap_init(uint32_t start, uint32_t end);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif
