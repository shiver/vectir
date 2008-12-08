#ifndef COMMON_H
#define COMMON_H

typedef struct _list {
	void *prev;
	void *next;
	void *data;
} List;

List *list_add(List *node, void *object);
List *list_remove(List *node);
List *list_getFirst(List *node);
List *list_getLast(List *node);
List *list_getNext(List *node);
List *list_getPrev(List *node);
void *list_getCurrent(List *node);

void error_print(const char*);
void debug_print(const char*);
unsigned int _random(unsigned int);

#endif

