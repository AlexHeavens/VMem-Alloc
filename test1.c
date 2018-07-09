/*	Stuart Norcross - 12/03/10 */

/* Sample test file.
This program allocates integer arrays and displays trace information*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vmemalloc.h"
#include "vmemprint.h"

#define NUMBER_OF_ALLOCATIONS 100
#define INTS_PER_ALLOCATION 1000

int alloc_size = sizeof(int) * INTS_PER_ALLOCATION;

void check_failed(int val) {
	fprintf(stderr, "Check failed for region with value %i.", val);
	exit(-1);
}

void check(int *mem, int value) {
	int i;
	for (i = 0; i < INTS_PER_ALLOCATION; i++) {
		if (mem[i] != value) check_failed(value);
	}
}

void set(int *mem, int value) {
	int i;
	for (i = 0; i < INTS_PER_ALLOCATION; i++) {
		mem[i] = value;
	}
}

int *alloc_and_set(int value) {
	int *mem = (int *)vmemalloc(alloc_size);
	set(mem, value);
	return mem;
}

int main() {
	setupTimer();
	setTraceFile("experiment1.txt");

	int *allocated[NUMBER_OF_ALLOCATIONS];
	int i;
	printf("test1 starting\n");

	// do some allocation
	for (i = 0; i < NUMBER_OF_ALLOCATIONS; i++) {
		allocated[i] = alloc_and_set(i);
	}

	// check the allocations.
	for (i = 0; i < NUMBER_OF_ALLOCATIONS; i++) {
		check(allocated[i], i);
	}

	// free the first, 10th and 20th allocations.
	vmemfree(allocated[0]);
	vmemfree(allocated[9]);
	vmemfree(allocated[19]);

	// more allocations
	allocated[0] = alloc_and_set(9999999);
	allocated[9] = alloc_and_set(9999998);
	allocated[19] = alloc_and_set(9999997);

	printf("little free\n");

	// check the latest allocations
	check(allocated[0], 9999999);
	check(allocated[9], 9999998);
	check(allocated[19], 9999997);

	// check the rest of the allocations
	for (i = 0; i < NUMBER_OF_ALLOCATIONS; i++) {
		if (i != 0 && i != 9 && i != 19) check(allocated[i], i);
	}

	printf("Starting big free\n");

	// free a series of allocations
	for (i = 50; i < 99; i++) {
		vmemfree(allocated[i]);
	}

	// free a series of allocations
	for (i = 50; i < 99; i++) {
		allocated[i] = alloc_and_set(i);
	}

	// free a series of allocations
	for (i = 98; i >= 50; i--) {
		vmemfree(allocated[i]);
	}

	print_free_list();

	closeTraceFile();
	printf("test1 complete\n");
	return 0;
}
