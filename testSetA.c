// testSetA.c
// Created by 080004721, 2011.

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "vmemalloc.h"
#include "vmemtest.c"

void testStandardAlloc();

int main() {
	setupTimer();
	struct stat st;
	if (!stat("./test data", &st)) mkdir("./test data", S_IRWXU);
	setTraceFile("test data/testSetAStats.txt");

	testStandardAlloc();

	closeTraceFile();
	return 0;
}

void testStandardAlloc() {
	printf("Standard allocation test STARTED.\n");

	// Try a single set of allocations and frees of the same data.
	int* testData1;
	testData1 = allocAndSetSame(5000, 1000);
	testBlockVal(testData1, 5000, 1000);
	vmemfree(testData1);

	// Try a single set of allocations and frees of incrementing data.
	int* testData2 = allocAndSetInc(5000, 1000);
	testBlockInc(testData2, 5000, 1000);
	vmemfree(testData2);

	// Try freeing first allocation, then check the second.
	testData1 = allocAndSetSame(10000, 1000);
	testData2 = allocAndSetInc(5000, 1000);
	testBlockVal(testData1, 10000, 1000);
	testBlockInc(testData2, 5000, 1000);
	vmemfree(testData1);
	testBlockInc(testData2, 5000, 1000);
	vmemfree(testData2);

	// Now, vice versa.
	testData1 = allocAndSetSame(10000, 1000);
	testData2 = allocAndSetInc(10000, 1000);
	testBlockVal(testData1, 10000, 1000);
	testBlockInc(testData2, 10000, 1000);
	vmemfree(testData2);
	testBlockVal(testData1, 10000, 1000);
	vmemfree(testData1);

	printf("Test PASSED.\n");
}
