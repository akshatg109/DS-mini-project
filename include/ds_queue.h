#ifndef DS_QUEUE_H
#define DS_QUEUE_H

#include <stdbool.h>

#define EVENT_QUEUE_CAPACITY 32
#define EVENT_TEXT_MAX 128

typedef struct {
    char items[EVENT_QUEUE_CAPACITY][EVENT_TEXT_MAX];
    int front;
    int rear;
    int size;
} EventQueue;

void queue_init(EventQueue *queue);
bool queue_enqueue(EventQueue *queue, const char *message);
bool queue_dequeue(EventQueue *queue, char *outMessage);
int queue_size(const EventQueue *queue);
void queue_peek_recent(const EventQueue *queue, int recentCount, char out[][EVENT_TEXT_MAX], int *outCount);

#endif
