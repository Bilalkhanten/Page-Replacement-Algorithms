#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

// circular singly linked list

struct node {
        unsigned int frame;
        struct node *next;
};
struct node *start;

void insert(struct node *pointer, int frame)
{
        struct node *s = pointer;
	
       
	if(pointer == NULL){
		pointer = (struct node *)malloc(sizeof(struct node));
		pointer->frame = frame;
		pointer->next = pointer;
		start = pointer;
		return;
	}
	 /* Iterate through the list till we encounter the last node.*/
        while(pointer->next != s)
        {
		 pointer = pointer->next;
        }
        /* Allocate memory for the new node and put data in it.*/
        pointer->next = (struct node *)malloc(sizeof(struct node));
        pointer = pointer->next;
        pointer->frame = frame;
        pointer->next = s; // set next to head, making it circular
}
/* debug print */
void print(struct node *start)

{	struct node *pointer = start;
        while(start->next != pointer)
        {
                printf("%d\n", start->frame);
		start = start->next;
        }
	printf("%d\n", start->frame);
}

int clock_evict() {
	
	while(coremap[start->frame].pte->frame & PG_REF){ // while ref bit is 1
		assert(coremap[start->frame].pte->frame & PG_REF); 
		
		coremap[start->frame].pte->frame &= ~ PG_REF; // set ref bit to 0
		start = start->next;
	}
	// check if ref bit is truly 0
	assert(!(coremap[start->frame].pte->frame & PG_REF));
	return start->frame;
	
	
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init(){
	// initialize circular linked list
	int i;
	for(i = 0; i < memsize; i++){
		insert(start, i);
	}
}

