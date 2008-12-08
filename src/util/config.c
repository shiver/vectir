#include "config.h"

#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// load all config entries from the specified file
void config_load(const char *filename) {
	FILE *file;
	char str[256];
	int i;
	char *token, *key, *value;
	ConfigEntry *entry = NULL;

	key = NULL;	value = NULL;

	// open config file for reading (plain text)
	file = fopen(filename, "r");
	if (!file) {
		error_print("Failed to open config file.");
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

		// create new config entry
		entry = config_createNewEntry();
		if (!entry) {
			error_print("Failed to create new config entry.");
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

// creates a new config entry
ConfigEntry *config_createNewEntry() {
	ConfigEntry *entry = NULL;
	ConfigEntry *prev = NULL;

 	if (!config) { // no config entries yet
 		config = calloc(1, sizeof(ConfigEntry));
		if (!config) {
			error_print("Failed to allocate memory for first config entry.");
			return NULL;
		}
		return config;	
	}
	else { // append a new entry
		entry = config;

		// find the next open space
		while (entry) {
			prev = entry;
			entry = entry->next;
		}

		// allocate memory for new entry and set access
		entry = calloc(1, sizeof(ConfigEntry));
		if (!entry) {
			error_print("Failed to allocate memory to append config entry.");
			return NULL;
		}
		
		prev->next = entry;
		return entry;
	}
}

char *config_getValue(const char *key) {
	ConfigEntry *entry = NULL;

	entry = config;
	
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

// sets the value of the config entry with the 
// supplied key
void config_setValue(const char *key, const char *value) {
	ConfigEntry *entry = NULL;

	entry = config;
	
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
	entry = config_createNewEntry();
	entry->key = strdup(key);
	entry->value = strdup(value);
	
}

// simple overwrite with everything we have
void config_save(const char *filename) {
	FILE *file;
	ConfigEntry *entry = NULL;

	// config needs to be loaded to be saved
	if (config == NULL) {
		return;
	}

	// open the file for writing
	file = fopen(filename, "w");
	if (!file) {
		error_print("Failed to save config file.");
		return;
	}

	entry = config;
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

// destroys all ConfigEntries
void config_destroy() {
  ConfigEntry *current = NULL;
  ConfigEntry *next = NULL;
  
  // check if a config has been loaded
  if (!config) {
    return;
  }
  
  current = config;
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

