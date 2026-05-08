#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>

#include "ds_stack.h"
#include "game.h"
#include "gui.h"
#include "roulette.h"

#define STATUS_TEXT_MAX 128
#define SPIN_DURATION 2.40f
#define WHEEL_SLOT_COUNT 37

static const int WHEEL_ORDER[WHEEL_SLOT_COUNT] = {
    0, 32, 15, 19, 4, 21, 2, 25, 17, 34,
    6, 27, 13, 36, 11, 30, 8, 23, 10, 5,
    24, 16, 33, 1, 20, 14, 31, 9, 22, 18,
    29, 7, 28, 12, 35, 3, 26
};

static void draw_centered_outlined_text(const char *text, Vector2 center, int fontSize, Color fill, Color outline) {
    int textWidth = MeasureText(text, fontSize);
    int x = (int)lroundf(center.x - (float)textWidth * 0.5f);
    int y = (int)lroundf(center.y - (float)fontSize * 0.5f);

    DrawText(text, x - 1, y, fontSize, outline);
    DrawText(text, x + 1, y, fontSize, outline);
    DrawText(text, x, y - 1, fontSize, outline);
    DrawText(text, x, y + 1, fontSize, outline);
    DrawText(text, x, y, fontSize, fill);
}

static float clamp01(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

static float ease_out_cubic(float t) {
    float p = 1.0f - clamp01(t);
    return 1.0f - p * p * p;
}

static float wrap_angle(float angle) {
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    while (angle < 0.0f) {
        angle += 360.0f;
    }
    return angle;
}

static int wheel_slot_index(int number) {
    for (int i = 0; i < WHEEL_SLOT_COUNT; i++) {
        if (WHEEL_ORDER[i] == number) {
            return i;
        }
    }
    return 0;
}

static void set_status_message(char *buffer, int size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, (size_t)size, fmt, args);
    va_end(args);
}

static void clear_current_bet(GameState *game) {
    game->hasBet = false;
    game->currentBet.type = BET_NONE;
    game->currentBet.amount = 0;
    game->currentBet.number = 0;
}

static const char *spin_color_text(int number) {
    RouletteColor color = roulette_number_color(number);
    return roulette_color_name(color);
}

static Color spin_color_fill(int number, Theme theme) {
    RouletteColor color = roulette_number_color(number);
    if (color == ROULETTE_RED) {
        return theme.redChip;
    }
    if (color == ROULETTE_BLACK) {
        return theme.blackChip;
    }
    return theme.positive;
}

static void format_bet_text(const Bet *bet, char *buffer, int size) {
    if (bet->type == BET_NUMBER) {
        snprintf(buffer, (size_t)size, "Number %d - $%d", bet->number, bet->amount);
        return;
    }
    snprintf(buffer, (size_t)size, "%s - $%d", bet_type_name(bet->type), bet->amount);
}

int main(void) {
    const int minWidth = 1000;
    const int minHeight = 700;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1200, 760, "Roulette Casino DS Project");
    SetWindowMinSize(minWidth, minHeight);
    SetTargetFPS(60);

    GameState game;
    game_init(&game, 1000);

    BetStack stack;
    stack_init(&stack);

    Theme theme = gui_make_theme();

    int selectedAmount = 25;
    int selectedNumber = 17;

    bool spinning = false;
    float spinTimer = 0.0f;
    float wheelAngle = 0.0f;
    float ballAngle = 0.0f;

    RoundRecord latestRecord;
    latestRecord.spinNumber = -1;
    latestRecord.win = false;
    latestRecord.payout = 0;
    latestRecord.balanceAfter = game.balance;
    latestRecord.bet.type = BET_NONE;
    latestRecord.bet.number = 0;
    latestRecord.bet.amount = 0;

    char statusMessage[STATUS_TEXT_MAX];
    set_status_message(statusMessage, sizeof(statusMessage), "Roulette table ready. Place a bet to begin.");

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        if (spinning) {
            spinTimer -= dt;
            float spinProgress = 1.0f - (spinTimer / SPIN_DURATION);
            float eased = ease_out_cubic(spinProgress);

            float wheelSpeed = 760.0f - 520.0f * eased;
            float ballSpeed = 1280.0f - 900.0f * eased;

            wheelAngle = wrap_angle(wheelAngle + wheelSpeed * dt);
            ballAngle = wrap_angle(ballAngle - ballSpeed * dt);

            if (spinTimer <= 0.0f) {
                spinning = false;
                spinTimer = 0.0f;
                latestRecord = game_spin(&game);
                stack_clear(&stack);

                float slotAngle = 360.0f / (float)WHEEL_SLOT_COUNT;
                int slotIndex = wheel_slot_index(latestRecord.spinNumber);
                ballAngle = wrap_angle(wheelAngle + (slotIndex + 0.5f) * slotAngle);

                if (latestRecord.win) {
                    set_status_message(statusMessage,
                                       sizeof(statusMessage),
                                       "Result: %d %s. You win $%d.",
                                       latestRecord.spinNumber,
                                       spin_color_text(latestRecord.spinNumber),
                                       latestRecord.payout);
                } else {
                    set_status_message(statusMessage,
                                       sizeof(statusMessage),
                                       "Result: %d %s. You lose $%d.",
                                       latestRecord.spinNumber,
                                       spin_color_text(latestRecord.spinNumber),
                                       -latestRecord.payout);
                }
            }
        }

        BeginDrawing();
        gui_draw_background(sw, sh, theme);

        float margin = 24.0f;
        float gap = 20.0f;
        float usableWidth = (float)sw - margin * 2.0f - gap;
        float leftWidth = usableWidth * 0.52f;
        float rightWidth = usableWidth - leftWidth;

        Rectangle left = {margin, margin, leftWidth, (float)sh - margin * 2.0f};
        Rectangle right = {left.x + left.width + gap, margin, rightWidth, (float)sh - margin * 2.0f};

        gui_draw_panel(left, theme.felt, 0.90f);
        gui_draw_panel(right, theme.feltDark, 0.90f);

        DrawText("Roulette Table", (int)left.x + 22, (int)left.y + 16, 40, theme.gold);
        DrawText(TextFormat("Balance: $%d", game.balance), (int)left.x + 24, (int)left.y + 68, 28, theme.ivory);

        Rectangle amountPanel = {left.x + 20, left.y + 112, left.width - 40, 86};
        gui_draw_panel(amountPanel, Fade(theme.blackChip, 0.35f), 1.0f);
        DrawText("Chip Size", (int)amountPanel.x + 14, (int)amountPanel.y + 12, 22, theme.muted);

        int chips[4] = {10, 25, 50, 100};
        float chipWidth = 80.0f;
        float chipGap = 12.0f;
        float chipRowWidth = chipWidth * 4.0f + chipGap * 3.0f;
        float chipStartX = amountPanel.x + (amountPanel.width - chipRowWidth) * 0.5f;

        for (int i = 0; i < 4; i++) {
            Rectangle chip = {chipStartX + i * (chipWidth + chipGap), amountPanel.y + 42, chipWidth, 32};
            Color fill = (selectedAmount == chips[i]) ? theme.gold : Fade(theme.ivory, 0.28f);
            Color textColor = (selectedAmount == chips[i]) ? theme.blackChip : theme.ivory;

            DrawRectangleRounded(chip, 0.4f, 12, fill);

            char chipLabel[16];
            snprintf(chipLabel, sizeof(chipLabel), "$%d", chips[i]);
            int chipLabelWidth = MeasureText(chipLabel, 18);
            DrawText(chipLabel,
                     (int)(chip.x + (chip.width - (float)chipLabelWidth) * 0.5f),
                     (int)chip.y + 8,
                     18,
                     textColor);

            if (!spinning && CheckCollisionPointRec(GetMousePosition(), chip) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selectedAmount = chips[i];
            }
        }

        Rectangle numberPanel = {left.x + 20, left.y + 212, left.width - 40, 96};
        gui_draw_panel(numberPanel, Fade(theme.blackChip, 0.35f), 1.0f);
        DrawText("Pick a Number", (int)numberPanel.x + 14, (int)numberPanel.y + 12, 22, theme.muted);

        Rectangle numDisplay = {numberPanel.x + numberPanel.width * 0.5f - 43.0f, numberPanel.y + 48, 86, 34};
        Rectangle minusBtn = {numDisplay.x - 52.0f, numDisplay.y, 42, 34};
        Rectangle plusBtn = {numDisplay.x + numDisplay.width + 10.0f, numDisplay.y, 42, 34};

        if (!spinning && gui_button(minusBtn, "-", 26, theme)) {
            selectedNumber--;
            if (selectedNumber < ROULETTE_MIN_NUMBER) {
                selectedNumber = ROULETTE_MAX_NUMBER;
            }
        }
        if (!spinning && gui_button(plusBtn, "+", 24, theme)) {
            selectedNumber++;
            if (selectedNumber > ROULETTE_MAX_NUMBER) {
                selectedNumber = ROULETTE_MIN_NUMBER;
            }
        }

        gui_draw_badge(numDisplay, TextFormat("%d", selectedNumber), Fade(theme.ivory, 0.22f), theme.ivory, 24);

        DrawText("Bet Type", (int)left.x + 22, (int)left.y + 326, 26, theme.muted);

        float betX = left.x + 20.0f;
        float betWidth = left.width - 40.0f;
        float betGap = 12.0f;
        float topRowWidth = (betWidth - betGap * 2.0f) / 3.0f;
        float bottomRowWidth = (betWidth - betGap) / 2.0f;
        float topRowY = left.y + 360.0f;
        float bottomRowY = topRowY + 58.0f;

        Rectangle betButtons[5] = {
            {betX, topRowY, topRowWidth, 48},
            {betX + topRowWidth + betGap, topRowY, topRowWidth, 48},
            {betX + (topRowWidth + betGap) * 2.0f, topRowY, topRowWidth, 48},
            {betX, bottomRowY, bottomRowWidth, 48},
            {betX + bottomRowWidth + betGap, bottomRowY, bottomRowWidth, 48}
        };

        const char *labels[5] = {"Number", "Red", "Black", "Odd", "Even"};
        BetType types[5] = {BET_NUMBER, BET_RED, BET_BLACK, BET_ODD, BET_EVEN};

        for (int i = 0; i < 5; i++) {
            if (spinning) {
                DrawRectangleRounded(betButtons[i], 0.2f, 10, Fade(theme.muted, 0.25f));
                int labelWidth = MeasureText(labels[i], 22);
                DrawText(labels[i],
                         (int)(betButtons[i].x + (betButtons[i].width - (float)labelWidth) * 0.5f),
                         (int)betButtons[i].y + 14,
                         22,
                         Fade(theme.ivory, 0.7f));
                continue;
            }

            if (gui_button(betButtons[i], labels[i], 22, theme)) {
                BetSnapshot snapshot;
                snapshot.hasBet = game.hasBet;
                snapshot.bet = game.currentBet;
                bool snapshotStored = stack_push(&stack, snapshot);
                if (!snapshotStored) {
                    set_status_message(statusMessage,
                                       sizeof(statusMessage),
                                       "Undo stack is full. Additional steps will not be saved.");
                }

                Bet newBet;
                newBet.type = types[i];
                newBet.number = selectedNumber;
                newBet.amount = selectedAmount;

                if (!game_place_bet(&game, newBet)) {
                    set_status_message(statusMessage,
                                       sizeof(statusMessage),
                                       "Bet not valid. Check chip size and balance.");
                    if (snapshotStored) {
                        BetSnapshot rollback;
                        if (stack_pop(&stack, &rollback)) {
                            game.hasBet = rollback.hasBet;
                            game.currentBet = rollback.bet;
                        }
                    }
                } else {
                    char betLabel[64];
                    format_bet_text(&newBet, betLabel, sizeof(betLabel));
                    set_status_message(statusMessage, sizeof(statusMessage), "Active bet: %s", betLabel);
                }
            }
        }

        float actionY = left.y + 492.0f;
        float actionGap = 10.0f;
        float spinWidth = 196.0f;
        float controlWidth = (betWidth - spinWidth - actionGap * 2.0f) * 0.5f;

        Rectangle undoBtn = {betX, actionY, controlWidth, 46};
        Rectangle clearBtn = {betX + controlWidth + actionGap, actionY, controlWidth, 46};
        Rectangle spinBtn = {betX + betWidth - spinWidth, actionY - 10.0f, spinWidth, 62};

        if (!spinning && gui_button(undoBtn, "Undo", 24, theme)) {
            BetSnapshot snapshot;
            if (stack_pop(&stack, &snapshot)) {
                game.hasBet = snapshot.hasBet;
                game.currentBet = snapshot.bet;
                set_status_message(statusMessage, sizeof(statusMessage), "Returned to the previous bet.");
            } else {
                set_status_message(statusMessage, sizeof(statusMessage), "No previous bet to undo.");
            }
        }

        if (!spinning && gui_button(clearBtn, "Clear", 24, theme)) {
            clear_current_bet(&game);
            stack_clear(&stack);
            set_status_message(statusMessage, sizeof(statusMessage), "Bet cleared.");
        }

        if (!spinning && gui_button(spinBtn, "SPIN", 30, theme)) {
            if (game_can_spin(&game)) {
                spinning = true;
                spinTimer = SPIN_DURATION;
                set_status_message(statusMessage, sizeof(statusMessage), "Spinning the wheel...");
            } else {
                set_status_message(statusMessage, sizeof(statusMessage), "Place a bet to spin.");
            }
        }

        Rectangle activeBetPanel = {left.x + 20, left.y + left.height - 106, left.width - 40, 86};
        gui_draw_panel(activeBetPanel, Fade(theme.blackChip, 0.35f), 1.0f);
        DrawText("Active Bet", (int)activeBetPanel.x + 14, (int)activeBetPanel.y + 10, 22, theme.muted);
        if (game.hasBet) {
            char currentBetText[64];
            format_bet_text(&game.currentBet, currentBetText, sizeof(currentBetText));
            DrawText(currentBetText, (int)activeBetPanel.x + 14, (int)activeBetPanel.y + 42, 24, theme.ivory);
        } else {
            DrawText("No bet selected", (int)activeBetPanel.x + 14, (int)activeBetPanel.y + 42, 24, theme.ivory);
        }

        DrawText("Roulette Wheel", (int)right.x + 20, (int)right.y + 16, 34, theme.gold);

        Rectangle wheelPanel = {right.x + 20, right.y + 66, right.width - 40, right.height * 0.56f};
        gui_draw_panel(wheelPanel, Fade(theme.blackChip, 0.30f), 1.0f);

        Vector2 wheelCenter = {wheelPanel.x + wheelPanel.width * 0.5f, wheelPanel.y + wheelPanel.height * 0.54f};
        float radius = fminf(wheelPanel.width, wheelPanel.height) * 0.33f;

        float slotAngle = 360.0f / (float)WHEEL_SLOT_COUNT;
        float outerPocketRadius = radius;
        float innerPocketRadius = radius - 28.0f;
        float numberRadius = innerPocketRadius + (outerPocketRadius - innerPocketRadius) * 0.55f;
        int numberFontSize = (int)fminf(20.0f, fmaxf(14.0f, outerPocketRadius * 0.11f));

        DrawCircleV(wheelCenter, radius + 28.0f, Fade(theme.gold, 0.18f));
        DrawCircleV(wheelCenter, radius + 16.0f, Fade(theme.blackChip, 0.74f));

        for (int i = 0; i < WHEEL_SLOT_COUNT; i++) {
            int slotNumber = WHEEL_ORDER[i];
            Color slotColor;
            if (slotNumber == 0) {
                slotColor = Fade(theme.positive, 0.95f);
            } else {
                RouletteColor slotType = roulette_number_color(slotNumber);
                slotColor = (slotType == ROULETTE_RED) ? theme.redChip : theme.blackChip;
            }

            float startAngle = wheelAngle + (float)i * slotAngle;
            float endAngle = startAngle + slotAngle;
            DrawCircleSector(wheelCenter, outerPocketRadius, startAngle, endAngle, 16, slotColor);
        }

        for (int i = 0; i < WHEEL_SLOT_COUNT; i++) {
            float edgeAngle = (wheelAngle + (float)i * slotAngle) * DEG2RAD;
            Vector2 edgeInner = {
                wheelCenter.x + cosf(edgeAngle) * (innerPocketRadius + 1.0f),
                wheelCenter.y + sinf(edgeAngle) * (innerPocketRadius + 1.0f),
            };
            Vector2 edgeOuter = {
                wheelCenter.x + cosf(edgeAngle) * (outerPocketRadius - 1.0f),
                wheelCenter.y + sinf(edgeAngle) * (outerPocketRadius - 1.0f),
            };
            DrawLineEx(edgeInner, edgeOuter, 1.5f, Fade(theme.ivory, 0.30f));
        }

        for (int i = 0; i < WHEEL_SLOT_COUNT; i++) {
            int slotNumber = WHEEL_ORDER[i];
            float centerAngle = (wheelAngle + ((float)i + 0.5f) * slotAngle) * DEG2RAD;
            Vector2 numberPos = {
                wheelCenter.x + cosf(centerAngle) * numberRadius,
                wheelCenter.y + sinf(centerAngle) * numberRadius,
            };

            char numberText[4];
            snprintf(numberText, sizeof(numberText), "%d", slotNumber);

            Color fill = theme.ivory;
            if (slotNumber == 0) {
                fill = theme.gold;
            }
            draw_centered_outlined_text(numberText, numberPos, numberFontSize, fill, Fade(BLACK, 0.92f));
        }

        DrawCircleV(wheelCenter, innerPocketRadius, Fade(theme.felt, 0.95f));
        DrawCircleV(wheelCenter, innerPocketRadius * 0.58f, Fade(theme.blackChip, 0.80f));
        DrawCircleV(wheelCenter, innerPocketRadius * 0.20f, Fade(theme.gold, 0.95f));
        DrawCircleLines((int)wheelCenter.x, (int)wheelCenter.y, outerPocketRadius, Fade(theme.gold, 0.60f));
        DrawCircleLines((int)wheelCenter.x, (int)wheelCenter.y, innerPocketRadius, Fade(theme.ivory, 0.30f));

        float spinProgress = spinning ? (1.0f - (spinTimer / SPIN_DURATION)) : 1.0f;
        float easedTrack = ease_out_cubic(spinProgress);
        float ballTrackRadius = outerPocketRadius - (spinning ? (8.0f + 14.0f * easedTrack) : 22.0f);

        float ballRadians = ballAngle * DEG2RAD;
        Vector2 ball = {
            wheelCenter.x + cosf(ballRadians) * ballTrackRadius,
            wheelCenter.y + sinf(ballRadians) * ballTrackRadius,
        };

        DrawCircleV((Vector2){ball.x + 1.6f, ball.y + 1.6f}, 8.8f, Fade(BLACK, 0.32f));
        DrawCircleV(ball, 8.2f, theme.ivory);
        DrawCircleV((Vector2){ball.x - 2.4f, ball.y - 2.4f}, 2.2f, Fade(RAYWHITE, 0.72f));

        Vector2 markerTip = {wheelCenter.x, wheelCenter.y - outerPocketRadius - 3.0f};
        Vector2 markerLeft = {markerTip.x - 12.0f, markerTip.y - 24.0f};
        Vector2 markerRight = {markerTip.x + 12.0f, markerTip.y - 24.0f};
        DrawTriangle(markerTip, markerLeft, markerRight, theme.gold);

        Rectangle wheelBadge = {wheelCenter.x - 58.0f, wheelCenter.y - 24.0f, 116, 48};
        if (spinning) {
            gui_draw_badge(wheelBadge, "...", Fade(theme.ivory, 0.2f), theme.ivory, 30);
        } else if (latestRecord.spinNumber >= 0) {
            char spinText[32];
            snprintf(spinText, sizeof(spinText), "%d", latestRecord.spinNumber);
            gui_draw_badge(wheelBadge, spinText, spin_color_fill(latestRecord.spinNumber, theme), theme.ivory, 30);

            const char *colorText = spin_color_text(latestRecord.spinNumber);
            int colorTextWidth = MeasureText(colorText, 22);
            DrawText(colorText,
                     (int)(wheelCenter.x - (float)colorTextWidth * 0.5f),
                     (int)(wheelCenter.y + 36.0f),
                     22,
                     theme.muted);
        } else {
            gui_draw_badge(wheelBadge, "--", Fade(theme.ivory, 0.2f), theme.ivory, 30);
        }

        float resultY = wheelPanel.y + wheelPanel.height + 16.0f;
        Rectangle resultPanel = {
            right.x + 20,
            resultY,
            right.width - 40,
            right.y + right.height - resultY - 20.0f,
        };
        gui_draw_panel(resultPanel, Fade(theme.blackChip, 0.30f), 1.0f);

        DrawText("Latest Result", (int)resultPanel.x + 16, (int)resultPanel.y + 14, 28, theme.gold);

        int detailY = (int)resultPanel.y + 56;
        if (spinning) {
            DrawText("Spinning the wheel...", (int)resultPanel.x + 16, detailY, 24, theme.ivory);
        } else if (latestRecord.spinNumber >= 0) {
            DrawText(TextFormat("Number: %d (%s)", latestRecord.spinNumber, spin_color_text(latestRecord.spinNumber)),
                     (int)resultPanel.x + 16,
                     detailY,
                     24,
                     theme.ivory);

            DrawText(latestRecord.win ? "Outcome: Win" : "Outcome: Loss",
                     (int)resultPanel.x + 16,
                     detailY + 32,
                     24,
                     latestRecord.win ? theme.positive : theme.negative);

            DrawText(TextFormat("Payout: %+$d", latestRecord.payout),
                     (int)resultPanel.x + 16,
                     detailY + 64,
                     22,
                     theme.ivory);
        } else {
            DrawText("No spin yet.", (int)resultPanel.x + 16, detailY, 24, theme.ivory);
        }

        DrawText("Table Status", (int)resultPanel.x + 16, (int)(resultPanel.y + resultPanel.height - 64.0f), 22, theme.muted);
        DrawText(statusMessage, (int)resultPanel.x + 16, (int)(resultPanel.y + resultPanel.height - 36.0f), 22, theme.ivory);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
