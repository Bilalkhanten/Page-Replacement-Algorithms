#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include <stdbool.h>
#include <string.h>

extern int memsize;

extern int debug;

extern struct frame *coremap;


//Linked list to keep track of the lru qeue
typedef struct _node {
  int frame; //frame number
  struct _node* next; // pointer to next node
} node;

node* lruHead; //To evict
node* lruTail; //To insert

//bitmap to kep track of recoding referenced frames
bool* lruReferencedFrames;


/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {

  //Error check 1: make sure that there is some frame to actually evict
  assert(lruHead != NULL);

  int frame = lruHead->frame;

  //Check if there is only one element in the list
  if(lruHead == lruTail){
    lruTail = NULL;
  }

  //Error check 2
  assert(lruReferencedFrames[frame] == 1);

  //assign frame as unreferenced
  lruReferencedFrames[frame] = 0;

  //Update the head of the lru list
  node* newLruHead = lruHead->next;

  //free the head
  free(lruHead);

  //reassign head
  lruHead = newLruHead;

	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {

  int frame = p->frame >> PAGE_SHIFT;

  //Check if not referenced
  if(lruReferencedFrames[frame] == 0){
    //Assign frame to be assigned
    lruReferencedFrames[frame] = 1;

    //create new node to store frame
    node* newNode = (node*)malloc(sizeof(node));
    newNode->frame = frame;
    newNode->next = NULL;

    //Attach newly created node to lru list

    //Case1: if empty list
    if(lruTail == NULL){
      lruTail = newNode;
      lruHead = lruTail;
    }

    //Case2: the list is non-empty
    else{
      lruTail->next = newNode;
      lruTail = newNode;
    }

  } else {

    //create new node to store frame
    node* newNode = (node*)malloc(sizeof(node));
    newNode->frame = frame;
    newNode->next = NULL;

    //attach new node to end of lru list.
    lruTail->next = newNode;
    lruTail = newNode;

    //get rid of last reference from the lru list
    node* p = lruHead;
    node* prev = NULL;
    while (p->frame != frame){
      prev = p;
      p = p->next;
    }

    if(prev != NULL){
      prev->next = p->next;
      free(p);
    }
    //delete lruHead
    else{
      lruHead = p->next;
      free(p);
      //list is wiped to be empty
      if(lruHead == NULL){
        lruTail = NULL;
      }
    }

  }

	return;
}


/* Initialize any data structures needed for this
 * replacement algorithm
 */
void lru_init() {
  //Initialize a empty linked list
  lruHead = NULL;
  lruTail = NULL;

  //allocate memeory to store refenece and set refenece to 0
  lruReferencedFrames = malloc(sizeof(bool) * memsize);
  memset(lruReferencedFrames, 0, sizeof(bool) * memsize);
}
