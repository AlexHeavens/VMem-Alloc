// testSetC.c
// Created by 080004721, 2011.

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "vmemalloc.h"
#include "vmemtest.c"

void testBoundaryCaseAlloc();

int main(){
	setupTimer();
	struct stat st;
	if(!stat("./test data",&st))mkdir("./test data", S_IRWXU);
	setTraceFile("test data/testSetCStats.txt");

	testBoundaryCaseAlloc();
	
	closeTraceFile();
	return 0;
}

void testBoundaryCaseAlloc(){
	
	printf("Edge case tests STARTED.\n");
	
	int *testDataSet1[1000];
	int *testDataSet2[1000];
	int i;
	
	// Test when the allocation size is a page size.
	
	printf("Testing page size allocations... ");
	// Try allocating many large blocks.
	for(i = 0; i < 1000; i++){
		testDataSet1[i] = allocAndSetInc(0, sysconf(_SC_PAGESIZE));
	}
	
	// Check those blocks.
	for(i = 0; i < 1000; i++){
		testBlockInc(testDataSet1[i], 0, sysconf(_SC_PAGESIZE));
	}
	printf("Test PASSED.\n");
	
	printf("Testing small allocations... ");
	for(i = 0; i < 1000; i++){
		testDataSet2[i] = allocAndSetInc(0, 8);
	}
	
	// Check those blocks.
	for(i = 0; i < 1000; i++){
		testBlockInc(testDataSet2[i], 0, 8);
	}
	
	// Free the first block.
	for(i = 0; i < 1000; i++){
		vmemfree(testDataSet2[i]);
	}
	
	// Check that we haven't damaged the second block.
	for(i = 0; i < 1000; i++){
		testBlockInc(testDataSet1[i], 0, sysconf(_SC_PAGESIZE));
	}
	printf("Test PASSED.\n");
	
	printf("All edge case tests PASSED.\n");
}
