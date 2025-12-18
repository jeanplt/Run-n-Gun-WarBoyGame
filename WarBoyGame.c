#include "Mechanics.h"
#include "Menu.h"																																									//Inclusão da biblioteca de quadrados

#define SPRITE_FLOOR 1.43

int main(){
	// Inicialização básica do Allegro
	al_init();																																																
	al_init_primitives_addon();		
	al_init_font_addon();																																										
	al_install_keyboard();
	al_install_mouse();		
	al_init_ttf_addon();
	al_init_image_addon();

	bool redraw = false;

	const char* font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";
	ALLEGRO_FONT* font_title = al_load_ttf_font(font_path, Y_SCREEN/20, 0);
	ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);																																						//Cria o relógio do jogo; isso indica quantas atualizações serão realizadas por segundo (30, neste caso)
	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();																																						//Cria uma janela para o programa, define a largura (x) e a altura (y) da tela em píxeis (320x320, neste caso)
	ALLEGRO_DISPLAY* disp = al_create_display(X_SCREEN, Y_SCREEN);	

	al_register_event_source(queue, al_get_keyboard_event_source());	
	al_register_event_source(queue, al_get_mouse_event_source());																																	//Indica que eventos de teclado serão inseridos na nossa fila de eventos
	al_register_event_source(queue, al_get_display_event_source(disp));																																		//Indica que eventos de tela serão inseridos na nossa fila de eventos
	al_register_event_source(queue, al_get_timer_event_source(timer));																																		//Indica que eventos de relógio serão inseridos na nossa fila de eventos

	// Definindo dimensões dos botões
    float btn_w = X_SCREEN * 0.4f;
	float btn_h = Y_SCREEN * 0.08f;
	float btn_x = X_SCREEN / 2 - btn_w / 2;
	float btn_spacing = 20; 

	float menu_start_y = Y_SCREEN / 2 - (btn_h * 3 + btn_spacing * 2) / 2;

	// Botões do Menu
	Button btn_novo =     {btn_x, menu_start_y, btn_w, btn_h, "Novo Jogo"};
	Button btn_sair =     {btn_x, menu_start_y + 1 * (btn_h + btn_spacing), btn_w, btn_h, "Sair"};

	// Botões da dificuldade
	Button btn_facil =     {btn_x, menu_start_y, btn_w, btn_h, "Fácil"};
	Button btn_normal = {btn_x, menu_start_y + btn_h + btn_spacing, btn_w, btn_h, "Normal"};
	Button btn_dificil =     {btn_x, menu_start_y + 2 * (btn_h + btn_spacing), btn_w, btn_h, "Difícil"};

	// Definindo mapa do jogo
	int **map = NULL;
    int mapSizeX = 0, mapSizeY = 0;

	if (!loadMap("Map.txt", &map, &mapSizeX, &mapSizeY)) {
        al_show_native_message_box(disp, "Erro", "Não carregou o mapa", " ", NULL, 0);
        return 1;
    }

	ALLEGRO_BITMAP *bmp_background = loadSprite("Sprites/Map/Cenario.png");
	ALLEGRO_BITMAP* bmp_floor = loadSprite("Sprites/Map/Piso.png");
	ALLEGRO_BITMAP* bmp_rocks = loadSprite("Sprites/Map/Rochas.png");
	ALLEGRO_BITMAP* bmp_barricade = loadSprite("Sprites/Map/Barreira.png");
	
	// Variável para controlar a câmera do player
	float camera_x = 0;

	// Coordenadas de início do player no mapa, sendo starRow o piso
	int startCol = 1.5; 
	float startRow = 3.5;

	// "Chão" para encaixar sprites dos players na hora de desenhá-las
	float playerGround = startRow * (SPRITE_FLOOR * TILE_SIZE);

	// Struct com dimensões do mapa para poder gerar o background e futuramente aplicar efeitos
    MapContext mapCtx = createMapContext(bmp_background, startRow);
	
	// Vetor de inimigos que terão ao longo do mapa
	square* enemies[MAX_ENEMIES];
	int n_enemies = 0;

	// Cria o player
	square* player_1 = square_create(
    	(unsigned short)TILE_SIZE,      							// largura = largura do tile
    	(unsigned short)TILE_SIZE*2,    							// altura  = altura do tile
    	1,                              							// face
    	(unsigned short)(startCol*TILE_SIZE + TILE_SIZE/2),			// Coordenada central X 
    	(unsigned short)(startRow*TILE_SIZE + TILE_SIZE/2),			// Coordenada central Y
    	mapSizeX * TILE_SIZE,           							// max mundo em X
    	mapSizeY * TILE_SIZE,           							// max mundo em Y
		1);															// ID do personagem
	if (!player_1) {
		al_show_native_message_box(disp, "Erro", "Não gerou o player", " ", NULL, 0);
		return 1;
	}

	// Laço para gerar os inimigos no mapa com base em suas coordenadas
	for (int i = 0; i < mapSizeX; i++) {
    	for (int j = 0; j < mapSizeY; j++) {
        	int id = map[i][j];
        	if ((id == 3 || id == 4) && n_enemies < MAX_ENEMIES) {
            	float cx = i * TILE_SIZE + TILE_SIZE/2;
            	float cy = (unsigned short)(startRow*TILE_SIZE + TILE_SIZE/2);
            	unsigned char face = 0;    
            	unsigned char type = id-1;   
            	
				square *e = square_create(
					TILE_SIZE, TILE_SIZE*2,
					face,
					(unsigned short)cx, (unsigned short)cy,
					mapSizeX * TILE_SIZE,
					mapSizeY * TILE_SIZE,
					type
				);
				// Verifica se conseguiu criar o inimigo
				if (!e) {
					al_show_native_message_box( disp, "Erro", "Falha ao criar um inimigo", " ", NULL, 0);
					return 1;
				}
				enemies[n_enemies++] = e;
        	}
    	}
	}
	
	// Sprites para disparo e flash
	ALLEGRO_BITMAP *bmp_bullet = loadSprite("Sprites/Bullets/Disparo.png");
	ALLEGRO_BITMAP *bmp_muzzle = loadSprite("Sprites/Bullets/Flash.png");
	ALLEGRO_BITMAP *bmp_boss_bullet = loadSprite("Sprites/Bullets/Super disparo.png");

    // Variáveis de Estado para o jogo e sua dificuldade
    GameState state = STATE_MENU;
	Difficulty diff = DIFF_NORMAL;
    bool running = true;

    ALLEGRO_EVENT event;
	al_start_timer(timer);
	
	// Loop do jogo
    while (running) {
		al_wait_for_event(queue, &event);
		// Alterna os tipos de eventos do jogo
		switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				// Checagens de movimentação para poder animar e atualizar valores do player
				check_jump(player_1, startRow);
				check_walk(player_1);
				check_crouch(player_1);

				// Laço para atualizar inimigos
				for (int i = 0; i < n_enemies; i++) {
					if (enemies[i]) {
						update_enemy_ai(enemies[i], player_1, mapSizeX * TILE_SIZE, mapSizeY * TILE_SIZE);
					}
				}

				redraw = true;
				break;
	
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				running = false;
				break;
	
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				float mx = event.mouse.x;
				float my = event.mouse.y;
				
				// Verifica estado do mouse para saber qual ação de botão tomar
				if (state == STATE_MENU) {
					if (is_inside_button(btn_novo, mx, my)) {
						state = STATE_SELECT_DIFF;
					}
					else if (is_inside_button(btn_sair, mx, my)) {
						running = false;
					}
				} else if (state == STATE_SELECT_DIFF) {
					if (is_inside_button(btn_facil,   mx, my)) diff = DIFF_EASY;
					else if (is_inside_button(btn_normal, mx, my)) diff = DIFF_NORMAL;
					else if (is_inside_button(btn_dificil,   mx, my)) diff = DIFF_HARD;
					else break;
					update_diff(enemies, n_enemies, diff); // Atualiza os inimigos com base na dificuldade selecionada
					state = STATE_PLAYING;
				}
				break;

			case ALLEGRO_EVENT_KEY_DOWN:
				switch (event.keyboard.keycode) {
					case ALLEGRO_KEY_ESCAPE: // Para fechar o jogo
            			if (state == STATE_GAMEOVER || state == STATE_VICTORY) 
                			running = false;
            			break;
					case ALLEGRO_KEY_W:
  						joystick_up(player_1->control);		// Liga o up
  						break;
					case ALLEGRO_KEY_S:
						check_spams(player_1);   // Liga o down
						break;
					case ALLEGRO_KEY_A:
						joystick_left(player_1->control);   // Liga o left
						turn_and_walk(player_1, 0);
						break;
					case ALLEGRO_KEY_D:
						joystick_right(player_1->control);  // Liga o right
						turn_and_walk(player_1, 1);
						break;
					case ALLEGRO_KEY_C:
						joystick_fire(player_1->control);
						break;
				}
				break; 
			
			case ALLEGRO_EVENT_KEY_UP:
				switch (event.keyboard.keycode) {
					case ALLEGRO_KEY_W:
						joystick_up(player_1->control);    // Desliga o up
						break;
					case ALLEGRO_KEY_S:
						joystick_down(player_1->control);  // Desliga o down
						break;
					case ALLEGRO_KEY_A:
						joystick_left(player_1->control);  // Desliga o left
						stop_move(player_1);
						break;
					case ALLEGRO_KEY_D:
						joystick_right(player_1->control); // Desliga o right
						stop_move(player_1);
						break;
					case ALLEGRO_KEY_C:
						joystick_fire(player_1->control);
						break;
				}
				break;
		}
		
		
		// Verifica o estado atual do jogo para desenhar na tela
		if (redraw && al_is_event_queue_empty(queue)) {
			redraw = false;
			
			// Verifica se o programa está no Menu, Seleção de Dificuldade, Game Over, Vitória ou Jogando
			if (state == STATE_MENU) {
				draw_vertical_gradient(X_SCREEN, Y_SCREEN);
				al_draw_text(font_title, al_map_rgb(255, 255, 255), X_SCREEN/2, Y_SCREEN/8, ALLEGRO_ALIGN_CENTER, "War Boy: Ninja Edition");
				draw_button(font_title, btn_novo);
				draw_button(font_title, btn_sair);
				al_flip_display();
                continue;
			} else if (state == STATE_SELECT_DIFF) {
				draw_vertical_gradient(X_SCREEN, Y_SCREEN);
				al_draw_text(font_title, al_map_rgb(255, 255, 255), X_SCREEN/2, Y_SCREEN/8, ALLEGRO_ALIGN_CENTER, "Selecione dificuldade");
				draw_button(font_title, btn_facil);
				draw_button(font_title, btn_normal);
				draw_button(font_title, btn_dificil);
				al_flip_display();
				continue;
			} else if (state == STATE_GAMEOVER) {
				al_clear_to_color(al_map_rgb(0,0,0));
				al_draw_text(font_title, al_map_rgb(255,0,0),
							 X_SCREEN/2, Y_SCREEN/2 - 20, ALLEGRO_ALIGN_CENTER, "GAME OVER!");
				al_draw_text(font,      al_map_rgb(200,200,200),
							 X_SCREEN/2, Y_SCREEN/2 + 40, ALLEGRO_ALIGN_CENTER, "Pressione ESC para sair");
				al_flip_display();
				continue;
			} else if (state == STATE_VICTORY) {
				al_clear_to_color(al_map_rgb(0,0,0));
				al_draw_text(font_title, al_map_rgb(0,255,0),
							 X_SCREEN/2, Y_SCREEN/2 - 20, ALLEGRO_ALIGN_CENTER, "VOCÊ VENCEU!");
				al_draw_text(font,       al_map_rgb(200,200,200),
							 X_SCREEN/2, Y_SCREEN/2 + 40, ALLEGRO_ALIGN_CENTER, "Pressione ESC para sair");
				al_flip_display();
				continue;
			} else if (state == STATE_PLAYING) {
				
				// Laços para detectar inimigos derrotados (incluindo boss) ou então o player
            	bool boss_dead = false;
            	for (int i = 0; i < n_enemies; i++) {
                	square *e = enemies[i];
                	if (!e) continue;
                	if (check_kill(player_1, e)) {
                    	if (e->type == 3) boss_dead = true;
                    	square_destroy(e);
                    	enemies[i] = NULL;
                	}
            	}
				for (int i = 0; i < n_enemies; i++) {
					square *e = enemies[i];
					if (!e) continue;
					if ( check_kill(e, player_1) ) {
						if (player_1->hp == 0) {
							state = STATE_GAMEOVER;
						}
					}
				}
				
				// Verifica se o boss ou o player foram derrotados para mudar o estado do jogo 
            	if (boss_dead) {
                	state = STATE_VICTORY;
                	continue;  
            	}
            	if (player_1->hp == 0) {
                	state = STATE_GAMEOVER;
                	continue;
            	}

				// Calcula camera_x
				float target_cam = player_1->x - X_SCREEN/3.0f;
				if (target_cam < 0) target_cam = 0;
				float max_cam = mapSizeX * TILE_SIZE - X_SCREEN;
				if (target_cam > max_cam) target_cam = max_cam;
				camera_x = target_cam;
				
				// Atualiza posições de objetos do jogo e aplica o efeito do cenário de fundo
				update_position(player_1, enemies, n_enemies);

				// Ativa camera transform
				ALLEGRO_TRANSFORM old, cam;
				al_copy_transform(&old, al_get_current_transform());
				al_identity_transform(&cam);
				al_translate_transform(&cam, -camera_x, 0);
				al_use_transform(&cam);
            	update_bullets(player_1);


            	// Desenha todos os elementos no jogo
            	al_clear_to_color(al_map_rgb(0,0,0));
            	draw_map(map, mapSizeX, mapSizeY, &mapCtx, bmp_barricade, bmp_floor, bmp_rocks);
            	draw_player(player_1, playerGround);
            	draw_shots(player_1, bmp_muzzle, bmp_bullet, bmp_boss_bullet);

            	for (int i = 0; i < n_enemies; i++) {
                	if (enemies[i]) {
                    	draw_player(enemies[i], playerGround);
                    	draw_shots(enemies[i], bmp_muzzle, bmp_bullet, bmp_boss_bullet);
                	}
            	}

				// Restaura transform para desenhar UI fixa
				al_use_transform(&old);
            	al_flip_display();
        	}
		}
	}

	// Limpeza de recursos 
    if (map) {
        for (int i = 0; i < mapSizeX; i++) {
            free(map[i]);
        }
        free(map);
    }

	for (int i = 0; i < n_enemies; i++) {
		if (enemies[i])
			square_destroy(enemies[i]);
	}

    square_destroy(player_1);

    al_destroy_display(disp);
    al_destroy_timer(timer);
	al_destroy_font(font_title);
	al_destroy_font(font);
    al_destroy_event_queue(queue);

	al_destroy_bitmap(bmp_background);
	al_destroy_bitmap(bmp_barricade);
	al_destroy_bitmap(bmp_floor);
	al_destroy_bitmap(bmp_rocks);
	al_destroy_bitmap(bmp_bullet);
	al_destroy_bitmap(bmp_muzzle);

	return 0;
}