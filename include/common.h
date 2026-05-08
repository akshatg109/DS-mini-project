#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

#define ROULETTE_MIN_NUMBER 0
#define ROULETTE_MAX_NUMBER 36

typedef enum {
    BET_NONE = 0,
    BET_NUMBER,
    BET_RED,
    BET_BLACK,
    BET_ODD,
    BET_EVEN
} BetType;

typedef struct {
    BetType type;
    int number;
    int amount;
} Bet;

typedef struct {
    bool hasBet;
    Bet bet;
} BetSnapshot;

typedef struct {
    Bet bet;
    int spinNumber;
    bool win;
    int payout;
    int balanceAfter;
} RoundRecord;

#endif
