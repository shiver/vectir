#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// used to store the previous time to get things more random
time_t prev_time;
int inc = 0;

void error_print(const char *text) {
	fprintf(stderr, "ERROR: %s\n", text);
	fflush(stderr);
}

void debug_print(const char *text) {
	fprintf(stdout, "%s\n", text);
	fflush(stdout);
}

// returns a random number
unsigned int _random(unsigned int max) {
  unsigned int result;
  time_t cur_time;

  cur_time = time(NULL);

  prev_time = cur_time;

  srand(cur_time + inc);
  result = rand() % max;

  inc++;
  return result;
}

/* list */

// add an object to a list or start a new list
// returns the new node
List *list_add(List *node, void *object) {
	List *list = NULL;
	List *curr_node = NULL;

	// cant add nothing
	if (!object) {
		return NULL;
	}

	// allocate space for new list object
	list = calloc(1, sizeof(List));
	// return a NULL if it fails
	if (!list) {
		return NULL;
	}
	
	// assign new object to data 
	list->data = object;

	// if we're appending to the list...
	if (node != NULL) {
		// check if the next space is open
		if (node->next == NULL) {
			list->prev = node;
			node->next = list;
		}
		// if it isn't
		else { 
			curr_node = node->next;
			// cycle until we find the end
			while (curr_node->next != NULL) {
				curr_node = curr_node->next;
			}

			// apply list to the end
			curr_node->next = list;
			list->prev = curr_node;
		}
	}
	
	return list;
}

// remove a node from the list
// returns the next node in the list
List *list_remove(List *node) {
	List *prev = node->prev;
	List *next = node->next;

	// cant remove nothing
	if (!node) {
		return NULL;
	}

	/* link up nodes that were on either side of this */
	if (prev) {
		prev->next = next;
	}

	if (next) {
		next->prev = prev;
	}

	// give back some mem
	free(node);

	if (next) { // return what used to fall after removed node (default)
		return next;
	}
	else if (prev) { // return what used to fall before removed node
		return prev;
	}
	else { // list is now blank
		return NULL;
	}
}

// retrieve the root of the list using any node
List *list_getFirst(List *node) {
	List *curr = NULL;
	
	// dont be silly
	if (!node) {
		return NULL;
	}

	// check if node is the root
	if (node->prev == NULL) {
		return node;
	}

	curr = node->prev;
	// cycle through the nodes backwards until prev is a NULL
	while (curr->prev) {
		curr = curr->prev;
	}

	return curr;
}

// retrieve the last node in the list using any node as a start
List *list_getLast(List *node) {
	List *curr = NULL;

	// DOES NOT COMPUTE!!
	if (!node) {
		return NULL;
	}

	// is this the last?
	if (node->next == NULL) {
		return node;
	}

	// cycle through all nodes until we hit the end
	curr = node->next;
	while (curr->next) {
		curr = curr->next;
	}

	return curr;
}

List *list_getNext(List *node) {
	if (!node) {
		return NULL;
	}

	return node->next;
}

List *list_getPrev(List *node) {
	if (!node) {
		return NULL;
	}

	return node->prev;
}

void *list_getCurrent(List *node) {
	if (!node) {
		return NULL;
	}

	return node->data;
}

