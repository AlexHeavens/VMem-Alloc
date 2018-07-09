// vmemtests.c
// Created by 080004721, 2011.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vmemalloc.h"
#include "vmemprint.h"

int* allocAndSetInc(int startInc, int size);
int* allocAndSetSame(int value, int size);
void testBlockInc(int loc[], int startInc, int size);
void testBlockVal(int loc[], int val, int size);
void testAlloc(int expected, int* checkLoc);

int* allocAndSetInc(int startInc, int size) {
	int* newBlock = (int*)vmemalloc(size * sizeof(int));
	int i;
	for (i = 0; i < size; i++) {
		newBlock[i] = startInc + i;
	}
	return newBlock;
}

int* allocAndSetSame(int value, int size) {
	int* newBlock = (int*)vmemalloc(size * sizeof(int));
	int i;
	for (i = 0; i < size; i++) {
		newBlock[i] = value;
	}
	return newBlock;
}

void testBlockInc(int loc[], int startInc, int size) {
	int i;
	for (i = 0; i < size; i++) {
		testAlloc(startInc + i, &loc[i]);
	}
}

void testBlockVal(int loc[], int val, int size) {
	int i;
	for (i = 0; i < size; i++) {
		testAlloc(val, &loc[i]);
	}
}

void testAlloc(int expected, int* checkLoc) {
	if (*checkLoc != expected) {
		printf("Test location[%lu]: expected[%i] actual[%i]\n",
			   (unsigned long)checkLoc, expected, *checkLoc);
		printf("Test FAILED.\n");
		exit(-1);
	}
}
