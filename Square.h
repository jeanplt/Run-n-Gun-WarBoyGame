#ifndef __SQUARE__ 																																	//Guardas de inclusão
#define __SQUARE__																																	//Guardas de inclusão																															//Quantidade de pixels que um quadrado se move por passo

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>			
#include <stdbool.h>																																						//Biblioteca de figuras básicas
#include <stdio.h>
#include <stdlib.h>
#include "Joystick.h"																																//Estrutura e procedimentos relacionados ao controle do quadrado
#include "Pistol.h" 																															//Estrutura e procedimentos relacionados ao controle da arma (pistola) no jogo

#define SQUARE_STEP 5																																//Tamanho, em pixels, de um passo do quadrado
#define PLAYER_RANGE 4
#define HEALTH_POINTS 6

// Struct exclusiva para inimigos
typedef struct EnemyAI {
    float spawnX;        // X inicial, em pixels
	float patrolRange;   // Alcance da busca
	int   patrolDir;     // +1 ou –1, direção atual da busca
} EnemyAI;

typedef struct {																																	//Definição da estrutura de um quadrado
	unsigned short w, h;     // Largura e altura, em pixels																															//Tamanmho da lateral de um quadrado
	unsigned char face;																																//A face principal do quadrado, algo como a sua "frente"
	unsigned char hp;																															//Quantidade de vida do quadrado, em unidades (!)
	float x, y;																																//Posição X do centro do quadrado
	unsigned short orig_h, crouch_h;

	bool is_walking;    // está se movimentando?
    int  step_frame;    // 0 ou 1: qual sprite de caminhada
    int  step_timer;    // contador de quadros até trocar de frame

	float velY;         // velocidade vertical atual
	bool  onGround;     // está colado ao chão?
	int   jump_cooldown;// contador para evitar spam de pulo

	int flash_timer; // Disparo
	int max_vision; // Distância máxima de disparo
	int type;		// ID

	// Sprites existentes para o personagem
    ALLEGRO_BITMAP *sprite_idle;
    ALLEGRO_BITMAP *sprite_walk[3];
	ALLEGRO_BITMAP *sprite_jump[3];
	ALLEGRO_BITMAP *sprite_crouch[2];

	// Para animação de agachado andando
  	int crouch_step_frame;
  	int crouch_step_timer;

	joystick *control;																																//Elemento de controle do quadrado no jogo
	pistol *gun;																																	//Elemento para realizar disparos no jogo
	EnemyAI *ai;
} square;																																			//Definição do nome da estrutura

square* square_create(unsigned short w, unsigned short h, unsigned char face, float x, float y, unsigned short max_x, unsigned short max_y, unsigned short ID);		//Protótipo da função de criação de um quadrado
void square_move(square *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y);								//Protótipo da função de movimentação de um quadrado
void square_shot(square *element);																													//Protótipo da função de disparo de um quadrado
void square_destroy(square *element);																												//Protótipo da função de destruição de um quadrado

#endif																																				//Guardas de inclusão