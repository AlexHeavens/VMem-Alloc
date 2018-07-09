// vmemalloc.c
// Created by 080004721, 2011. FileTrace skeleton provided by Stuart Norcross.

#include "vmemalloc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include "debug.h"
#include "vmemaccess.h"

unsigned long used_bm;
unsigned long size_bm;

signed short asc_addrs = 0;
unsigned long *last_mmap = NULL;
unsigned long *free_start = NULL;
unsigned long *free_end = NULL;

// Function Prototypes.
void header_stamp(unsigned long *start, unsigned long size, unsigned long used);
unsigned long get_used(unsigned long *block);
unsigned long get_size(unsigned long *block);
void set_used(unsigned long *block, unsigned long used);
void set_size(unsigned long *block, unsigned long size);
unsigned long *get_prev_free(unsigned long *block);
unsigned long *get_next_free(unsigned long *block);
void set_prev_free(unsigned long *block, unsigned long *newPrev);
void set_next_free(unsigned long *block, unsigned long *newNext);
void free_add(unsigned long *block);
void *find_free_block(unsigned long size);
void coalesce_forward(unsigned long *block);
void coalesce_backward(unsigned long *block);
void coalesce(unsigned long *firstBlock, unsigned long *secondBlock);
void coalesce_both(unsigned long *block);
unsigned long *allocate_mem(unsigned long size);
unsigned long largest_free_region();

/* Record usage statistics for trace data. */

// The total size (in bytes)  of all currently allocated regions.
static unsigned long currentAllocatedMemory = 0l;

// The total number of currently allocated regions.
static unsigned long currentAllocations = 0l;

// The total size in bytes) of all current free regions
static unsigned long totalFreeSpace = 0l;

// The current number of free regions
static unsigned long currentFreeRegions = 0l;

// The size (in bytes) of the largest free region
static unsigned long largestFreeRegion = 0l;

// The output stream to which trace data will be written
static FILE *traceFile = NULL;

// This is to identify the fist call to output the trace data (first call to
// vmemfree or vmemalloc). Before any trace data is written the collumn headers
// are written. Set to 0 when trace data is output.
static int firstCall = 1;

#define VMEMALLOC_OP "vmemalloc"
#define VMEMFREE_OP "vmefree  "

// The start time of the trace.
static long start_time;

// Current time in microseconds
long time_in_usecs() {
	struct timeval tim;
	gettimeofday(&tim, NULL);
	long current_time = tim.tv_sec * 1000000 + tim.tv_usec;
	return current_time;
}

// Execution time in micro-seconds.
long execution_time() {
	long current_time = time_in_usecs();
	return current_time - start_time;
}

void setupTimer() { start_time = time_in_usecs(); }

// Outputs the headers for the tab seperated trace data columns
void outputTraceColumnHeaders() {
	// This implmentation is dumb so it ignores some of the required trace data.
	fprintf(traceFile,
			"Function\tTime(us)\tAlloc Mem\tFree Mem\tNo. Alloc\tNo. "
			"Free\tLargest Free\n");
}

// Outputs tab seperated trace data
void outputTraceData(const char *op) {
	// Only output trace data if the trace file has been set.
	if (traceFile != NULL) {
		if (firstCall) {
			firstCall = 0;
			start_time = time_in_usecs();
			outputTraceColumnHeaders();
		}
		// This implementation is dumb so it ignores some of the required trace
		// data.
		fprintf(traceFile, "%s\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\n", op,
				execution_time(), currentAllocatedMemory, totalFreeSpace,
				currentAllocations, currentFreeRegions, largestFreeRegion);
	}
}

/**
Allocates space of at least the specified size from the heap.  Returns the
address of the newly allocated memory.

size - size of memory to allocate.
*/
void *vmemalloc(int size) {
	// We shouldn't attempt to allocate a negative or zero amount of memory.
	assert(size > 0);
	if (!last_mmap) {
		// Create the bitmasks we will be using.
		used_bm = (1l << (HEADER_SIZE * 8l - 1l));
		size_bm = (~(1l << (HEADER_SIZE * 8l - 1l)));

		debug_printf("First call of vmemalloc.\n");
		debug_printf("Using bit masks used_bm(%lu) and size_bm(%lu)", used_bm,
					 size_bm);
		debug_printf("Header size is %lu.", HEADER_SIZE);
	}

	// If there is not currently enough free space, mmap more.
	if (largestFreeRegion <
		(unsigned long)size + HEADER_SIZE + MIN_BLOCK_SIZE) {
		unsigned long *newBlock = allocate_mem(size);
		free_add(newBlock);
		coalesce_both(newBlock);
	}

	void *newRegion = find_free_block(size);
	outputTraceData(VMEMALLOC_OP);
	debug_printf("Have allocated %lu.", (unsigned long)newRegion);
	return newRegion + HEADER_SIZE;
}

/**
Adds a block to the free list, updating the statistics if it is going from used
to free.  This works on an address- ordered basis.

block - the location of the block to add to the free list.
*/
void free_add(unsigned long *block) {
	assert(block != NULL);
	if (block) {
		// If we are adding a used block to the free list, set it as free and
		// update the statistics.
		if (get_used(block)) {
			set_used(block, FREE);
			currentAllocatedMemory -= get_size(block);
			currentAllocations--;
			currentFreeRegions++;
			totalFreeSpace += get_size(block);
			if (get_size(block) > largestFreeRegion)
				largestFreeRegion = get_size(block);
		}

		// If the free list is empty, make the new block the only item.
		if (free_start == NULL) {
			set_next_free(block, NULL);
			set_prev_free(block, NULL);
			free_start = block;
			free_end = block;
			debug_printf("Added %lu to the start of the free list.",
						 (unsigned long)block);
			return;
		}

		// If the new free block is before the start of the free list, make it
		// the start.
		if (block < free_start) {
			set_prev_free(free_start, block);
			set_next_free(block, free_start);
			free_start = block;
			return;
		}

		// Otherwise, loop through the free list...
		unsigned long *current = get_next_free(free_start);
		while (current != NULL) {
			// If the next node has a greater address, insert the block into the
			// free list here.
			if (block < current) {
				set_next_free(get_prev_free(current), block);
				set_prev_free(block, get_prev_free(current));
				set_next_free(block, current);
				set_prev_free(current, block);
				return;
			}

			// Else, continue to the next item.
			current = get_next_free(current);
		}

		// If we get to the end of the free list, place the block here.
		set_next_free(free_end, block);
		set_prev_free(block, free_end);
		set_next_free(block, NULL);
		free_end = block;
	}
}

/**
Frees allocated memory, returning it to the heap.

ptr - pointer to the memory item to deallocate.
*/
void vmemfree(void *ptr) {
	assert(ptr != NULL);
	// The value of currentAllocatedMemory should be reduced here
	unsigned long *block = (ptr - HEADER_SIZE);
	debug_printf("Freeing %lu size %lu", (unsigned long)block, get_size(block));
	free_add(block);
	coalesce_both(block);
	outputTraceData(VMEMFREE_OP);
}

// Set the trace file. If path is NULL then trace is output to stdout.
void setTraceFile(char *path) {
	if (traceFile == NULL) {
		if (path != NULL) {
			if ((traceFile = fopen(path, "a+")) == NULL) {
				perror("Failed to open tracefile:");
				exit(EXIT_FAILURE);
			}
		} else {
			traceFile = stdout;
		}

	} else {
		fprintf(stderr, "Trace file cannot be set twice. Exiting.\n");
		exit(EXIT_FAILURE);
	}
}

void closeTraceFile() { fclose(traceFile); }

/**
This stamps a custom header into memory that will denote the start of a block.

start - the location in memory that the block should start.
size - the size that the block will have.
used - whether or not the block is used.
*/
void header_stamp(unsigned long *start, unsigned long size,
				  unsigned long used) {
	set_size(start, size);
	set_used(start, used);
	debug_printf("Block %lu now has size %lu.", (unsigned long)start,
				 get_size(start));
}

unsigned long get_used(unsigned long *block) { return ((*block) & used_bm); }

unsigned long get_size(unsigned long *block) { return ((*block) & size_bm); }

void set_used(unsigned long *block, unsigned long used) {
	char *isUsed = (used) ? "USED" : "FREE";
	debug_printf("Setting %lu as %s.", (unsigned long)block, isUsed);
	*block = (get_size(block) | used);
}

void set_size(unsigned long *block, unsigned long size) {
	debug_printf("Setting %lu as size %lu.", (unsigned long)block, size);
	*block = (get_used(block) | size);
}

// Free list manipulators.
unsigned long *get_prev_free(unsigned long *block) {
	if (block == free_start) return NULL;
	return *((unsigned long **)((void *)block + HEADER_SIZE));
}

unsigned long *get_next_free(unsigned long *block) {
	if (block == free_end) return NULL;
	return *((unsigned long **)((void *)block + HEADER_SIZE +
								sizeof(unsigned long *)));
}

void set_prev_free(unsigned long *block, unsigned long *newPrev) {
	assert(block != NULL);
	*((unsigned long **)((void *)block + HEADER_SIZE)) = newPrev;
	debug_printf("Setting prev free of %lu to %lu.", (unsigned long)block,
				 (unsigned long)newPrev);
}

void set_next_free(unsigned long *block, unsigned long *newNext) {
	assert(block != NULL);
	*((unsigned long **)((void *)block + HEADER_SIZE +
						 sizeof(unsigned long *))) = newNext;
	debug_printf("Setting next free of %lu to %lu.", (unsigned long)block,
				 (unsigned long)newNext);
}

/**
Finds a block in the free list that is large enough to store the requested size.
Splits the free block if it is larger. Works on a "first-fit" basis.

size - the size of the required space.
Returns a pointer to a block that is now allocated for that size.
*/
void *find_free_block(unsigned long size) {
	// Ensure that the allocated space is enough to contain free list pointers.
	unsigned long min = (size >= MIN_BLOCK_SIZE - HEADER_SIZE)
							? size
							: MIN_BLOCK_SIZE - HEADER_SIZE;

	// Loop through the free list.
	unsigned long *current = free_start;
	while (current != NULL) {
		// If we have space at this node, take it.
		if (get_size(current) >= min) {
			unsigned long *newEmptyBlock;
			unsigned long *newUsedBlock;

			// If there is not enough spare space to create a new free block,
			// take it all.
			if ((get_size(current) - min) < MIN_BLOCK_SIZE) {
				// Mark it as used.
				set_used(current, USED);

				// Remove it from the free list.
				if (current == free_start) {
					set_prev_free(get_next_free(current), NULL);
					free_start = get_next_free(current);
				} else if (current == free_end) {
					set_next_free(get_prev_free(current), NULL);
					free_end = get_prev_free(current);
				} else {
					set_next_free(get_prev_free(current),
								  get_next_free(current));
					set_prev_free(get_next_free(current),
								  get_prev_free(current));
				}

				// If we have taken (possibly) the largest free region, we need
				// to update the field.
				if (get_size(current) == largestFreeRegion) {
					largestFreeRegion = largest_free_region();
				}
				currentFreeRegions--;
				newUsedBlock = current;

			} else {
				// Otherwise, create a new free block to fill the extra space.
				unsigned long *newBlock =
					(asc_addrs) ? ((void *)current + min + HEADER_SIZE)
								: ((void *)current + get_size(current) - min);
				unsigned long oldSize = get_size(current);
				unsigned long newSize = (oldSize - min - HEADER_SIZE);

				// Depending on the architecture, it may be cause less
				// fragmentation to stamp from the bottom or top on a block.
				if (asc_addrs) {
					newEmptyBlock = newBlock;
					newUsedBlock = current;
				} else {
					newEmptyBlock = current;
					newUsedBlock = newBlock;
				}
				header_stamp(newUsedBlock, min, USED);
				header_stamp(newEmptyBlock, newSize, FREE);

				// Adjust the free list.
				if (current == free_start) {
					free_start = newEmptyBlock;
					set_prev_free(newEmptyBlock, NULL);
				} else {
					set_next_free(get_prev_free(current), newEmptyBlock);
					set_prev_free(newEmptyBlock, get_prev_free(current));
				}

				if (current == free_end) {
					set_next_free(newEmptyBlock, NULL);
					free_end = newEmptyBlock;
				} else {
					set_next_free(newEmptyBlock, get_next_free(current));
					set_prev_free(get_next_free(current), newEmptyBlock);
				}

				// If we have taken (possibly) the largest free region, we need
				// to update the field.
				if (oldSize == largestFreeRegion) {
					largestFreeRegion = largest_free_region();
				}
			}

			debug_printf("Returning new allocated block %lu, size %lu.",
						 (unsigned long)current, get_size(current));

			// Update our stats.
			currentAllocations++;
			totalFreeSpace -= get_size(newUsedBlock);
			currentAllocatedMemory += get_size(newUsedBlock);
			return newUsedBlock;
		}

		current = get_next_free(current);
	}

	// If we have reached the end of the free list, return NULL.
	return NULL;
}

/**
Loops through the free list to find size of the largest free region.
*/
unsigned long largest_free_region() {
	// Loop through the free blocks.
	unsigned long currentLargest = 0l;
	unsigned long *current = free_start;
	while (current != NULL) {
		// Make a note of the largest free we find.
		if (get_size(current) > currentLargest)
			currentLargest = get_size(current);
		current = get_next_free(current);
	}
	debug_printf("Returning new largest free region %lu.", currentLargest);
	return currentLargest;
}

/**
Coalesces a block forwards.  Works on the assumption that the next free block is
free.
*/
void coalesce_forward(unsigned long *block) {
	assert(!get_used(block));
	debug_printf("Coalescing %lu forward.", (unsigned long)block);
	coalesce(block, get_next_free(block));
}

/**
Coalesces a block backwards.  Works on the assumption that the previous free
block is free.
*/
void coalesce_backward(unsigned long *block) {
	assert(!get_used(block));
	debug_printf("Coalescing %lu backward.", (unsigned long)block);
	coalesce(get_prev_free(block), block);
}

/**
Coalesces two blocks.  Works on the assumption that both blocks are free.
*/
void coalesce(unsigned long *firstBlock, unsigned long *secondBlock) {
	assert(firstBlock < secondBlock);
	debug_printf("(Coalescing %lu and %lu)", (unsigned long)firstBlock,
				 (unsigned long)secondBlock);

	// Remove the second block from the free list.
	set_next_free(firstBlock, get_next_free(secondBlock));
	if (secondBlock != free_end) {
		set_prev_free(get_next_free(secondBlock), firstBlock);
	} else {
		free_end = firstBlock;
	}

	// Adjust the size of the first block.
	unsigned long newSize =
		get_size(secondBlock) + get_size(firstBlock) + HEADER_SIZE;
	set_size(firstBlock, newSize);

	// Update the stats.
	totalFreeSpace += HEADER_SIZE;
	currentFreeRegions--;
	if (get_size(firstBlock) > largestFreeRegion)
		largestFreeRegion = get_size(firstBlock);
}

/**
Given a block, this will check if the previous and latter blocks can be
coalesced, which it will do.
*/
void coalesce_both(unsigned long *block) {
	assert(!get_used(block));

	// If we aren't at the start of the memory space..
	unsigned long *prevBlock = get_prev_free(block);
	if (prevBlock) {
		// ..and the previous block is free, coalesce the previous block.
		if (!get_used(prevBlock) &&
			((void *)prevBlock + get_size(prevBlock) + HEADER_SIZE) == block) {
			// Make sure to update local block pointers in case we also coalesce
			// forward.
			coalesce_backward(block);
			block = prevBlock;
			prevBlock = get_prev_free(block);
		}
	}

	// If the next block isn't the end of the memory space and it is free,
	// coalesce it.
	unsigned long *nextBlock = get_next_free(block);
	if (nextBlock) {
		if (!get_used(nextBlock) &&
			((void *)block + get_size(block) + HEADER_SIZE) == nextBlock)
			coalesce_forward(block);
	}
}

/**
MMaps a multiple of the page size that can contain the requested size.
*/
unsigned long *allocate_mem(unsigned long size) {
	// We need to keep enough space for a header.
	unsigned long totalSize = size + HEADER_SIZE;

	// Find a multiple of the system page size that will fit our request.
	unsigned long remainder = totalSize % sysconf(_SC_PAGESIZE);
	unsigned long numPages = totalSize / sysconf(_SC_PAGESIZE);
	if (remainder) {
		numPages++;
	}

	// mmap the space we need.
	unsigned long *newBlock = mmap(last_mmap, numPages * sysconf(_SC_PAGESIZE),
								   PROT_EXEC | PROT_READ | PROT_WRITE,
								   MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	debug_printf("Have mmapped size %lu at location %lu.",
				 numPages * sysconf(_SC_PAGESIZE), (unsigned long)newBlock);
	asc_addrs = (newBlock > last_mmap);
	last_mmap = newBlock;

	// Stamp it as free and add to the free list.
	header_stamp(newBlock, numPages * sysconf(_SC_PAGESIZE) - HEADER_SIZE,
				 FREE);

	// Update stats.
	currentFreeRegions++;
	totalFreeSpace += get_size(newBlock);
	if (get_size(newBlock) > largestFreeRegion)
		largestFreeRegion = get_size(newBlock);

	return newBlock;
}
