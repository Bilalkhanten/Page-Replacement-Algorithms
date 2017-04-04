#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;


int head;

/* Page to evict is chosen using the fifo algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int fifo_evict() {

//check if the head is the same size as the memsize
   if(head == memsize){
//reset the head
		 head = 0;
	 }

//move to the next frame
	return head++;
}

/* This function is called on each access to a page to update any information
 * needed by the fifo algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void fifo_ref(pgtbl_entry_t *p) {
       //Not needed
	return;
}

/* Initialize any data structures needed for this
 * replacement algorithm
 */
void fifo_init() {
	// initial frame is the first page
	head = 0;
}
