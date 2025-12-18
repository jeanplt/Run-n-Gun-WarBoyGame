#include "Menu.h"

void draw_vertical_gradient(int width, int height) {
    // Cor inicial (topo)
    int r1 = 61, g1 = 110, b1 = 112;
    // Cor final (base)
    int r2 = 75, g2 = 91, b2 = 171;

    // For para desenhar o gradiente do Menu
    for (int y = 0; y < height; y++) {
        float t = (float)y / (height - 1);  // t varia de 0.0 até 1.0

        int r = (int)(r1 * (1 - t) + r2 * t);
        int g = (int)(g1 * (1 - t) + g2 * t);
        int b = (int)(b1 * (1 - t) + b2 * t);

        ALLEGRO_COLOR color = al_map_rgb(r, g, b);
        al_draw_filled_rectangle(0, y, width, y + 1, color);
    }
}


int is_inside_button(Button btn, float mx, float my) {
    return (mx >= btn.x && mx <= btn.x + btn.w && my >= btn.y && my <= btn.y + btn.h);
}

void draw_button(ALLEGRO_FONT* font, Button btn) {
    al_draw_filled_rectangle(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h, al_map_rgb(100, 100, 255));
    al_draw_rectangle(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h, al_map_rgb(255, 255, 255), 2);
    int text_width = al_get_text_width(font, btn.label);
    int text_height = al_get_font_line_height(font);
    al_draw_text(font, al_map_rgb(255, 255, 255), btn.x + btn.w/2 - text_width/2, btn.y + btn.h/2 - text_height/2, 0, btn.label);
}
