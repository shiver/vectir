#include "score.h"

#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// load all Score entries from the specified file
void score_load(const char *filename) {
	FILE *file;
	char str[256];
	int i;
	char *token, *key, *value;
	ScoreEntry *entry = NULL;

	key = NULL;	value = NULL;

	// open Score file for reading (plain text)
	file = fopen(filename, "r");
	if (!file) {
		error_print("Failed to open Score file.");
		return;
	}

	// read a line from the file
	while (fgets(str, 255, file)) {
		
		// strip trailing \n and whitespace
		for (i = 0; i < 256; i++) {
			if (str[i] == ' ' || str[i] == '\n'){
				str[i] = '\0';
			}
		}
		// get the key
		token = strtok(str, "=");
		key = calloc(1, sizeof(char) * strlen(token) + 1);
		strncpy(key, token, strlen(token) + 1);
		// get the value
		token = strtok(NULL, "=");
		if (token) { // handle null values
			value = calloc(1, sizeof(char) * strlen(token) + 1);
			strncpy(value, token, strlen(token) + 1);
		}

		// create new Score entry
		entry = score_createNewEntry();
		if (!entry) {
			error_print("Failed to create new Score entry.");
		}
		else {
			entry->key = strdup(key);
			entry->value = strdup(value);	
		}
		
		// free current key and value resources
		if (key) {
			free(key);
			key = NULL;
		}
		if (value) {
			free(value);
			value = NULL;
		}
	}

	// close the file
	fclose(file);
	file = NULL;
}

// creates a new Score entry
ScoreEntry *score_createNewEntry() {
	ScoreEntry *entry = NULL;
	ScoreEntry *prev = NULL;

 	if (!score) { // no Score entries yet
 		score = calloc(1, sizeof(ScoreEntry));
		if (!score) {
			error_print("Failed to allocate memory for first Score entry.");
			return NULL;
		}
		return score;	
	}
	else { // append a new entry
		entry = score;

		// find the next open space
		while (entry) {
			prev = entry;
			entry = entry->next;
		}

		// allocate memory for new entry and set access
		entry = calloc(1, sizeof(ScoreEntry));
		if (!entry) {
			error_print("Failed to allocate memory to append Score entry.");
			return NULL;
		}
		
		prev->next = entry;
		return entry;
	}
}

char *score_getValue(const char *key) {
	ScoreEntry *entry = NULL;

	entry = score;
	
	// cycle through all entries until we find
	// what we're looking for
	while (entry) {
		if(strcmp(key, entry->key) == 0) {
			return entry->value;
		}
		entry = entry->next;
	}

	return NULL;
}

// sets the value of the Score entry with the 
// supplied key
void score_setValue(const char *key, const char *value) {
	ScoreEntry *entry = NULL;

	entry = score;
	
	// cycle through all entries until we find
	// what we're looking for
	while (entry) {
		if(strcmp(key, entry->key) == 0) {
			entry->value = strdup(value);
			return;
		}
		entry = entry->next;
	}

	// if we didn't find the key, we need to create it
	entry = score_createNewEntry();
	entry->key = strdup(key);
	entry->value = strdup(value);
	
}

// simple overwrite with everything we have
void score_save(const char *filename) {
	FILE *file;
	ScoreEntry *entry = NULL;

	// Score needs to be loaded to be saved
	if (score == NULL) {
		return;
	}

	// open the file for writing
	file = fopen(filename, "w");
	if (!file) {
		error_print("Failed to save Score file.");
		return;
	}

	entry = score;
	while (entry) {
		if (entry->value) {
			fprintf(file, "%s=%s\n", entry->key, entry->value);
		}
		else {
			fprintf(file, "%s=\n", entry->key);
		}
		entry = entry->next;
	}
}

// destroys all ScoreEntries
void score_destroy() {
  ScoreEntry *current = NULL;
  ScoreEntry *next = NULL;
  
  // check if a Score has been loaded
  if (!score) {
    return;
  }
  
  current = score;
  while (current) {
    next = current->next;
    if (current->key) {
      free(current->key);
      current->key = NULL;
    }
    
    if (current->value) {
      free(current->value);
      current->value = NULL;
    }
    
    free(current);
    current = next;
  }
}

