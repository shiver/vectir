#ifndef UI_H
#define UI_H

#include "SDL/SDL.h"
#include "../util/common.h"

#define MENU_MAIN						1
#define MENU_IN_GAME					2
#define MENU_OPTIONS					3
#define MENU_OPTIONS_GAME				4
#define MENU_OPTIONS_VIDEO				5
#define MENU_OPTIONS_AUDIO				6
#define MENU_OPTIONS_INPUT				7

#define MENU_MAX_DEPTH					10

// menu entry types
#define MENU_ENTRY_OPTION_TYPE			1
#define MENU_ENTRY_REDIRECT_TYPE		2
#define MENU_ENTRY_BAR_TYPE				3
#define MENU_ENTRY_PERCENT_TYPE			4
#define MENU_ENTRY_TEXT_TYPE			5
#define MENU_ENTRY_EVENT_DRIVEN_TYPE	6

typedef struct _menuOption {
	int id;
	char *text;
	void *parent;
} MenuOption;

typedef struct _menuEntry {
	int id;
	char *text;
	int type;
	int value;
	List *options;
	void *parent;
	int selected;
} MenuEntry;

typedef struct _menuPage {
	int id;
	List *entries;
	int num_entries;
	int selected;
	int parent_id;
} MenuPage;

/* options */
void ui_menuOptionSetText(void *option, char text[]);
char *ui_menuOptionGetText(void *option);

/* entries */
MenuEntry *ui_menuEntryNew();
MenuEntry *ui_menuEntryGetById(MenuPage *page, int id);
int ui_menuEntryGetId(MenuEntry *entry);
void ui_menuEntrySetText(MenuEntry *entry, char text[]);
char *ui_menuEntryGetText(MenuEntry *entry);
void ui_menuEntrySetType(MenuEntry *entry, int type);
int ui_menuEntryGetType(void *entry);
void ui_menuEntrySetValue(void *entry, int value);
int ui_menuEntryGetValue(void *entry);
void ui_menuEntrySetSelected(void *entry, int selected);
int ui_menuEntryGetSelected(void *entry);
void ui_menuEntryAddOption(void *entry, void *option);
void ui_menuEntryRemoveOption(void *entry, void *option);

/* pages */
MenuPage *ui_menuPageNew();
void ui_menuPageAdd(MenuPage *page);
void ui_menuPageRemove(MenuPage *page);
MenuPage *ui_menuPageGetCurrent();
int ui_menuPageGetParentId(MenuPage *page);
void ui_menuPageSetParentId(MenuPage *page, int id);
int ui_menuPageGetId(MenuPage *page);
void ui_menuPageSetCurrent(MenuPage *page);
void ui_menuPageSetCurrentById(int id);
MenuPage *ui_menuPageGetById(int id);
void ui_menuPageAddEntry(MenuPage *page, MenuEntry *entry);
void ui_menuPageRemoveEntry(MenuPage *page, MenuEntry *entry);
int ui_menuPageGetNumEntries(MenuPage *page);
List *ui_menuPageGetEntries(MenuPage *page);

int ui_menuPageGetSelected(MenuPage *page);
void ui_menuPageSetSelected(MenuPage *page, int id);
void ui_menuPageSelectNext(MenuPage *page);
void ui_menuPageSelectPrev(MenuPage *page);


void ui_menuInit();

/* misc */
int ui_isMenuVisible();
void ui_drawMainMenu(int);
int ui_handleEvents(SDL_Event *);
void ui_handleSelection();
void ui_handleValueIncrease();
void ui_handleValueDecrease();
void ui_toggleMenuVisible();

#endif

