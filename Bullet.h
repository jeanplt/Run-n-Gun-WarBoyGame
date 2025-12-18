#ifndef __BULLET__ 																																//Guardas de inclusão
#define __BULLET__																																//Guardas de inclusão

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>			

#define BULLET_MOVE 15

typedef struct {																																//Definição da estrutura de um pŕojétil
	unsigned short x;																															//Local, no eixo x, onde se localiza a bala
	unsigned short y;																															//Local, no eixo y, onde se localiza a bala
	unsigned char trajectory;																													//Trajetória da bala
	unsigned char dir;        // 0 = esquerda, 1 = direita
    bool crouched;            // Para saber se o disparo foi agachado
	float start_x, max_range; // Posição inicial e alcance máximo da bala
	struct bullet *next; 																														//Próxima bala, se houver, formando uma lista encadeada

} bullet;																																		//Definição do nome da estrutura

bullet* bullet_create(unsigned short x, unsigned short y, unsigned char trajectory, bullet *next);												//Protótipo da função de criação de uma bala
void bullet_move(bullet *elements);																												//Protótipo da função de movimentação de uma bala	
void bullet_destroy(bullet *element);																											//Protótipo da função de destruição de uma bala

#endif																																			//Guardas de inclusão