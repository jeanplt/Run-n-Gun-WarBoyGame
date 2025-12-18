#ifndef __MECHANICS__ 																																	//Guardas de inclusão
#define __MECHANICS__	

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "Square.h"
#include "Square.h"
#include "Pistol.h"
#include "Designs.h"

#define TILE_SIZE 128             // tamanho dos chunks do mapa
#define SCREEN_W    X_SCREEN       // largura da janela
#define SCREEN_H    Y_SCREEN       // altura da janela

// Constantes para controlar gravidade
#define GRAVITY 0.5  
#define JUMP_SPEED  -12.0 
#define  JUMP_COOLDOWN  15 

// Constante para número de inimigos
#define MAX_ENEMIES 7

// Struct para ditar o estado do jogo
typedef enum {
    STATE_MENU,
    STATE_SELECT_DIFF,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_VICTORY,
    STATE_GAMEOVER
} GameState;	

// Struct para selecionar a dificuldade
typedef enum {
    DIFF_EASY,
    DIFF_NORMAL,
    DIFF_HARD,
} Difficulty;

unsigned char collision_2D(square *a, square *b);
unsigned char check_kill(square *killer, square *victim);
void update_bullets(square *player);
void update_position(square *player, square **enemies, int n_enemies);
void check_jump(square *player, float floor);
void check_walk(square *player);
void check_crouch(square *player);
void check_spams(square *player);
void turn_and_walk(square *player, unsigned char face);
void stop_move(square *player);
void update_enemy_ai(square *e, square *player, unsigned short max_x, unsigned short max_y);
void update_diff(square **enemies, int n_enemies, Difficulty diff);

#endif