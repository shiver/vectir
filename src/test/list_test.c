#include <stdio.h>
#include "../util/common.h"

typedef struct {
	int id;
	List *items;
} Page;

typedef struct {
	int id;
	List *options;
} Item;

typedef struct {
	int id;
} Option;

void test_list(List *list) {
	Page *page;
	List *curr;

	printf("\nList\n");

	curr = list;
	while (curr) {
		page = (Page*)list_getCurrent(curr);
		if (page) {
			printf("%d\n", page->id);
		}
		curr = list_getNext(curr);
	}
	printf("\n");
}

int main() {
	Page *mainPage;
	Page *secondPage;
	Page *thirdPage;
	Item *firstItem;
	List *menuPages = NULL;
	List *tmpNode;
	List *pageItems;

	printf("Allocating pages...");
	mainPage = calloc(1, sizeof(Page));
	firstItem = calloc(1, sizeof(Item));
	secondPage = calloc(1, sizeof(Page));
	thirdPage = calloc(1, sizeof(Page));
	
	if (mainPage && secondPage) {
		printf("OK\n");
	}
	else {
		printf("FAIL!\n");
		return;
	}

	printf("Setting page values...");
	mainPage->id = 101;
	secondPage->id = 202;
	thirdPage->id = 3;
	printf("OK\n");

	printf("Setting item values...");
	firstItem->id = 1;
	printf("OK\n");

	printf("Adding items to pages...");
	thirdPage->items = list_add(thirdPage->items, firstItem);
	printf("OK\n");
	test_list(thirdPage->items);

	printf("Creating list root and adding pages...");
	menuPages = list_add(NULL, mainPage);
	tmpNode = list_add(menuPages, secondPage);
	list_add(menuPages, thirdPage);

	if (menuPages) {
		printf("OK\n");
	}

	printf("\n*Testing*\n");
	test_list(menuPages);

	printf("Removing first node...");
	menuPages = list_remove(menuPages);
	printf("OK\n");
	test_list(menuPages);
	printf("Re-appending removed node...");
	list_add(menuPages, mainPage);
	printf("OK\n");
	test_list(menuPages);

	printf("Showing page items...\n");
	printf("Page 3\n");
	test_list(thirdPage->items);

	// crap, but not the aim of the test
	if (mainPage || secondPage) {
		printf("Freeing pages...");
		free(mainPage);
		free(secondPage);
		printf("DONE\n");
	}
	return 0;
}

