#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

#include <raylib.h>

typedef struct {
    Color backgroundTop;
    Color backgroundBottom;
    Color felt;
    Color feltDark;
    Color gold;
    Color ivory;
    Color muted;
    Color redChip;
    Color blackChip;
    Color positive;
    Color negative;
} Theme;

Theme gui_make_theme(void);
void gui_draw_background(int width, int height, Theme theme);
void gui_draw_panel(Rectangle panel, Color color, float alpha);
bool gui_button(Rectangle bounds, const char *text, int fontSize, Theme theme);
void gui_draw_badge(Rectangle bounds, const char *text, Color fill, Color textColor, int fontSize);
void gui_draw_title_center(const char *title, int y, int fontSize, Color color);

#endif
