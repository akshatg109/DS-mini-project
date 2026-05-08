#include "gui.h"

#include <math.h>

Theme gui_make_theme(void) {
    Theme theme;
    theme.backgroundTop = (Color){6, 17, 10, 255};
    theme.backgroundBottom = (Color){14, 42, 24, 255};
    theme.felt = (Color){18, 77, 45, 255};
    theme.feltDark = (Color){13, 56, 33, 255};
    theme.gold = (Color){234, 190, 93, 255};
    theme.ivory = (Color){248, 241, 221, 255};
    theme.muted = (Color){188, 177, 148, 255};
    theme.redChip = (Color){181, 56, 45, 255};
    theme.blackChip = (Color){25, 25, 30, 255};
    theme.positive = (Color){109, 217, 149, 255};
    theme.negative = (Color){242, 113, 98, 255};
    return theme;
}

void gui_draw_background(int width, int height, Theme theme) {
    DrawRectangleGradientV(0, 0, width, height, theme.backgroundTop, theme.backgroundBottom);

    for (int y = 0; y < height; y += 34) {
        float t = (float)y / (float)height;
        int alpha = (int)(24.0f * (0.2f + 0.8f * sinf(t * 4.0f)));
        DrawLine(0, y, width, y, Fade(theme.gold, (float)alpha / 255.0f));
    }
}

void gui_draw_panel(Rectangle panel, Color color, float alpha) {
    DrawRectangleRounded(panel, 0.08f, 12, Fade(color, alpha));
    DrawRectangleRoundedLinesEx(panel, 0.08f, 12, 2.0f, Fade(RAYWHITE, 0.09f));
}

bool gui_button(Rectangle bounds, const char *text, int fontSize, Theme theme) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, bounds);
    bool pressed = hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color fill = hover ? Fade(theme.gold, 0.95f) : Fade(theme.gold, 0.80f);
    Color textColor = hover ? (Color){18, 18, 21, 255} : (Color){35, 35, 38, 255};

    DrawRectangleRounded(bounds, 0.25f, 12, fill);
    DrawRectangleRoundedLinesEx(bounds, 0.25f, 12, 2.0f, Fade(theme.ivory, 0.5f));

    int width = MeasureText(text, fontSize);
    DrawText(text,
             (int)(bounds.x + bounds.width * 0.5f - width * 0.5f),
             (int)(bounds.y + bounds.height * 0.5f - fontSize * 0.5f),
             fontSize,
             textColor);
    return pressed;
}

void gui_draw_badge(Rectangle bounds, const char *text, Color fill, Color textColor, int fontSize) {
    DrawRectangleRounded(bounds, 0.35f, 10, fill);
    DrawRectangleRoundedLinesEx(bounds, 0.35f, 10, 2.0f, Fade(RAYWHITE, 0.22f));
    int width = MeasureText(text, fontSize);
    DrawText(text,
             (int)(bounds.x + bounds.width * 0.5f - width * 0.5f),
             (int)(bounds.y + bounds.height * 0.5f - fontSize * 0.5f),
             fontSize,
             textColor);
}

void gui_draw_title_center(const char *title, int y, int fontSize, Color color) {
    int width = MeasureText(title, fontSize);
    int x = GetScreenWidth() / 2 - width / 2;
    DrawText(title, x, y, fontSize, color);
}
