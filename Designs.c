#include "Designs.h"

MapContext createMapContext(ALLEGRO_BITMAP* background, int startRow) {
    MapContext ctx;
    ctx.background = background;

    // Dimensões originais do cenário
    float bg_w = al_get_bitmap_width(background);
    float bg_h = al_get_bitmap_height(background);

    // Escala para preencher verticalmente
    float scaleY = (float)Y_SCREEN / bg_h;
    ctx.drawW = bg_w * scaleY;
    ctx.drawH = bg_h * scaleY;

    // Calcula onde começa o topo do background
    float groundY = startRow * (BACKGROUND_ALIGN * TILE_SIZE);
    ctx.y0 = groundY - ctx.drawH;

    return ctx;
}


ALLEGRO_BITMAP* loadSprite(const char *filename) {
    ALLEGRO_BITMAP *bmp = al_load_bitmap(filename);

    // Verifica se conseguiu carregar o Sprite
    if (!bmp) {
        fprintf(stderr, "Erro ao carregar sprite: %s\n", filename);
        exit(1);
    }
    return bmp;
}


int loadMap(const char *filename, int ***outMap, int *outWidth, int *outHeight) {
    FILE *fp = fopen(filename, "r");
    if (!fp) 
        return 0;

    // Verifica se deu certo a leitura da largura e altura do mapa
    if (fscanf(fp, "%d %d", outWidth, outHeight) != 2) {
        fclose(fp);
        return 0;
    }

    int width  = *outWidth;
    int height = *outHeight;

    // Aloca uma matriz [width][height] para coordenadas do mapa
    int **map = (int **) malloc(width * sizeof(int *));
    if (!map) {
        fclose(fp);
        return 0;
    }
    // Laço para conferir se deu certo toda a alocação
    for (int x = 0; x < width; ++x) {
        map[x] = (int *) malloc(height * sizeof(int));
        if (!map[x]) {
            for (int j = 0; j < x; ++j) {
                free(map[j]);
            }
            free(map);
            fclose(fp);
            return 0;
        }
    }

    // Lê os valores no arquivo para carregar na matriz
    int x = 0, y = 0;
    while (!feof(fp) && y < height) {
        int tile;
        if (fscanf(fp, "%d", &tile) != 1) {
            break;
        }
        map[x][y] = tile;
        ++x;
        if (x >= width) {
            x = 0;
            ++y;
        }
    }

    fclose(fp);

    // Devolve o ponteiro da matriz preenchida
    *outMap    = map;
    *outWidth  = width;
    *outHeight = height;
    return 1;
}


void draw_map(int **map, int mapSizeX, int mapSizeY, MapContext *ctx, ALLEGRO_BITMAP* barricade, ALLEGRO_BITMAP* floor, ALLEGRO_BITMAP* rocks) {

    // Desenha background repetido/deslocado
    float offset = 0;
    for (float x = -offset; x < X_SCREEN; x += ctx->drawW) {
        al_draw_scaled_bitmap(
            ctx->background,
            0, 0,
            al_get_bitmap_width(ctx->background),
            al_get_bitmap_height(ctx->background),
            x, ctx->y0,
            ctx->drawW, ctx->drawH,
            0
        );
    }

    // Variáveis para desenhar os sprites nas coordenadas correspondentes com a proporção de TILE_SIZE
    float x0, y0, x1, y1;
    for (int i = 0; i < mapSizeX; i++) {
        for (int j = 0; j < mapSizeY; j++) {
            x0 = i * TILE_SIZE;
            y0 = j * TILE_SIZE;
            x1 = x0 + TILE_SIZE;
            y1 = y0 + TILE_SIZE;


			if (map[i][j] == 1) {  
				// desenha a imagem do tile “1”
				al_draw_scaled_bitmap(
					floor,
					0, 0,
					al_get_bitmap_width(floor),
					al_get_bitmap_height(floor),
					x0,y0,
					TILE_SIZE,TILE_SIZE,
					0
				);
        	} else if (map[i][j] == 2) {
				// desenha a imagem do tile “2”
				al_draw_scaled_bitmap(
					rocks,
					0, 0,
					al_get_bitmap_width(rocks),
					al_get_bitmap_height(rocks),
					x0,y0,
					TILE_SIZE,TILE_SIZE,
					0
				);
			} else if (map[i][j] == 5) {
				// desenha a imagem do tile “2”
				al_draw_scaled_bitmap(
					barricade,
					0, 0,
					al_get_bitmap_width(barricade),
					al_get_bitmap_height(barricade),
					x0,y0,
					TILE_SIZE,TILE_SIZE,
					0
				);
			}
    	}
	}
}


void draw_player(square *player, float playerGround) {
    // Variáveis para determinar qual será o sprite atual, altura do sprite e coordenada tela
    ALLEGRO_BITMAP *to_draw;
	unsigned short draw_h;
	float dy;

    // Verifica se o player está agachado para calcular seu sprite
	if (player->control->down && player->onGround) {
		// Verifica se está parado, se não anima movimento agachado
  		if (!(player->control->left || player->control->right)) 
    		to_draw = player->sprite_crouch[0];
  		else 
    		to_draw = player->sprite_crouch[player->crouch_step_frame];
  		
        // Valores ajustados para sprite agachado
  		draw_h = player->crouch_h;
		dy = playerGround - draw_h;
	} else if (!player->onGround) {
		// Valores ajustados para sprite pulando
        draw_h = player->orig_h;
    	dy = player->y - draw_h/2;
    	
        // Verifica em qual momento do pulo está para aplicar o sprite (subindo, ponto máximo do pulo ou descendo)
        if (player->velY < 0) 
        	to_draw = player->sprite_jump[0];
    	else if (player->velY == 0) 
        	to_draw = player->sprite_jump[1];
    	else 
        	to_draw = player->sprite_jump[2];
    	
	} else {
    	// Valores ajustados para sprite andando/parado
		draw_h = player->orig_h;
		dy     = player->y - draw_h/2;
        if (player->is_walking)
            to_draw = player->sprite_walk[player->step_frame];
        else
            to_draw = player->sprite_idle;
	}

	// Dimensões originais e destino do sprite
	int bw = al_get_bitmap_width(to_draw);
    int bh = al_get_bitmap_height(to_draw);
    float sw = player->w, sh = draw_h;
    int flags = (player->face == 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;

    // Posição no mapa do canto esquerdo do sprite
    float world_x = player->x - sw * 0.5f;

    // Conversão para tela (por causa do camera offset)
    float screen_x = world_x;   

    al_draw_scaled_bitmap(
        to_draw, 0, 0, bw, bh,
        screen_x, dy, sw, sh,
        flags
    );
}


void draw_shots(square *player, ALLEGRO_BITMAP* bmp_muzzle, ALLEGRO_BITMAP* bmp_bullet, ALLEGRO_BITMAP* bmp_boss_bullet) {
    
    // Verifica se houve disparo para desenhar o muzzle flash
    if (player->flash_timer > 0) {
        // Calcula posição do cano (Sprite do player)
        float fx = player->x + (player->face == 1 ? 
            player->w/2         // direita
            : -(player->w));    // esquerda
        float fy;

        // Verifica o ajuste do flash no cano da arma para player, inimigo e boss
        if (player->type == 1) {
            if (!player->control->down)
                fy = player->y - 44; 

            else 
                fy = player->y - 16;

        } else if (player->type == 2)
            fy = player->y - 38;

        else {
            if (!player->control->down)
                fy = player->y - 28; 

            else 
                fy = player->y;
        }

        int mw = al_get_bitmap_width(bmp_muzzle);
        int mh = al_get_bitmap_height(bmp_muzzle);
        
        // Escala do muzzle flash 
        float scale = 0.6f;          // 60% do tamanho original
        float dw = mw * scale;       // largura destino
        float dh = mh * scale;       // altura destino
        float dx = fx + (player->face == 1 ? 0: +36);

        float dy = fy;
        
        al_draw_scaled_bitmap(
              bmp_muzzle,
              0, 0, mw, mh,     // origem inteira
              dx, dy,           // canto superior esquerdo destino
              dw, dh,           // tamanho destino
              (player->face==0) ? ALLEGRO_FLIP_HORIZONTAL : 0
        );

        player->flash_timer--;
    }

    // For para impressão das balas
    for (bullet *b = player->gun->shots; b != NULL; b = (bullet*) b->next){
        float sw = 22, sh = 8;
        int bw, bh;
        if(player->type == 3) {
            bw = al_get_bitmap_width(bmp_boss_bullet);
            bh = al_get_bitmap_height(bmp_boss_bullet);
        } else {
            bw = al_get_bitmap_width(bmp_bullet);
            bh = al_get_bitmap_height(bmp_bullet);
        }
        float fy;
        
        // Verifica o ajuste da bala com cano da arma para player, inimigo e boss
        if (player->type == 1) {
            if (!b->crouched)
                fy = b->y - 22; 

            else 
                fy = b->y + 8;
        
        } else if (player->type == 2)
            fy = b->y - 18;
        
        else {
            if (!b->crouched)
                fy = b->y - 8; 

            else 
                fy = b->y + 18;
        }

        float world_x = b->x + (player->face? 0:-32);
        float screen_x = world_x;
        al_draw_scaled_bitmap(player->type == 3 ? bmp_boss_bullet : bmp_bullet,
            0,0,bw,bh,
            screen_x, fy, sw,sh,
            (b->dir?0:ALLEGRO_FLIP_HORIZONTAL));
    }
    
    if (player->gun->timer) player->gun->timer--;
}
