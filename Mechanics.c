#include "Mechanics.h"


unsigned char collision_2D(square *a, square *b) {
    float a_left   = a->x - a->w/2;
    float a_right  = a->x + a->w/2;
    float a_top    = a->y - a->h/2;
    float a_bottom = a->y + a->h/2;

    float b_left   = b->x - b->w/2;
    float b_right  = b->x + b->w/2;
    float b_top    = b->y - b->h/2;
    float b_bottom = b->y + b->h/2;

    return !(a_right < b_left ||
             a_left  > b_right ||
             a_bottom< b_top  ||
             a_top   > b_bottom);
}


unsigned char check_kill(square *killer, square *victim){
    bullet *previous = NULL;
    for (bullet *index = killer->gun->shots; index != NULL; index = (bullet*) index->next){
        float victim_left   = victim->x - victim->w/2;
        float victim_right  = victim->x + victim->w/2;
        float victim_top    = victim->y - victim->h/2;
        float victim_bottom = victim->y + victim->h/2;

        if (index->x >= victim_left && index->x <= victim_right &&
            index->y >= victim_top  && index->y <= victim_bottom){

            victim->hp--;
            if (previous){
                previous->next = index->next;
                bullet_destroy(index);
                index = (bullet*) previous->next;
            }
            else {
                killer->gun->shots = (bullet*) index->next;
                bullet_destroy(index);
                index = killer->gun->shots;
            }

            return victim->hp == 0; // 1 se morreu, 0 se só tomou dano
        }
        previous = index;
    }
    return 0;
}


void update_bullets(square *player){																																										//Implementação da função que atualiza o posicionamento de projéteis conforme o movimento dos mesmos
	
	bullet *previous = NULL;																																												//Variável auxiliar para salvar a posição imediatamente anterior na fila
	for (bullet *index = player->gun->shots; index != NULL;){																																				//Para cada projétil presente na lista de projéteis disparados
		if (!index->trajectory) index->x -= BULLET_MOVE;																																					//Se a trajetória for para a esquerda, atualiza a posição para a esquerda
		else if (index->trajectory == 1) index->x += BULLET_MOVE;																																			//Se a trajetória for para a direita, atualiza a posição para a esquerda
		
		if ((index->x < 0) || (index->x > 56 * TILE_SIZE) || 
		(fabsf(index->x - index->start_x) > index->max_range)){																																						//Verifica se o projétil saiu das bordas da janela
			if (previous){																																													//Verifica se não é o primeiro elemento da lista de projéteis
				previous->next = index->next;																																								//Se não for, salva o próximo projétil
				bullet_destroy(index);																																										//Chama o destrutor para o projétil atual
				index = (bullet*) previous->next;																																							//Atualiza para o próximo projétil
			}
			else {																																															//Se for o primeiro projétil da lista
				player->gun->shots = (bullet*) index->next;																																					//Atualiza o projétil no início da lista
				bullet_destroy(index);																																										//Chama o destrutor para o projétil atual
				index = player->gun->shots;																																									//Atualiza para o próximo projétil
			}
		}
		else{																																																//Se não saiu da tela
			previous = index;																																												//Atualiza o projétil anterior (para a próxima iteração)
			index = (bullet*) index->next;																																									//Atualiza para o próximo projétil
		}
	}
}


void update_position(square *player, square **enemies, int n_enemies) {
	float prev_x, prev_y;

	// Verifica movimentação para esquerda
	if (player->control->left) {
		// guarda posição
		prev_x = player->x;
		prev_y = player->y;

		square_move(player, 1, 0, 56*TILE_SIZE, 7*TILE_SIZE);

		// For para saber se colide com algum inimigo
		for (int i = 0; i < n_enemies; i++) {
			square *e = enemies[i];
			if (e && collision_2D(player, e)) {
				player->x = prev_x;
				player->y = prev_y;
				break;
			}
		}
	}

	// Verifica movimentação para direita
	if (player->control->right) {
		prev_x = player->x;
		prev_y = player->y;

		square_move(player, 1, 1, 56*TILE_SIZE, 7*TILE_SIZE);

		// For para saber se colide com algum inimigo
		for (int i = 0; i < n_enemies; i++) {
			square *e = enemies[i];
			if (e && collision_2D(player, e)) {
				player->x = prev_x;
				player->y = prev_y;
				break;
			}
		}
	}

	// Verifica pulo
	if (player->control->up) {
		prev_x = player->x;
		prev_y = player->y;

		square_move(player, 1, 2, 56*TILE_SIZE, 7*TILE_SIZE);

		for (int i = 0; i < n_enemies; i++) {
				square *e = enemies[i];
				if (e && collision_2D(player, e)) {
					player->x = prev_x;
					player->y = prev_y;
					break;
				}
		}
	}

	// Verifica disparo
	if (player->control->fire) {
		if (!player->gun->timer) {
			square_shot(player);
			player->gun->timer = player->gun->pistol_cooldown;
		}
	}

	// Atualiza balas dele
	update_bullets(player);
}


void check_jump(square *player, float floor) {
	// Diminui o cooldown do pulo até zero
	if (player->jump_cooldown > 0)
	player->jump_cooldown--;

	// Verifica se o pulo está ligado
	if (player->control->up) {
		if (player->onGround && player->jump_cooldown == 0) {
			player->velY         = JUMP_SPEED;
			player->onGround     = false;
			player->jump_cooldown = JUMP_COOLDOWN;
		}
		joystick_up(player->control);  // limpa o XOR‑flag
	}

	// Verifica se está no ar para aplicar gravidade
	if (!player->onGround) {
		player->velY += GRAVITY;
		player->y    += player->velY;

		// Verifica se voltou para o chão
		const float groundY = floor * TILE_SIZE + TILE_SIZE/2;
		if (player->y >= groundY) {
			player->y        = groundY;
			player->velY     = 0;
			player->onGround = true;

			// Verifica se foi pressionado para agachar durante o salto para não bugar o sprite
			if (player->control->down)
			joystick_down(player->control);
		}
	}
}


void check_walk(square *player) {
	// Verifica se o player está andando
	if (player->is_walking) {
		player->step_timer++;
		
		// Verifica o tempo da passada do player para atualizar a animação
		if (player->step_timer >= SQUARE_STEP) {
			player->step_timer = 0;
			player->step_frame++;  
		}
		if(player->step_frame >= 3)
			player->step_frame = 0;
	}
}


void check_crouch(square *player) {
	// Verifica se está agachado
	if (player->control->down) {
		
		// Verifica se está movendo para algum dos lados para poder animar
		if (player->control->left || player->control->right) {
		  player->crouch_step_timer++;
		  
		  if (player->crouch_step_timer >= SQUARE_STEP) {
			player->crouch_step_timer = 0;
			player->crouch_step_frame ^= 1;
		  }
		} else {
		  // Reseta a animação se parou de andar agachado
		  player->crouch_step_frame = 0;
		  player->crouch_step_timer = 0;
		}
	}
}


void check_spams(square *player){
	if(player->onGround && !player->control->down)
		joystick_down(player->control);
}


void turn_and_walk(square *player, unsigned char face) {
	// Muda a direção do player se preciso e confirma sua movimentação
	player->face       = face;
	player->is_walking = true;
}


void stop_move(square *player) {
	player->is_walking = false;
	player->step_frame = 0;
	player->step_timer = 0;
}


void update_enemy_ai(square *e, square *player, unsigned short max_x, unsigned short max_y) {
	float dx = player->x - e->x;
    float dy = player->y - e->y;
	unsigned int max_enemy_vision = (e->type == 3) ? TILE_SIZE * 4 : TILE_SIZE * 3;
    // Verifica se encontrou o player para disparar
    if (fabsf(dx) < max_enemy_vision && fabsf(dy) < TILE_SIZE * 2) {
		e->control->left = e->control->right = 0;
		e->is_walking = false;
		e->step_frame = 0;
	
		if (e->type == 3) {
			// Se for o boss, forçar o agachado enquanto vê o player
			e->control->down = 1;
		} else {
			// Para outros inimigos, ficar em pé
			e->control->down = 0;
		}
	
		if (e->gun->timer == 0) {
			square_shot(e);
			e->gun->timer = e->gun->pistol_cooldown;
		}
	}	
    else {
		// Sai do modo agachado ao voltar à patrulha (pro Boss)
        e->control->down = 0;

        // Busca padrão do inimigo
        float leftLimit  = e->ai->spawnX - e->ai->patrolRange;
        float rightLimit = e->ai->spawnX + e->ai->patrolRange;

		// Verifica qual será a direção da busca
        if (e->x <= leftLimit  && e->ai->patrolDir < 0) e->ai->patrolDir = +1;
        if (e->x >= rightLimit && e->ai->patrolDir > 0) e->ai->patrolDir = -1;

		// Valores para movimentação do inimigo
        e->is_walking = true;

        if (e->ai->patrolDir > 0) {
            e->control->right = 1;
            e->control->left  = 0;
            e->face = 1;
        } else {
            e->control->left  = 1;
            e->control->right = 0;
            e->face = 0;
        }

        square_move(e, 1, e->ai->patrolDir > 0 ? 1 : 0, max_x, max_y);
        check_walk(e);
    }

    if (e->gun->timer > 0) e->gun->timer--;
    update_bullets(e);
}

void update_diff(square **enemies, int n_enemies, Difficulty diff) {
	unsigned short diff_multiplier;
	
	// Verifica qual será o mutiplicador de dificuldade
	if(diff == DIFF_EASY) 
		diff_multiplier = 2;
	else if(diff == DIFF_NORMAL) 
		diff_multiplier = 4;
	else 
		diff_multiplier = 6;
	
	// For para aplicar efeitos da dificuldade nos inimigos
	for (int i = 0; i < n_enemies; i++) {
		square *e = enemies[i];
		if (e) {
			e->max_vision *= diff_multiplier;
			e->ai->patrolRange *= (diff_multiplier == 2 ? 2 : diff_multiplier/2);
			
			// Verifica se é o boss ou um inimigo normal
			if (e->type == 3) {
				e->gun->pistol_cooldown = (4*PISTOL_COOLDOWN) / diff_multiplier;
				e->hp += diff_multiplier;
			} else {
				e->gun->pistol_cooldown = (6*PISTOL_COOLDOWN) / diff_multiplier;
				e->hp += diff_multiplier/2;
			}
		}
	}
}