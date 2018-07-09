#ifndef VMEM_PRINT_HGUARD_kfhk8469
#define VMEM_PRINT_HGUARD_kfhk8469

// Function prototypes;

/**
print_block() prints a block, the format of which is dependent on if the block
is used or not.
*/
void print_block(unsigned long* block);

/**
print_free_list() prints the free list in a pretty-print format.
*/
extern void print_free_list();

#endif
