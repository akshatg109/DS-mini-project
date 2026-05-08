#include "ds_stack.h"

#include <stddef.h>

void stack_init(BetStack *stack) {
    stack->top = -1;
}

bool stack_push(BetStack *stack, BetSnapshot snapshot) {
    if (stack->top >= BET_STACK_CAPACITY - 1) {
        return false;
    }

    stack->top++;
    stack->items[stack->top] = snapshot;
    return true;
}

bool stack_pop(BetStack *stack, BetSnapshot *outSnapshot) {
    if (stack->top < 0) {
        return false;
    }

    if (outSnapshot != NULL) {
        *outSnapshot = stack->items[stack->top];
    }

    stack->top--;
    return true;
}

bool stack_is_empty(const BetStack *stack) {
    return stack->top < 0;
}

void stack_clear(BetStack *stack) {
    stack->top = -1;
}
