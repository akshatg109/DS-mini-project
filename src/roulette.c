#include "roulette.h"

#include <raylib.h>

static const int RED_NUMBERS[] = {
    1, 3, 5, 7, 9, 12, 14, 16, 18,
    19, 21, 23, 25, 27, 30, 32, 34, 36
};

int roulette_spin_number(void) {
    return GetRandomValue(ROULETTE_MIN_NUMBER, ROULETTE_MAX_NUMBER);
}

RouletteColor roulette_number_color(int number) {
    if (number == 0) {
        return ROULETTE_GREEN;
    }

    int total = (int)(sizeof(RED_NUMBERS) / sizeof(RED_NUMBERS[0]));
    for (int i = 0; i < total; i++) {
        if (RED_NUMBERS[i] == number) {
            return ROULETTE_RED;
        }
    }

    return ROULETTE_BLACK;
}

const char *roulette_color_name(RouletteColor color) {
    switch (color) {
        case ROULETTE_GREEN:
            return "Green";
        case ROULETTE_RED:
            return "Red";
        case ROULETTE_BLACK:
            return "Black";
        default:
            return "Unknown";
    }
}

const char *bet_type_name(BetType betType) {
    switch (betType) {
        case BET_NUMBER:
            return "Number";
        case BET_RED:
            return "Red";
        case BET_BLACK:
            return "Black";
        case BET_ODD:
            return "Odd";
        case BET_EVEN:
            return "Even";
        default:
            return "None";
    }
}
