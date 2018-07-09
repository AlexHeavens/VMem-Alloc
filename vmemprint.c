// vmemprint.c
// Created by 080004721, 2011.

#include <assert.h>
#include <stdio.h>
#include "vmemaccess.h"

// Function prototypes.
void print_mem();
void print_block(unsigned long* block);
void print_free_list();
void print_free_block(unsigned long* block);
void print_used_block(unsigned long* block);

void print_block(unsigned long* block) {
	// Print the header of the block.
	char* isUsed = (get_used(block)) ? "USED" : "FREE";
	printf("pos abs(%lu) header \t- value %lu - %s - size(words) %lu\n",
		   (unsigned long)block, (unsigned long)*block, isUsed,
		   get_size(block) / sizeof(void*));

	// Print it depending on the type.
	if (get_used(block))
		print_used_block(block);
	else
		print_free_block(block);
}

void print_free_list() {
	unsigned long* current = free_start;
	unsigned long pos = 0l;
	printf("free list\n");
	while (current) {
		printf("free block(%lu) - pos abs(%lu) - size(words) %lu\n\t", pos,
			   (unsigned long)current, get_size(current));
		print_free_block(current);
		current = get_next_free(current);
		pos++;
	}
}

void print_free_block(unsigned long* block) {
	assert(!get_used(block));

	printf("previous ");
	if (!get_prev_free(block))
		printf("(none) - ");
	else
		printf("(%lu) - ", (unsigned long)get_prev_free(block));
	printf("next ");
	if (!get_next_free(block))
		printf("(none)\n");
	else
		printf("(%lu)\n", (unsigned long)get_next_free(block));
}

void print_used_block(unsigned long* block) {
	// For all following words, print their contents.
	unsigned long pos = 1l;
	while (pos < ((get_size(block) + HEADER_SIZE) / sizeof(unsigned long))) {
		// Skip printing the middle of the block.
		if (pos > 5l && (pos < get_size(block) / sizeof(unsigned long) - 5l) &&
			get_size(block) / sizeof(unsigned long) > 10l) {
			pos = get_size(block) / sizeof(unsigned long) - 5l;
			printf("... %lu words\n",
				   (get_size(block) / sizeof(unsigned long) - 10l));
		} else {
			printf("pos abs(%lu) rel(%lu) \t- value %lu\n",
				   (unsigned long)(void*)(block + pos), pos, *(block + pos));
		}
		pos++;
	}
}
