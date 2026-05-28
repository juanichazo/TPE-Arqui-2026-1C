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

#define MAP_WIDTH 28
#define MAP_HEIGHT 31

#define BLOCK_SIZE 12

#define SCALE 2
#define OFFSET_X 176
#define OFFSET_Y 12

// 0 es pasillo, 1 es pared, 2 es pastilla chica, 3 es pastilla grande
int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
    {1, 3, 1, 0, 0, 1, 2, 1, 0, 0, 0, 1, 2, 1, 1, 2, 1, 0, 0, 0, 1, 2, 1, 0, 0, 1, 3, 1},
    {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1},
    {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
    {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
    {1, 3, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 3, 1},
    {1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1},
    {1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
    {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int pacman_bmp_open[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}};

// Frame 1: boca cerrada (círculo completo)
int pacman_bmp_closed[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}};

int pacman_mouth_frame = 0;

int ghost_bmp[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1}};

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
    pacman.x = 13;
    pacman.y = 23;
    pacman.color = COLOR_PACMAN;
    pacman.current_dir = NONE;
    pacman.is_alive = 1;

    ghosts[0].x = 13;
    ghosts[0].y = 11;
    ghosts[0].color = COLOR_BLINKY;
    ghosts[0].is_player_2 = two_players_mode;

    ghosts[1].x = 11;
    ghosts[1].y = 14;
    ghosts[1].color = COLOR_PINKY;
    ghosts[1].is_player_2 = 0;

    ghosts[2].x = 13;
    ghosts[2].y = 14;
    ghosts[2].color = COLOR_INKY;
    ghosts[2].is_player_2 = 0;

    ghosts[3].x = 15;
    ghosts[3].y = 14;
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

void moveGhostAI(Entity *ghost, int ghost_id)
{
    // TODO
}

void handleCollision(Entity *ghost)
{
    eraseEntity(&pacman);
    for (int i = 0; i < NUM_GHOSTS; i++)
    {
        eraseEntity(&ghosts[i]);
    }

    initGame(currentState == PLAYING_2P ? 1 : 0);
}

void drawRect(int startX, int startY, int width, int height, uint32_t color)
{
    int x1 = (startX * SCALE) + OFFSET_X;
    int y1 = (startY * SCALE) + OFFSET_Y;

    int x2 = x1 + (width * SCALE) - 1;
    int y2 = y1 + (height * SCALE) - 1;

    drawRectangle(x1, y1, x2, y2, color);
}

void drawWallCell(int pixel_x, int pixel_y)
{
    uint32_t wallColor = 0x0000FF;

    drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000);

    if (pixel_y > 0)
        drawRect(pixel_x, pixel_y, BLOCK_SIZE, 1, wallColor);

    drawRect(pixel_x, pixel_y + BLOCK_SIZE - 1, BLOCK_SIZE, 1, wallColor);

    drawRect(pixel_x, pixel_y, 1, BLOCK_SIZE, wallColor);
    drawRect(pixel_x + BLOCK_SIZE - 1, pixel_y, 1, BLOCK_SIZE, wallColor);
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
                drawWallCell(pixel_x, pixel_y);
            }
            else
            {
                drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000);

                if (map[y][x] == 2)
                {
                    drawRect(pixel_x + 3, pixel_y + 3, 2, 2, 0xFFFFFF);
                }
                else if (map[y][x] == 3)
                {
                    drawRect(pixel_x + 2, pixel_y + 2, 4, 4, 0xFFFFFF);
                }
            }
        }
    }
}

void clearGraphics()
{
    drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);
}

void drawEntity(Entity *e)
{
    int (*bmp)[8] = (pacman_mouth_frame == 0) ? pacman_bmp_open : pacman_bmp_closed;
    int pixel_x = e->x * BLOCK_SIZE;
    int pixel_y = e->y * BLOCK_SIZE;
    // primero fondo negro
    drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000);
    if (e == &pacman)
    {
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                if (bmp[y][x] == 1)
                {
                    int target_x = x;
                    int target_y = y;

                    if (e->current_dir == LEFT)
                    {
                        target_x = 7 - x; // ← flip horizontal para LEFT
                    }
                    else if (e->current_dir == RIGHT)
                    {
                        // usa target_x = x original, sin cambio
                    }
                    else if (e->current_dir == DOWN)
                    {
                        target_x = 7 - y;
                        target_y = x;
                    }
                    else if (e->current_dir == UP)
                    {
                        target_x = y;
                        target_y = 7 - x;
                    }
                    // Si current_dir == LEFT usa x y originales.
                    drawRect(pixel_x + target_x, pixel_y + target_y, 1, 1, e->color);
                }
            }
        }
    }
    else
    {
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
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
        drawRect(pixel_x + 3, pixel_y + 3, 2, 2, 0xFFFFFF);
    }
    else if (map[e->y][e->x] == 3)
    {
        drawRect(pixel_x + 2, pixel_y + 2, 4, 4, 0xFFFFFF);
    }
}

void gameLoop()
{
    clearGraphics();

    // dibujar todo
    drawMap();

    while (currentState != GAME_OVER)
    {
        // leer teclado para pacman y para el fantasma jugador 2 (si aplica)
        handleInput();

        // borrar entidades de su posicion vieja
        eraseEntity(&pacman);
        for (int i = 0; i < NUM_GHOSTS; i++)
        {
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
                moveGhostAI(&ghosts[i], i);
            }
        }

        if (pacman.current_dir != NONE)
            pacman_mouth_frame = 1 - pacman_mouth_frame;
        else
            pacman_mouth_frame = 0;
            
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
    {
        clearScreen();

        setTextSize(30);
        setTextColor(COLOR_PACMAN, 0x000000);
        puts("\n   PAC-MAN   \n");

        setTextSize(15);
        setTextColor(0xFFFFFF, 0x000000);
        puts(" Seleccione el modo de juego:\n");

        setTextColor(0x00FFFF, 0x000000);
        puts("  1. Un Jugador (Humano vs IA)");
        setTextColor(0xFF0000, 0x000000);
        puts("  2. Dos Jugadores (Humano vs Humano)\n");
        setTextColor(0xFFFFFF, 0x000000);
        puts("  Q. Salir a la terminal");

        char key = getChar();
        if (key == '1')
            currentState = PLAYING_1P;
        else if (key == '2')
            currentState = PLAYING_2P;
        else if (key == 'q' || key == 'Q')
        {
            clearScreen();
            setTextSize(10);
            setTextColor(0xFFFFFF, 0x000000);
            return; // Abortamos antes de empezar
        }
    }

    clearScreen();
    clearGraphics();
    setTextSize(10);
    setTextColor(0xFFFFFF, 0x000000);

    initGame(currentState == PLAYING_2P ? 1 : 0);
    gameLoop();

    clearGraphics();
    clearScreen();
    setTextSize(10);
    setTextColor(0xFFFFFF, 0x000000);

    currentState = MENU;
}