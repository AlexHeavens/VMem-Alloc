#ifndef VMEM_ACCESS_HGUARD_hgfk4854
#define VMEM_ACCESS_HGUARD_hgfk4854
// vmemaccess.h
// Created by 080004721, 2011.

// Defines.
#define USED used_bm
#define FREE 0l
#define HEADER_SIZE (sizeof(unsigned long) * 1l)
#define MIN_BLOCK_SIZE \
	((sizeof(unsigned long) * 1l) + (sizeof(unsigned long*) * 2l))

// Global variable assumptions.
extern unsigned long used_bm;
extern unsigned long size_bm;
extern unsigned long* mem_start;
extern unsigned long* mem_end;
extern unsigned long* free_start;
extern unsigned long* free_end;

// Method Assumptions.
extern unsigned long* get_next(unsigned long*);
extern unsigned long* get_prev_free(unsigned long* block);
extern unsigned long* get_next_free(unsigned long* block);
extern unsigned long get_used(unsigned long* block);
extern unsigned long get_size(unsigned long* block);

#endif
