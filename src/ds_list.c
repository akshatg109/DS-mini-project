#include "ds_list.h"

#include <stdlib.h>

void history_init(HistoryList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

int history_append(HistoryList *list, RoundRecord record) {
    HistoryNode *node = (HistoryNode *)malloc(sizeof(HistoryNode));
    if (node == NULL) {
        return 0;
    }

    node->record = record;
    node->next = NULL;

    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    list->size++;
    return 1;
}

void history_clear(HistoryList *list) {
    HistoryNode *curr = list->head;
    while (curr != NULL) {
        HistoryNode *next = curr->next;
        free(curr);
        curr = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

int history_size(const HistoryList *list) {
    return list->size;
}

int history_recent(const HistoryList *list, int count, RoundRecord *out) {
    if (count <= 0 || out == NULL || list->size == 0) {
        return 0;
    }

    int start = list->size - count;
    if (start < 0) {
        start = 0;
    }

    int index = 0;
    int outIndex = 0;
    HistoryNode *curr = list->head;
    while (curr != NULL) {
        if (index >= start) {
            out[outIndex] = curr->record;
            outIndex++;
        }
        curr = curr->next;
        index++;
    }

    return outIndex;
}
