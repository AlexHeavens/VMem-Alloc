// testSetB.c
// Created by 080004721, 2011.

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "vmemalloc.h"
#include "vmemtest.c"

void testStressAlloc();

int main() {
	setupTimer();
	struct stat st;
	if (!stat("./test data", &st)) mkdir("./test data", S_IRWXU);
	setTraceFile("test data/testSetBStats.txt");

	testStressAlloc();

	closeTraceFile();
	return 0;
}

void testStressAlloc() {
	printf("Stress allocation test STARTED.\n");

	int *testDataSet1[500];
	int *testDataSet2[1000];
	int i;

	printf("Testing many large allocations... ");
	// Try allocating many large blocks.
	for (i = 0; i < 500; i++) {
		testDataSet1[i] = allocAndSetInc(0, 10000);
	}

	// Check those blocks.
	for (i = 0; i < 500; i++) {
		testBlockInc(testDataSet1[i], 0, 10000);
	}
	printf("Test PASSED.\n");

	printf("Testing many small allocations... ");
	// Try allocating many small blocks.
	for (i = 0; i < 1000; i++) {
		testDataSet2[i] = allocAndSetInc(50, 32);
	}

	// Check those blocks.
	for (i = 0; i < 1000; i++) {
		testBlockInc(testDataSet2[i], 50, 32);
	}
	printf("Test PASSED.\n");

	printf("Testing many frees... ");
	// Check that freeing the first set doesn't affect the second.
	for (i = 0; i < 500; i++) {
		vmemfree(testDataSet1[i]);
	}
	for (i = 0; i < 1000; i++) {
		testBlockInc(testDataSet2[i], 50, 32);
	}
	printf("Test PASSED.\n");

	printf("Testing many simultaneous allocs and deallocs... ");
	// Test that repeated allocation and deallocation is possible.
	for (i = 0; i < 500; i++) {
		testDataSet1[i] = allocAndSetInc(0, 1000);
		testBlockInc(testDataSet1[i], 0, 1000);
		vmemfree(testDataSet1[i]);
	}

	// Test that this has had no negative effects.
	for (i = 0; i < 1000; i++) {
		testBlockInc(testDataSet2[i], 50, 32);
	}
	printf("Test PASSED.\n");

	printf("All stress tests PASSED.\n");
}
