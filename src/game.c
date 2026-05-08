#include "game.h"

#include <stddef.h>

#include "roulette.h"

void game_init(GameState *state, int startingBalance) {
    state->balance = startingBalance;
    state->hasBet = false;
    state->currentBet.type = BET_NONE;
    state->currentBet.amount = 0;
    state->currentBet.number = 0;
    state->lastSpin = -1;
    state->lastWin = false;
    state->lastPayout = 0;
}

bool game_place_bet(GameState *state, Bet bet) {
    if (bet.amount <= 0) {
        return false;
    }
    if (bet.type == BET_NUMBER && (bet.number < ROULETTE_MIN_NUMBER || bet.number > ROULETTE_MAX_NUMBER)) {
        return false;
    }
    if (bet.amount > state->balance) {
        return false;
    }

    state->currentBet = bet;
    state->hasBet = true;
    return true;
}

bool game_can_spin(const GameState *state) {
    return state->hasBet;
}

static bool game_is_win(const Bet *bet, int spinNumber) {
    RouletteColor color = roulette_number_color(spinNumber);

    switch (bet->type) {
        case BET_NUMBER:
            return bet->number == spinNumber;
        case BET_RED:
            return color == ROULETTE_RED;
        case BET_BLACK:
            return color == ROULETTE_BLACK;
        case BET_ODD:
            return spinNumber != 0 && (spinNumber % 2 != 0);
        case BET_EVEN:
            return spinNumber != 0 && (spinNumber % 2 == 0);
        default:
            return false;
    }
}

int game_estimated_return(const Bet *bet) {
    if (bet == NULL) {
        return 0;
    }

    if (bet->type == BET_NUMBER) {
        return bet->amount * 35;
    }
    if (bet->type == BET_RED || bet->type == BET_BLACK || bet->type == BET_ODD || bet->type == BET_EVEN) {
        return bet->amount;
    }

    return 0;
}

RoundRecord game_spin(GameState *state) {
    RoundRecord record;
    record.bet = state->currentBet;
    record.spinNumber = roulette_spin_number();
    record.win = false;
    record.payout = -state->currentBet.amount;
    record.balanceAfter = state->balance;

    state->balance -= state->currentBet.amount;
    bool win = game_is_win(&state->currentBet, record.spinNumber);
    if (win) {
        int gain = game_estimated_return(&state->currentBet);
        state->balance += state->currentBet.amount + gain;
        record.win = true;
        record.payout = gain;
    }

    record.balanceAfter = state->balance;

    state->lastSpin = record.spinNumber;
    state->lastWin = record.win;
    state->lastPayout = record.payout;
    state->hasBet = false;
    state->currentBet.type = BET_NONE;
    state->currentBet.amount = 0;
    state->currentBet.number = 0;

    return record;
}
