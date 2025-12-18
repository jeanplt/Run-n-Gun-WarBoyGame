#ifndef __MENU__ 																																	//Guardas de inclusão
#define __MENU__	

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

#define X_SCREEN 1280
#define Y_SCREEN 780

typedef struct {
    float x, y, w, h;
    const char* label;
} Button;

void draw_vertical_gradient(int width, int height);
int is_inside_button(Button btn, float mx, float my);
void draw_button(ALLEGRO_FONT* font, Button btn);

#endif