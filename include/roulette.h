#ifndef ROULETTE_H
#define ROULETTE_H

#include "common.h"

typedef enum {
    ROULETTE_GREEN = 0,
    ROULETTE_RED,
    ROULETTE_BLACK
} RouletteColor;

int roulette_spin_number(void);
RouletteColor roulette_number_color(int number);
const char *roulette_color_name(RouletteColor color);
const char *bet_type_name(BetType betType);

#endif
