#include <mm/frames.h>

/*Initializes the page frame allocator
  Calculates number of frames
*/
void page_frame_init(uint32_t mem_size)
{
	nframes = mem_size/* - (((uint32_t)&endkernel)/0x1000)*/;

	memset(bitmap, 0, nframes/8);
	
	for (uint32_t i = 0; i <= ((uint32_t)&endkernel) / 0x1000; i++)
		alloc_frame_int();
}

uint32_t alloc_frame_int()
{
	uint32_t i = 0;
	while (bitmap[i/32] >> (i % 32) != 0)
	{
		i++;
		if (i == nframes)
		{
			kpanic("Ran out of page frames");
		}
	}

	bitmap[i/32] |= 1 << i;
	return /*(uint32_t)&endkernel + */(i * 0x1000);
}

void free_frame_int(uint32_t frame_adr)
{
	uint32_t i = (frame_adr / 0x1000)/* - ((uint32_t)&endkernel)/0x1000*/;

	if (i >= nframes)
		kpanic("Tried to free a non-existant frame");

	bitmap[i/32] &= 0 << i;
}
