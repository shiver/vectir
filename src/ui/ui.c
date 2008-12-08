#include "ui.h"

#include "../util/common.h"
#include "../graphics/graphics.h"
#include "../graphics/font.h"
#include "../input/input.h"
#include "../game/game.h"
#include "../game/grid.h"

#include "SDL/SDL_opengl.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

// globals
static int _menuVisible = 0;
static List *_menuPages = NULL;
static MenuPage *_menuCurrentPage = NULL;
static unsigned int _numMenuPages = 0;

// handle menu events
// basically it will only accept key down events and pass
// the rest back to the main message pump
int ui_handleEvents(SDL_Event *event) {
	MenuPage *page = NULL;

	switch(event->type) {
		// not a very modular way of handling input... might revisit
		case SDL_KEYDOWN:
			switch (event->key.keysym.sym) {
				case SDLK_UP:
					ui_menuPageSelectPrev(ui_menuPageGetCurrent());
					return 0;
					break;
				case SDLK_DOWN:	
					ui_menuPageSelectNext(ui_menuPageGetCurrent());
					return 0;
					break;
				case SDLK_ESCAPE:
					// are we escaping out of the menu
					if (ui_menuPageGetId(ui_menuPageGetCurrent()) == MENU_MAIN || 
							ui_menuPageGetId(ui_menuPageGetCurrent()) == MENU_IN_GAME) 
					{
						SDL_SetEventFilter(NULL); // turn off filtering
						// set the message back to the main pump
						// for handling
						return 1;				
					}
					// otherwise go back a page 
					else {
						// remember where we were
						page = ui_menuPageGetCurrent();
						
						// now change to the parent page
						ui_menuPageSetCurrentById(
							ui_menuPageGetParentId(ui_menuPageGetCurrent()));

						// now make the page we just came from forget about its parent
						ui_menuPageSetParentId(page, 0);
					}
					break;
				case SDLK_RETURN:
					ui_handleSelection();
					return 0;
					break;
				case SDLK_LEFT:
					ui_handleValueDecrease();
					return 0;
					break;
				case SDLK_RIGHT:
					ui_handleValueIncrease();
					return 0;
					break;
				default:
					break;
			}
			return 0;
			break;
		default:
			return 1;
			break;
	}
}

// perform actions required when a menu option is activated
void ui_handleSelection() {
	SDL_Event event;
	MenuPage *page = NULL;

	page = ui_menuPageGetCurrent();
	switch (ui_menuPageGetId(page)) {
		case MENU_MAIN: // Main menu
			switch (ui_menuPageGetSelected(page)) {
				case 1: // start game
					SDL_SetEventFilter(NULL);
					event.type = GAME_START_NEW;
					SDL_PushEvent(&event);
					break;
				case 2: // high scores
					break;
				case 3: // options
					ui_menuPageSetCurrentById(MENU_OPTIONS);
					break;
				case 4: // quit
					event.type = GAME_QUIT;
					SDL_PushEvent(&event);
					break;
				default:
					break;
			}
			break;
		case MENU_IN_GAME: // In-game menu
			switch (ui_menuPageGetSelected(page)) {
				case 1: // resume
					SDL_SetEventFilter(NULL);
					event.type = ESCAPE_KEY; // let the main loop know we're out of the menu
					SDL_PushEvent(&event);
					break;
				case 2: // options
					ui_menuPageSetCurrentById(MENU_OPTIONS);
					break;
				case 3: // End game
					event.type = GAME_END;
					SDL_PushEvent(&event);
					ui_menuPageSetCurrentById(MENU_MAIN);
					break;
				default:
					break;
			}
			break;
		case MENU_OPTIONS: // Options menu
			switch (ui_menuPageGetSelected(page)) {
				case 1: // game
					ui_menuPageSetCurrentById(MENU_OPTIONS_GAME);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void ui_handleValueDecrease() {
}

void ui_handleValueIncrease() {
}

// returns 1 if menu is visible, otherwise 0
int ui_isMenuVisible() {
	return _menuVisible;
}

// turns the menu on or off
void ui_toggleMenuVisible() {
	_menuVisible = !_menuVisible;
}

// draw the actual menu to screen. should be called by the main game renderer
void ui_drawMainMenu(int type) {
	SDL_Color red;
	MenuPage *page = NULL;
	List *entries = NULL;
	MenuEntry *entry = NULL;
	int count = 0;

	// set the value for the selection color
	red.r = 255;
	red.g = 0;
	red.b = 0;

	// this does the full screen image thing
	// ie: before the game has started
	if (type == MENU_MAIN) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		graphics_drawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glDisable(GL_BLEND);

		// get the Main Menu page
		page = ui_menuPageGetCurrent();
		// get a list to the entries for the page
		entries = ui_menuPageGetEntries(page);

		// draw the current page
		while (entries) { 
			entry = (MenuEntry *)list_getCurrent(entries);
			if (ui_menuEntryGetId(entry) != ui_menuPageGetSelected(page)) {
				font_print(10, 
					SCREEN_HEIGHT - (SCREEN_HEIGHT * (0.25f - (0.05 * count))), 
					ui_menuEntryGetText(entry), 
					NULL);
			}
			else { // else, draw in selected color 
				font_print(10, 
					SCREEN_HEIGHT - (SCREEN_HEIGHT * (0.25f - (0.05f * count))), 
					ui_menuEntryGetText(entry), 
					&red);
			}
			
			// go to next entry in the list
			entries = list_getNext(entries);
			count++;
		}
	}
	// placed over the playing area in the center
	// ie: in-game menu
	else if (type == MENU_IN_GAME) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		
		graphics_drawBox(GRID_BLOCK_SIZE, ((GRID_BLOCK_SIZE * GRID_HEIGHT) / 3), 
			GRID_BLOCK_SIZE * GRID_WIDTH, 100);
		glDisable(GL_BLEND);

		// get the Main Menu page
		page = ui_menuPageGetCurrent();
		// get a list to the entries for the page
		entries = ui_menuPageGetEntries(page);

		// draw the current page 
		while (entries) {
			entry = (MenuEntry *)list_getCurrent(entries);
			if (ui_menuEntryGetId(entry) != ui_menuPageGetSelected(page)) {
				font_print(GRID_BLOCK_SIZE + 15, 
					((GRID_HEIGHT * GRID_BLOCK_SIZE) / 3) + 10 + (20 * count), 
					ui_menuEntryGetText(entry),	NULL);
			}
			else {
				font_print(GRID_BLOCK_SIZE + 15, 
					((GRID_HEIGHT * GRID_BLOCK_SIZE) / 3) + 10 + (20 * count),
					ui_menuEntryGetText(entry),	&red);
			}

			entries = list_getNext(entries);
			count++;
		}
	}
}

// sets the current page of the menu
/*void ui_setMenuPage(int page) {
	// remember where we're coming from
	ui_setPreviousMenuPage(ui_getMenuPage());

	printf("Setting menu page to %d\n", page);
	// set where we are going to
	_currentPage = page;
}

// returns the current page of the menu
int ui_getMenuPage() {
	return _currentPage;
}

// append to the previous page list
void ui_setPreviousMenuPage(int page) {
	int i;

	for (i = 0; i < MENU_MAX_DEPTH; i++) {
		// cycle through all the pages until we find one that isn't used
		if (previousPages[i] == -1) {
			printf("Previous menu page set in slot %d\n", i);
			previousPages[i] = page;
			// we can jump out now
			return;
		}
	}

	debug_print("Didn't find an available previous page slot. That's bad!");
}

// sets the current page to the previous one
void ui_getPreviousMenuPage() {
	int i;

	// cycle backwards through the previous page list to find the last
	for (i = MENU_MAX_DEPTH - 1; i >= 0; i--) {
		if (previousPages[i] != -1) { // if this slot is used, return it
			_currentPage = previousPages[i];
			previousPages[i] = -1; // we've returned it now, so clear that slot
			printf("Previous page retrieved (%d) from slot (%d)\n", _currentPage, i);
			return;
		}
	}
}*/

/* start of new menu system */ 

// create a new menu page
MenuPage *ui_menuPageNew() {
	MenuPage *page = NULL; 

	page = calloc(1, sizeof(MenuPage));
	if (!page) {
		return NULL;
	}

	// set default page properties
	page->selected = 0;
	page->num_entries = 0;

	return page;
}

// add a menu page to the global list of pages
void ui_menuPageAdd(MenuPage *page) {
	if (!page) {
		return;
	}

	// if this isn't the first page, just append
	if (_menuPages) {
		list_add(_menuPages, page);
		_numMenuPages++;
		page->id = _numMenuPages;
	}
	// otherwise, we have to set the first page slightly differently
	else {
		_menuPages = list_add(NULL, page);
		_numMenuPages++;
		page->id = _numMenuPages;
	}
}

// returns a menu page which has the specified id
MenuPage *ui_menuPageGetById(int id) {
	List *node = NULL;
	MenuPage *page = NULL;

	// need to have pages in the global first
	if (!_menuPages) {
		return NULL;
	}

	// check if it is the first page in the list
	page = (MenuPage*)list_getCurrent(_menuPages);
	if (page->id == id) {
		return page;
	}
	// otherwise cycle through the list until we find it
	else {
		node = list_getNext(_menuPages);
		while (node) {
			page = (MenuPage*)list_getCurrent(node);
			if (page->id == id) {
				return page;	
			}

			node = list_getNext(node);
		}
	}

	// if we didn't find a page with that ID...
	return NULL;
}

// returns the id of the specified page
// failure returns a -1
int ui_menuPageGetId(MenuPage *page) {
	if (!page) {
		return -1;
	}

	return page->id;
}

void ui_menuPageAddEntry(MenuPage *page, MenuEntry *entry) {
	if (!page || !entry) {
		return;
	}

	// if this isn't the first entry, just append
	if (page->entries) {
		list_add(page->entries, entry); // add entry
		page->num_entries++;			// increment entry counter
		entry->id = page->num_entries;	// not accurate!!
			/* removing an entry will break the uniquness */
	}
	//otherwise, we have to set the first entry slightly differently
	else {
		page->entries = list_add(NULL, entry);
		page->num_entries++;
		entry->id = page->num_entries;	// not accurate!!
			/* removing an entry will break the uniquness */
	}
}

// remove a page from the global page list
void ui_menuPageRemove(MenuPage *page) { 
	// haven't implemented yet cause I don't really need it
}

// returns the current page
MenuPage *ui_menuPageGetCurrent() {
	return _menuCurrentPage;
}

// set the supplied page as the current
void ui_menuPageSetCurrent(MenuPage *page) {
	if (!page) {
		return;
	}
	
	// set the parent to the new page
	// enables you to jump back pages
	// it only sets a parent if one doesn't exist already
	// no lousy step parents here!
	if (ui_menuPageGetParentId(page) <= 0) {
		ui_menuPageSetParentId(page, ui_menuPageGetId(ui_menuPageGetCurrent()));
	}
	_menuCurrentPage = page;
}

// returns the parent id of the supplied page
int ui_menuPageGetParentId(MenuPage *page) {
	if (!page) {
		return -1;
	}

	return page->parent_id;
}

// sets the pages parent id to the id supplied
void ui_menuPageSetParentId(MenuPage *page, int id) {
	if (!page) {
		return;
	}

	page->parent_id = id;
}

// set the current page using the supplied page id
void ui_menuPageSetCurrentById(int id) {
	ui_menuPageSetCurrent(ui_menuPageGetById(id));
}

// return the number of entries for a page
int ui_menuPageGetNumEntires(MenuPage *page) {
	if (!page) {
		return -1;
	}

	return page->num_entries;
}

// return the list of entries for a page
List *ui_menuPageGetEntries(MenuPage *page) {
	if (!page) {
		return NULL;
	}

	return page->entries;
}

// returns the id of the selected menu entry
int ui_menuPageGetSelected(MenuPage *page) {
	if (!page) {
		return -1;
	}

	return page->selected;
}

// set page selected entry to the specified id
// WARNING: this doesn't attempt to validate selection
void ui_menuPageSetSelected(MenuPage *page, int id) {
	if (!page) {
		return;
	}

	page->selected = id;
}

// select the next entry in the page if possible
// otherwise cycle back to the first
void ui_menuPageSelectNext(MenuPage *page) {
	MenuEntry *entry = NULL;
	List *entries = NULL;
	List *node = NULL;

	if (!page) {
		return;
	}

	// cycle through the entries until we find the current
	entries = ui_menuPageGetEntries(page);
	while (entries) {
		entry = (MenuEntry *)list_getCurrent(entries);

		// is the current entry the one that is selected?
		if (ui_menuEntryGetId(entry) == ui_menuPageGetSelected(page)) {
			// try to get the next entry in the list
			node = list_getNext(entries);
			// if there is a next entry, select it
			if (node) {
				entry = (MenuEntry *)list_getCurrent(node);
				ui_menuPageSetSelected(page, ui_menuEntryGetId(entry));
				return;
			}
			// otherwise, set back to the first entry
			else {
				ui_menuPageSetSelected(page, 
					ui_menuEntryGetId(list_getCurrent(ui_menuPageGetEntries(page))));
				return;
			}
		}

		// go to next entry in the list
		entries = list_getNext(entries);
	}
}

// select the previous entry in the page if possible
// otherwise cycle back to the first
void ui_menuPageSelectPrev(MenuPage *page) {
	MenuEntry *entry = NULL;
	List *entries = NULL;
	List *node = NULL;

	if (!page) {
		return;
	}

	// cycle through the entries until we find the current
	entries = ui_menuPageGetEntries(page);
	while (entries) {
		entry = (MenuEntry *)list_getCurrent(entries);

		// is the current entry the one that is selected?
		if (ui_menuEntryGetId(entry) == ui_menuPageGetSelected(page)) {
			// try to get the prev entry in the list
			node = list_getPrev(entries);
			// if there is a prev entry, select it
			if (node) {
				entry = (MenuEntry *)list_getCurrent(node);
				ui_menuPageSetSelected(page, ui_menuEntryGetId(entry));
				return;
			}
			// otherwise, set to the last entry
			else {
				ui_menuPageSetSelected(page, 
					ui_menuEntryGetId(list_getCurrent(list_getLast(ui_menuPageGetEntries(page)))));
				return;
			}
		}

		// go to next entry in the list
		entries = list_getNext(entries);
	}
}

// create a new menu entry
MenuEntry *ui_menuEntryNew() {
	MenuEntry *entry = NULL;

	entry = calloc(1, sizeof(MenuEntry));
	if (!entry) {
		return NULL;
	}

	return entry;
}

// retrieves an entry with the specified id
// otherwise, returns null
MenuEntry *ui_menuEntryGetById(MenuPage *page, int id) {
	MenuEntry *entry = NULL; 
	List *node = NULL;

	if (!page) {
		return NULL;
	}

	// check if it is the first entry in the list
	entry = (MenuEntry*)list_getCurrent(page->entries);
	if (entry->id == id) {
		return entry;
	}
	// otherwise cycle through the list until we find it
	else {
		node = list_getNext(page->entries);
		while (node) {
			entry = (MenuEntry*)list_getCurrent(node);
			if (entry->id == id) {
				return entry;	
			}
		}
	}

	// if we didn't find a page with that ID...
	return NULL;
}

// return the id of the specified entry
// NOTE: an id is only created for an entry once it has been added
// to a page
int ui_menuEntryGetId(MenuEntry *entry) {
	if (!entry) {
		return -1;
	}

	return entry->id;
}

// set the text to display for the menu entry
void ui_menuEntrySetText(MenuEntry *entry, char text[]) {
	if (!entry) {
		return;
	}

	// allocate space for text
	entry->text = calloc(strlen(text) + 1, sizeof(char));

	// copy the text into the entry's text member var
	sprintf(entry->text, "%s", text);
}

char *ui_menuEntryGetText(MenuEntry *entry) {
	if (!entry) {
		return NULL;
	}

	return entry->text;
}

void ui_menuEntrySetType(MenuEntry *entry, int type) {
	if (!entry) {
		return;
	}

	entry->type = type;	
}

// create the menu
void ui_menuInit() {
	MenuPage *page = NULL;
	MenuEntry *entry = NULL;

	/* Main Menu Page ID = 1 */
	page = ui_menuPageNew();
	if (!page) {
		printf("Failed to create Main Menu page\n");
	}

	/* start game */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Start Game entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Start Game");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	// after adding the entry to the page, the entry gets and id
	// use this id to set it as the selected page entry
	ui_menuPageSetSelected(page, ui_menuEntryGetId(entry));

	/* high scores*/
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Start Game entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "High Scores");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	
	/* options */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Start Game entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Options");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	
	/* quit */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Start Game entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Quit");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	
	// add main menu page to the global page list
	ui_menuPageAdd(page);
	
	// make the Main menu the default
	ui_menuPageSetCurrent(page);


	/* In-Game Menu Page ID = 2*/
	page = ui_menuPageNew();
	if (!page) {
		printf("Failed to create In-Game Menu page\n");
	}

	/* resume */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Resume entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Resume");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	// after adding the entry to the page, the entry gets and id
	// use this id to set it as the selected page entry
	ui_menuPageSetSelected(page, ui_menuEntryGetId(entry));

	/* options */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Options entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Options");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	
	/* end game */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create End Game entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "End Game");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	
	// add in-game menu page to the global page list
	ui_menuPageAdd(page);

	/* Options Menu Page ID = 3 */
	page = ui_menuPageNew();
	if (!page) {
		printf("Failed to create Options Menu page\n");
	}

	/* Game */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Game entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Game");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	// after adding the entry to the page, the entry gets and id
	// use this id to set it as the selected page entry
	ui_menuPageSetSelected(page, ui_menuEntryGetId(entry));

	/* video */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create video entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Video");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	
	/* audio */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create audio entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Audio");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);

	/* input */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create input entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Input");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	
	// add options menu page to the global page list
	ui_menuPageAdd(page);


	/* Game Options Menu Page ID = 4 */
	page = ui_menuPageNew();
	if (!page) {
		printf("Failed to create Game Options Menu page\n");
	}

	/* Starting Level */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Starting Level entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Starting Level");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	// after adding the entry to the page, the entry gets and id
	// use this id to set it as the selected page entry
	ui_menuPageSetSelected(page, ui_menuEntryGetId(entry));

	/* Timer */
	entry = ui_menuEntryNew();
	if (!entry) {
		printf("Failed to create Timer entry.\n");
	}

	// set some options
	ui_menuEntrySetText(entry, "Timer");
	ui_menuEntrySetType(entry, MENU_ENTRY_REDIRECT_TYPE);
	// add entry to the page
	ui_menuPageAddEntry(page, entry);
	
	// add game options menu page to the global page list
	ui_menuPageAdd(page);
}

