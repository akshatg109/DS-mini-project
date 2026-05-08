#include "ds_queue.h"

#include <stdio.h>
#include <string.h>

void queue_init(EventQueue *queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}

bool queue_enqueue(EventQueue *queue, const char *message) {
    if (queue->size == EVENT_QUEUE_CAPACITY) {
        char discarded[EVENT_TEXT_MAX];
        (void)queue_dequeue(queue, discarded);
    }

    queue->rear = (queue->rear + 1) % EVENT_QUEUE_CAPACITY;
    snprintf(queue->items[queue->rear], EVENT_TEXT_MAX, "%s", message);
    queue->size++;
    return true;
}

bool queue_dequeue(EventQueue *queue, char *outMessage) {
    if (queue->size == 0) {
        return false;
    }

    if (outMessage != NULL) {
        snprintf(outMessage, EVENT_TEXT_MAX, "%s", queue->items[queue->front]);
    }

    queue->front = (queue->front + 1) % EVENT_QUEUE_CAPACITY;
    queue->size--;
    return true;
}

int queue_size(const EventQueue *queue) {
    return queue->size;
}

void queue_peek_recent(const EventQueue *queue, int recentCount, char out[][EVENT_TEXT_MAX], int *outCount) {
    int wanted = recentCount;
    if (wanted > queue->size) {
        wanted = queue->size;
    }

    int start = queue->size - wanted;
    for (int i = 0; i < wanted; i++) {
        int idx = (queue->front + start + i) % EVENT_QUEUE_CAPACITY;
        snprintf(out[i], EVENT_TEXT_MAX, "%s", queue->items[idx]);
    }

    if (outCount != NULL) {
        *outCount = wanted;
    }
}
