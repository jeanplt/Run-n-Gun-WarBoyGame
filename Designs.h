#ifndef __DESIGNS__ 																																	//Guardas de inclusão
#define __DESIGNS__	

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "Square.h"
#include "Designs.h"
#include "Mechanics.h"
#include "Menu.h"

#define BACKGROUND_ALIGN 1.67

// Struct para atributos do mapa com background
typedef struct {
    ALLEGRO_BITMAP *background;
    float drawW, drawH;
    float y0;
} MapContext;

MapContext createMapContext(ALLEGRO_BITMAP* background, int startRow);
ALLEGRO_BITMAP* loadSprite(const char *filename);
int loadMap(const char *filename, int ***outMap, int *outWidth, int *outHeight);
void draw_map(int **map, int mapSizeX, int mapSizeY, MapContext *ctx, ALLEGRO_BITMAP* barricade, ALLEGRO_BITMAP* floor, ALLEGRO_BITMAP* rocks);
void draw_player(square *player, float playerGround);
void draw_shots(square *player, ALLEGRO_BITMAP* bmp_muzzle, ALLEGRO_BITMAP* bmp_bullet, ALLEGRO_BITMAP* bmp_boss_bullet);

#endif