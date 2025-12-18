#include "Square.h"
#include "Designs.h"
#include "Mechanics.h"

square* square_create(unsigned short w, unsigned short h,
	unsigned char face,
	float x, float y,
	unsigned short max_x, unsigned short max_y, unsigned short ID)
{
	// Checa limites:
	if (x - w/2 < 0 || x + w/2 > max_x || y - h/2 < 0 || y + h/2 > max_y) return NULL;
	if (face > 3) return NULL;
	if (ID > 3 || ID < 1) return NULL;

	square *s = malloc(sizeof(*s));
	if (!s) return NULL;

	// Carrega coordenadas absolutas no mapa do personagem e sua identificação
	s->x = x;
	s->y = y;
	s->type = ID;

	// Verifica o ID do personagem para saber se ele é o player (ID == 1), inimigo genérico (ID == 2) ou boss (ID == 3)
	if (ID == 1) {
		s->sprite_idle = loadSprite("Sprites/Warboy/War boy parado.png");
    	s->sprite_walk[0] = loadSprite("Sprites/Warboy/War boy andando1.png");
    	s->sprite_walk[1] = loadSprite("Sprites/Warboy/War boy andando2.png");
    	s->sprite_walk[2] = loadSprite("Sprites/Warboy/War boy andando3.png");
    	s->sprite_jump[0] = loadSprite("Sprites/Warboy/War boy pulando1.png");
    	s->sprite_jump[1] = loadSprite("Sprites/Warboy/War boy pulando2.png");
    	s->sprite_jump[2] = loadSprite("Sprites/Warboy/War boy pulando3.png");
    	s->sprite_crouch[0] = loadSprite("Sprites/Warboy/War boy agachado1.png");
    	s->sprite_crouch[1] = loadSprite("Sprites/Warboy/War boy agachado2.png");
		s->max_vision = PLAYER_RANGE * TILE_SIZE;
		s->hp = HEALTH_POINTS;
		s->ai = NULL;
	} else if (ID == 2) {
		s->sprite_idle = loadSprite("Sprites/Ninja/Ninja parado.png");
    	s->sprite_walk[0] = loadSprite("Sprites/Ninja/Ninja andando1.png");
    	s->sprite_walk[1] = loadSprite("Sprites/Ninja/Ninja andando2.png");
    	s->sprite_walk[2] = loadSprite("Sprites/Ninja/Ninja andando3.png");
    	s->sprite_jump[0] = NULL; // Sprite não desenhado
    	s->sprite_jump[1] = NULL; // Sprite não desenhado
    	s->sprite_jump[2] = NULL; // Sprite não desenhado
    	s->sprite_crouch[0] = loadSprite("Sprites/Ninja/Ninja agachado.png");
    	s->sprite_crouch[1] = loadSprite("Sprites/Ninja/Ninja deitado.png");
		s->max_vision = TILE_SIZE;
		s->hp = (HEALTH_POINTS/2) - 2;
		s->ai = malloc(sizeof(*s->ai));
     	s->ai->spawnX = s->x;
		s->ai->patrolDir = +1;
		s->ai->patrolRange = TILE_SIZE;
	} else {
		s->sprite_idle = loadSprite("Sprites/Boss/Boss parado.png");
    	s->sprite_walk[0] = loadSprite("Sprites/Boss/Boss andando1.png");
    	s->sprite_walk[1] = loadSprite("Sprites/Boss/Boss andando2.png");
    	s->sprite_walk[2] = loadSprite("Sprites/Boss/Boss andando3.png");
    	s->sprite_jump[0] = loadSprite("Sprites/Boss/Boss pulando1.png");
    	s->sprite_jump[1] = loadSprite("Sprites/Boss/Boss pulando2.png");
    	s->sprite_jump[2] = loadSprite("Sprites/Boss/Boss pulando3.png");
    	s->sprite_crouch[0] = loadSprite("Sprites/Boss/Boss agachado.png");
    	s->sprite_crouch[1] = NULL; // Sprite não desenhado
		s->max_vision = TILE_SIZE;
		s->hp = HEALTH_POINTS - 2;
		s->ai = malloc(sizeof(*s->ai));
		s->ai->spawnX = s->x;
		s->ai->patrolDir = +1;
		s->ai->patrolRange = TILE_SIZE;
	}

	// Carrega os tamanhos, formas e vida do personagem
	s->w = w;
	s->h = h;
	s->face = face;

	// Carrega valores para definir o personagem como agachado quando preciso
	s->orig_h = h;
	s->crouch_h = h * 3 / 4;

	// Variáveis de intervalo para disparo, andar, pulo e andar agachado
	s->flash_timer = 0;
	s->is_walking = false;
	s->step_frame = 0;
	s->step_timer = 0;
	s->velY = 0;
	s->onGround = true;
	s->jump_cooldown = 0;
	s->crouch_step_frame = 0;
	s->crouch_step_timer = 0;

	// Cria os controles e arma para o personagem
	s->control = joystick_create();
	s->gun     = pistol_create();
	return s;
}


void square_move(square *e, char steps, unsigned char traj,
	unsigned short max_x, unsigned short max_y)
{
	if (traj == 0) { // esquerda
		float nx = e->x - steps * SQUARE_STEP;
		if (nx - e->w/2 >= 0) e->x = nx;
	}
	else if (traj == 1) { // direita
		float nx = e->x + steps * SQUARE_STEP;
		if (nx + e->w/2 <= max_x) e->x = nx;
	}
	else if (traj == 2) { // cima
		float ny = e->y - steps * SQUARE_STEP;
		if (ny - e->h/2 >= 0) e->y = ny;
	}
	else if (traj == 3) { // baixo
		float ny = e->y + steps * SQUARE_STEP;
		if (ny + e->h/2 <= max_y) e->y = ny;
	}
}


void square_shot(square *e) {
    bullet *shot = NULL;
	e->flash_timer = 3;   // mostra o efeito por 3 frames 
    if (e->face == 0) {
        // dispara da borda esquerda
        shot = pistol_shot(e->x - e->w/2, e->y, e->face, e->gun);
    }
    else if (e->face == 1) {
        // dispara da borda direita
        shot = pistol_shot(e->x + e->w/2, e->y, e->face, e->gun);
    }
	// Verifica se o disparo foi feito para guardar campos do disparo como origem, direção e alcance
    if (shot) {
		shot->dir      = e->face;                  
    	shot->crouched = e->control->down;         
		shot->start_x  = e->x;                    
		shot->max_range = e->max_vision + (2*TILE_SIZE);
    	e->gun->shots  = shot;
	}
}


void square_destroy(square *element){																													//Implementação da função "square_destroy"
	
	pistol_destroy(element->gun);																														//Destrói o armemento do quadrado
	joystick_destroy(element->control);																													//Destrói o controle do quadrado
	
	// Libera todos os Sprites do personagem
	al_destroy_bitmap(element->sprite_idle);
	al_destroy_bitmap(element->sprite_walk[0]);
	al_destroy_bitmap(element->sprite_walk[1]);
	al_destroy_bitmap(element->sprite_walk[2]);
	al_destroy_bitmap(element->sprite_jump[0]);
	al_destroy_bitmap(element->sprite_jump[1]);
	al_destroy_bitmap(element->sprite_jump[2]);
	al_destroy_bitmap(element->sprite_crouch[0]);
	al_destroy_bitmap(element->sprite_crouch[1]);

	free(element->ai);

	free(element);																																		//Libera a memória do quadrado na heap
}
