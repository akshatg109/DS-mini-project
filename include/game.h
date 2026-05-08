#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "common.h"

typedef struct {
    int balance;
    bool hasBet;
    Bet currentBet;
    int lastSpin;
    bool lastWin;
    int lastPayout;
} GameState;

void game_init(GameState *state, int startingBalance);
bool game_place_bet(GameState *state, Bet bet);
bool game_can_spin(const GameState *state);
RoundRecord game_spin(GameState *state);
int game_estimated_return(const Bet *bet);

#endif
