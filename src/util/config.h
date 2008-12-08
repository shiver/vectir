#ifndef CONFIG_H
#define CONFIG_H

typedef struct _configEntry {
	char *key;
	char *value;
	void *next;
} ConfigEntry;

ConfigEntry *config;

void config_load(const char *);
void config_save(const char *);
void config_destroy();
char *config_getValue(const char *);
void config_setValue(const char *, const char *);
ConfigEntry *config_createNewEntry();

#endif
