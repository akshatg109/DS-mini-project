#ifndef DS_LIST_H
#define DS_LIST_H

#include "common.h"

typedef struct HistoryNode {
    RoundRecord record;
    struct HistoryNode *next;
} HistoryNode;

typedef struct {
    HistoryNode *head;
    HistoryNode *tail;
    int size;
} HistoryList;

void history_init(HistoryList *list);
int history_append(HistoryList *list, RoundRecord record);
void history_clear(HistoryList *list);
int history_size(const HistoryList *list);
int history_recent(const HistoryList *list, int count, RoundRecord *out);

#endif
