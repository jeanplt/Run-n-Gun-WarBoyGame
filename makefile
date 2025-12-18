TARGET = AS
SOURCES = WarBoyGame.c Square.c Joystick.c Bullet.c Pistol.c Menu.c Mechanics.c Designs.c
ALLEGRO_FLAGS = $(shell pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_dialog-5 allegro_image-5 --libs --cflags)

all:
	gcc $(SOURCES) -o $(TARGET) -lm $(ALLEGRO_FLAGS)

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
