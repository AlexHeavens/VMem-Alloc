/*	Stuart Norcross - 12/03/10 */

//An interface for a heap alocation library

/*	Allocate 'size' bytes of memory. On success the function returns a pointer to 
	the start of the allocated region. On failure NULL is returned. */
extern void *vmemalloc(int size);

/*	Release the region of memory pointed to by 'ptr'. */
extern void vmemfree(void *ptr);

/*	Set the file specified by the 'file' parameter as the target for trace data. 
	If 'file' does not exist it will be created. 
	If this function is not called then no trace output should be generated.*/
extern void setTraceFile(char *file);

/*	Initialise the timing mechanism. */
extern void setupTimer(void);

extern void closeTraceFile();
