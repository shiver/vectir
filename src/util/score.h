#ifndef SCORE_H
#define SCORE_H

// limit to only top 10 scores
#define MAX_NUM_SCORES 10

typedef struct _scoreEntry {
	char *key;
	char *value;
	void *next;
} ScoreEntry;

ScoreEntry *score;

void score_load(const char *);
void score_save(const char *);
void score_destroy();
char *score_getValue(const char *);
void score_setValue(const char *, const char *);
ScoreEntry *score_createNewEntry();

#endif
