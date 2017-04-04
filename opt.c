#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"


extern unsigned memsize;

extern int debug;

extern struct frame *coremap;

FILE *infp = NULL;
int counter = 0;

/* virtual address linked list */
struct vaddr_node {
        addr_t vaddr;
        struct vaddr_node *next;
	int reference;
};

struct vaddr_node *front = NULL;
struct vaddr_node *tail = NULL;

/* SUMMARY: STORE EVERY VIRTUAL ADDRESS IN A LINKED LIST, CONTAINING ITS REFERENCE LINE */

/* insert virtual address with reference point */
void insertAddress(addr_t vaddr, int reference)
{
	struct vaddr_node *pointer = tail;

        /* Iterate through the list till we encounter the last node.*/
	if(pointer == NULL){
		pointer = (struct vaddr_node *)malloc(sizeof(struct vaddr_node));
		pointer->vaddr = vaddr;
		pointer->next = NULL;
		pointer->reference = reference;
		front = pointer;
		tail = pointer;
		return;
	}

        /* Allocate memory for the new node and put data in it.*/
        pointer->next = (struct vaddr_node *)malloc(sizeof(struct vaddr_node));
        pointer = pointer->next;
        pointer->vaddr = vaddr;
        pointer->next = NULL;
	pointer->reference = reference;
	tail = pointer;
}

/* debug print function */
void print_vaddr(struct vaddr_node *start){

	struct vaddr_node *pointer = start;
        while(pointer != NULL){
                printf("%lx on line %d\n", pointer->vaddr, pointer->reference);
		pointer = pointer->next;
        }
}

/* returns the next reference point of virtual address */
int getNextReference(addr_t vaddr){

	struct vaddr_node *pointer = front;
	while(pointer != NULL){
		if(pointer->vaddr == vaddr){
			return pointer->reference;
		}
		pointer = pointer->next;
	}
        // means that it is never accessed again, so return next reference as beyond length of trace
	return counter+1;
	
}
/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int i; 
	int temp = 0; // next reference checker
	int furthestReference = 0; // furthest reference point
	int frame = 0; // frame that belongs to the furthest reference
														
	//for every page currently in memory (i.e. allocated on a frame in coremap), find the furthest reference point 
	for(i = 0; i < memsize; i++){
		if((temp = coremap[i].pte->nextReference) > furthestReference){ 
			furthestReference = temp;
			frame = i;
		}	 
	}	
	return frame;
	
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
/* increments linked list to p's virtual address' next reference point */
void opt_ref(pgtbl_entry_t *p) {
	front = front->next;
	p->nextReference = getNextReference(p->vaddr);
	return; 
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	/* reads trace file */
	if(tracefile != NULL){
		if((infp = fopen(tracefile, "r")) == NULL) {
			perror("Error opening tracefile\n");
			exit(1);
		}
	}
	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;

	while(fgets(buf, MAXLINE, infp) != NULL) {
		if(buf[0] != '='){
			sscanf(buf, "%c %lx", &type, &vaddr);
			/* insert reference point length to vaddr's reference points */
			insertAddress(vaddr, counter);
			counter++;
		} else {
			continue;
		}
	}
}
