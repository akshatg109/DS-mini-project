#ifndef DS_STACK_H
#define DS_STACK_H

#include <stdbool.h>

#include "common.h"

#define BET_STACK_CAPACITY 64

typedef struct {
    BetSnapshot items[BET_STACK_CAPACITY];
    int top;
} BetStack;

void stack_init(BetStack *stack);
bool stack_push(BetStack *stack, BetSnapshot snapshot);
bool stack_pop(BetStack *stack, BetSnapshot *outSnapshot);
bool stack_is_empty(const BetStack *stack);
void stack_clear(BetStack *stack);

#endif
