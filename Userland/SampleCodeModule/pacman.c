#include <stdint.h>
#include "include/libc.h"
#define NUM_GHOSTS 4

#define COLOR_PACMAN 0xFFFF00
#define COLOR_BLINKY 0xFF0000
#define COLOR_PINKY 0xFFB8FF
#define COLOR_INKY 0x00FFFF
#define COLOR_CLYDE 0xFFB852

#define Y_RESOLUTION 768
#define X_RESOLUTION 1024

#define MAP_WIDTH 20
#define MAP_HEIGHT 15

#define BLOCK_SIZE 16

#define SCALE 3      
#define OFFSET_X 32  
#define OFFSET_Y 24  

// 0 es pasillo, 1 es pared, 2 es pastilla chica, 3 es pastilla grande
int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 3, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 3, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 2, 1, 1, 1, 1},
    {0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0},
    {1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 3, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 3, 1},
    {1, 2, 2, 1, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 1, 2, 2, 1},
    {1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1},
    {1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int pacman_bmp[BLOCK_SIZE][BLOCK_SIZE] = {
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}};

int ghost_bmp[BLOCK_SIZE][BLOCK_SIZE] = {
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1},
    {1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1}};

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
} Direction;

typedef struct
{
    int x;
    int y;
    int start_x;
    int start_y;
    Direction current_dir;
    uint32_t color;
    int is_alive;
    int is_player_2; // 1 si lo controla un humano, 0 si es IA
} Entity;

typedef enum
{
    MENU,
    PLAYING_1P, // humano vs IA
    PLAYING_2P, // humano vs humano
    GAME_OVER
} GameState;

GameState currentState = MENU;

Entity pacman;
Entity ghosts[NUM_GHOSTS];

void initGame(int two_players_mode)
{
    pacman.x = 9;
    pacman.y = 11;
    pacman.color = COLOR_PACMAN;
    pacman.current_dir = NONE;
    pacman.is_alive = 1;

    ghosts[0].x = 10;
    ghosts[0].y = 7;
    ghosts[0].color = COLOR_BLINKY;
    ghosts[0].is_player_2 = two_players_mode; // este va a manejar el segundo jugador

    ghosts[1].x = 9;
    ghosts[1].y = 9;
    ghosts[1].color = COLOR_PINKY;
    ghosts[1].is_player_2 = 0;

    ghosts[2].x = 10;
    ghosts[2].y = 9;
    ghosts[2].color = COLOR_INKY;
    ghosts[2].is_player_2 = 0;

    ghosts[3].x = 11;
    ghosts[3].y = 9;
    ghosts[3].color = COLOR_CLYDE;
    ghosts[3].is_player_2 = 0;
}

void handleInput()
{
    char key = getCharNoWait();

    if (key == 0)
        return;

    if (key == 'w')
        pacman.current_dir = UP;
    else if (key == 's')
        pacman.current_dir = DOWN;
    else if (key == 'a')
        pacman.current_dir = LEFT;
    else if (key == 'd')
        pacman.current_dir = RIGHT;

    if (ghosts[0].is_player_2)
    {
        if (key == 'i')
            ghosts[0].current_dir = UP;
        else if (key == 'k')
            ghosts[0].current_dir = DOWN;
        else if (key == 'j')
            ghosts[0].current_dir = LEFT;
        else if (key == 'l')
            ghosts[0].current_dir = RIGHT;
    }

    if (key == 'q')
    { // o esc
        currentState = GAME_OVER;
    }
}

void moveEntity(Entity *e)
{
    if (e->current_dir == NONE)
        return;

    int next_x = e->x;
    int next_y = e->y;

    if (e->current_dir == UP)
        next_y--;
    else if (e->current_dir == DOWN)
        next_y++;
    else if (e->current_dir == LEFT)
        next_x--;
    else if (e->current_dir == RIGHT)
        next_x++;

    if (next_x < 0)
        next_x = MAP_WIDTH - 1;
    else if (next_x >= MAP_WIDTH)
        next_x = 0;

    if (map[next_y][next_x] != 1)
    {
        e->x = next_x;
        e->y = next_y;

        if (e == &pacman)
        {
            if (map[next_y][next_x] == 2 || map[next_y][next_x] == 3)
            {
                map[next_y][next_x] = 0; // come
                // agregar score despues
            }
        }
    }
}

void moveGhostAI(Entity *ghost)
{
    // TODO
}

void handleCollision(Entity *ghost)
{
    // TODO
    initGame(0);
}

void drawRect(int startX, int startY, int width, int height, uint32_t color)
{
    int x1 = (startX * SCALE) + OFFSET_X;
    int y1 = (startY * SCALE) + OFFSET_Y;
    
    int x2 = x1 + (width * SCALE) - 1;
    int y2 = y1 + (height * SCALE) - 1;

    drawRectangle(x1, y1, x2, y2, color);
}

void drawMap()
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int pixel_x = x * BLOCK_SIZE;
            int pixel_y = y * BLOCK_SIZE;

            if (map[y][x] == 1)
            {
                drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x0000FF); // pared
            }
            else if (map[y][x] == 2)
            {
                drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000); // fondo negro
                drawRect(pixel_x + 6, pixel_y + 6, 4, 4, 0xFFFFFF);           // pastilla
            }
            else if (map[y][x] == 3)
            {
                drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000); // fondo negro
                drawRect(pixel_x + 4, pixel_y + 4, 8, 8, 0xFFFFFF);           // pastilla grande
            }
            else
            {
                drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000); // fondo negro
            }
        }
    }
}

void clearGraphics()
{
    //  resolución estándar de 1024x768. 
    for (int y = 0; y < Y_RESOLUTION; y++)
    {
        for (int x = 0; x < X_RESOLUTION; x++)
        {
            drawPixel(0x000000, x, y); 
        }
    }
}

void drawEntity(Entity *e)
{
    int pixel_x = e->x * BLOCK_SIZE;
    int pixel_y = e->y * BLOCK_SIZE;
    // primero fondo negro
    drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000);
    if (e == &pacman)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                if (pacman_bmp[y][x] == 1)
                {
                    int target_x = x;
                    int target_y = y;

                    if (e->current_dir == RIGHT)
                    {
                        target_x = 15 - x;
                    }
                    else if (e->current_dir == UP)
                    {
                        target_x = 15 - y; // Rotación 90° Horaria (boca arriba)
                        target_y = x;
                    }
                    else if (e->current_dir == DOWN)
                    {
                        target_x = y; // Rotación 90° Antihoraria (boca abajo)
                        target_y = 15 - x;
                    }
                    // Si current_dir == LEFT usa x y originales.
                    drawRect(pixel_x + target_x, pixel_y + target_y, 1, 1, e->color);
                }
            }
        }
    }
    else
    {
        for (int y = 0; y < 16; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                if (ghost_bmp[y][x] == 1)
                {
                    drawRect(pixel_x + x, pixel_y + y, 1, 1, e->color);
                }
            }
        }
    }
}

void eraseEntity(Entity *e)
{
    int pixel_x = e->x * BLOCK_SIZE;
    int pixel_y = e->y * BLOCK_SIZE;

    drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000);

    if (map[e->y][e->x] == 2)
    {
        drawRect(pixel_x + 6, pixel_y + 6, 4, 4, 0xFFFFFF);
    }
    else if (map[e->y][e->x] == 3)
    {
        drawRect(pixel_x + 4, pixel_y + 4, 8, 8, 0xFFFFFF);
    }
}

void gameLoop()
{
    clearGraphics();

    // dibujar todo
    drawMap();

    while (1)
    {
        // leer teclado para pacman y para el fantasma jugador 2 (si aplica)
        handleInput();

        // borrar entidades de su posicion vieja
        eraseEntity(&pacman);
        for (int i = 0; i < NUM_GHOSTS; i++) {
            eraseEntity(&ghosts[i]);
        }

        // mover pacman
        moveEntity(&pacman);

        // mover a los fantasmas
        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            if (ghosts[i].is_player_2)
            {
                moveEntity(&ghosts[i]);
            }
            else
            {
                moveGhostAI(&ghosts[i]);
            }
        }

        drawEntity(&pacman);
        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            drawEntity(&ghosts[i]);
        }


        // colisiones entre pacman y alguno de los fantasmas
        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            if (pacman.x == ghosts[i].x && pacman.y == ghosts[i].y)
            {
                handleCollision(&ghosts[i]);
            }
        }

        sleep(2);
    }
}

void startPacman()
{
    while (currentState == MENU)
    { // hacerlo lindo
        puts("PACMAN");
        puts("Ingrese la cantidad de jugadores");
        puts("1. Un Jugador");
        puts("2. Dos Jugadores");

        char key = getChar();
        if (key == '1')
        {
            currentState = PLAYING_1P;
        }
        else if (key == '2')
        {
            currentState = PLAYING_2P;
        }
    }
    initGame(0); // despues agregar la otra opc
    gameLoop();
}