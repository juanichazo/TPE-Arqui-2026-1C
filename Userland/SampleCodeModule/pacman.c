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

#define SCORE_SMALL_DOT 10
#define SCORE_BIG_DOT 50
#define STARTING_LIVES 3

#define GHOST_MOVE_INTERVAL 2

static const int GHOST_RELEASE[4] = {0, 20, 40, 60};

static const int ORIGINAL_MAP[MAP_HEIGHT][MAP_WIDTH] = {
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

int map[MAP_HEIGHT][MAP_WIDTH];

int pacman_bmp_open[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}};

int pacman_bmp_closed[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}};

int ghost_bmp[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1}};

int pacman_mouth_frame = 0;

static const uint8_t digit_bmp[10][5] = {
    {7, 5, 5, 5, 7},
    {2, 6, 2, 2, 7},
    {7, 1, 7, 4, 7},
    {7, 1, 7, 1, 7},
    {5, 5, 7, 1, 1},
    {7, 4, 7, 1, 7},
    {7, 4, 7, 5, 7},
    {7, 1, 1, 1, 1},
    {7, 5, 7, 5, 7},
    {7, 5, 7, 1, 7}};

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
    int x, y;
    Direction current_dir;
    uint32_t color;
    int is_player_2;
    int in_house;
} Entity;

typedef enum
{
    MENU,
    PLAYING_1P,
    PLAYING_2P,
    GAME_OVER,
    WIN
} GameState;

GameState currentState = MENU;
Entity pacman;
Entity ghosts[NUM_GHOSTS];

int score = 0;
int lives = STARTING_LIVES;
int total_dots = 0;
int dots_eaten = 0;
int game_tick = 0;

#define DPIX 4 // Para la escala de los pixeles de los digitos
#define DGAP 2 // Separacion entre numeros
#define DSTEP (3 * DPIX + DGAP)

#define HUD_X 10
#define HUD_Y 20

void drawRect(int startX, int startY, int width, int height, uint32_t color)
{
    int x1 = (startX * SCALE) + OFFSET_X;
    int y1 = (startY * SCALE) + OFFSET_Y;
    int x2 = x1 + (width * SCALE) - 1;
    int y2 = y1 + (height * SCALE) - 1;
    drawRectangle(x1, y1, x2, y2, color);
}

static void drawDigit(int digit, int px, int py, uint32_t color)
{
    for (int row = 0; row < 5; row++)
    {
        uint8_t bits = digit_bmp[digit][row];
        for (int col = 0; col < 3; col++)
        {
            uint32_t c = ((bits >> (2 - col)) & 1) ? color : 0x000000;
            int x1 = px + col * DPIX;
            int y1 = py + row * DPIX;
            drawRectangle(x1, y1, x1 + DPIX - 1, y1 + DPIX - 1, c);
        }
    }
}

static void drawNumber(int num, int px, int py, uint32_t color)
{
    char digs[8];
    int cnt = 0;

    if (num == 0)
    {
        drawDigit(0, px, py, color);
        return;
    }

    int tmp = num;

    while (tmp > 0 && cnt < 7)
    {
        digs[cnt++] = (char)(tmp % 10);
        tmp /= 10;
    }

    for (int i = cnt - 1; i >= 0; i--)
    {
        drawDigit(digs[i], px + (cnt - 1 - i) * DSTEP, py, color);
    }
}

static void drawHUD(void)
{
    drawRectangle(HUD_X, HUD_Y, HUD_X + 140, HUD_Y + 6, 0xFFFFFF);
    drawRectangle(HUD_X, HUD_Y + 10, HUD_X + 140, HUD_Y + 40, 0x000000);
    drawNumber(score, HUD_X, HUD_Y + 12, 0xFFFFFF);

    drawRectangle(HUD_X, HUD_Y + 50, HUD_X + 140, HUD_Y + 56, COLOR_PACMAN);
    drawRectangle(HUD_X, HUD_Y + 60, HUD_X + 140, HUD_Y + 90, 0x000000);

    for (int i = 0; i < lives && i < 5; i++)
    {
        int lx = HUD_X + i * 18;
        int ly = HUD_Y + 62;
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                if (pacman_bmp_closed[y][x])
                {
                    drawRectangle(lx + x * 2, ly + y * 2, lx + x * 2 + 1, ly + y * 2 + 1, COLOR_PACMAN);
                }
            }
        }
    }
}

void drawWallCell(int pixel_x, int pixel_y)
{
    uint32_t wallColor = 0x0000FF;
    drawRect(pixel_x, pixel_y, BLOCK_SIZE, BLOCK_SIZE, 0x000000);

    if (pixel_y > 0)
    {
        drawRect(pixel_x, pixel_y, BLOCK_SIZE, 1, wallColor);
    }

    drawRect(pixel_x, pixel_y + BLOCK_SIZE - 1, BLOCK_SIZE, 1, wallColor);
    drawRect(pixel_x, pixel_y, 1, BLOCK_SIZE, wallColor);
    drawRect(pixel_x + BLOCK_SIZE - 1, pixel_y, 1, BLOCK_SIZE, wallColor);
}

void drawMap(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int px = x * BLOCK_SIZE;
            int py = y * BLOCK_SIZE;

            if (map[y][x] == 1)
            {
                drawWallCell(px, py);
            }
            else
            {
                drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, 0x000000);
                if (map[y][x] == 2)
                {
                    drawRect(px + 3, py + 3, 2, 2, 0xFFFFFF);
                }
                else if (map[y][x] == 3)
                {
                    drawRect(px + 2, py + 2, 4, 4, 0xFFFFFF);
                }
            }
        }
    }
}

void drawEntity(Entity *e)
{
    int (*bmp)[8] = (pacman_mouth_frame == 0) ? pacman_bmp_open : pacman_bmp_closed;
    int px = e->x * BLOCK_SIZE;
    int py = e->y * BLOCK_SIZE;

    drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, 0x000000);

    if (e == &pacman)
    {
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                if (!bmp[y][x])
                {
                    continue;
                }

                int tx = x, ty = y;

                switch (e->current_dir)
                {
                case LEFT:
                    tx = 7 - x;
                    break;

                case DOWN:
                    tx = 7 - y;
                    ty = x;
                    break;
                case UP:
                    tx = y;
                    ty = 7 - x;
                    break;
                default:
                    break;
                }
                drawRect(px + tx, py + ty, 1, 1, e->color);
            }
        }
    }
    else
    {
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                if (ghost_bmp[y][x])
                {
                    drawRect(px + x, py + y, 1, 1, e->color);
                }
            }
        }
    }
}

void eraseEntity(Entity *e)
{
    int px = e->x * BLOCK_SIZE;
    int py = e->y * BLOCK_SIZE;
    drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, 0x000000);

    if (map[e->y][e->x] == 2)
    {
        drawRect(px + 3, py + 3, 2, 2, 0xFFFFFF);
    }
    else if (map[e->y][e->x] == 3)
    {
        drawRect(px + 2, py + 2, 4, 4, 0xFFFFFF);
    }
}

static int absi(int n)
{
    return n < 0 ? -n : n;
}

static Direction oppositeDir(Direction d)
{
    switch (d)
    {
    case UP:
        return DOWN;

    case DOWN:
        return UP;

    case LEFT:
        return RIGHT;

    case RIGHT:
        return LEFT;

    default:
        return NONE;
    }
}

static void respawnEntities(int two_players_mode)
{
    pacman.x = 13;
    pacman.y = 23;
    pacman.current_dir = NONE;
    pacman.color = COLOR_PACMAN;

    ghosts[0].x = 13;
    ghosts[0].y = 11;
    ghosts[0].color = COLOR_BLINKY;
    ghosts[0].in_house = 0;
    ghosts[1].x = 11;
    ghosts[1].y = 14;
    ghosts[1].color = COLOR_PINKY;
    ghosts[1].in_house = 1;
    ghosts[2].x = 13;
    ghosts[2].y = 14;
    ghosts[2].color = COLOR_INKY;
    ghosts[2].in_house = 1;
    ghosts[3].x = 15;
    ghosts[3].y = 14;
    ghosts[3].color = COLOR_CLYDE;
    ghosts[3].in_house = 1;

    for (int i = 0; i < 4; i++)
    {
        ghosts[i].current_dir = NONE;
        ghosts[i].is_player_2 = (i == 0) ? two_players_mode : 0;
    }
}

void initGame(int two_players_mode)
{
    score = 0;
    lives = STARTING_LIVES;
    game_tick = 0;
    dots_eaten = 0;
    total_dots = 0;

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            map[y][x] = ORIGINAL_MAP[y][x];
            if (map[y][x] == 2 || map[y][x] == 3)
            {
                total_dots++;
            }
        }
    }
    respawnEntities(two_players_mode);
}

void moveEntity(Entity *e)
{
    if (e->current_dir == NONE)
    {
        return;
    }

    int nx = e->x, ny = e->y;

    switch (e->current_dir)
    {
    case UP:
        ny--;
        break;

    case DOWN:
        ny++;
        break;

    case LEFT:
        nx--;
        break;
    case RIGHT:
        nx++;
        break;

    default:
        break;
    }

    if (nx < 0)
    {
        nx = MAP_WIDTH - 1;
    }
    else if (nx >= MAP_WIDTH)
    {
        nx = 0;
    }

    if (map[ny][nx] == 1)
    {
        return;
    }

    e->x = nx;
    e->y = ny;

    if (e == &pacman)
    {
        if (map[ny][nx] == 2)
        {
            map[ny][nx] = 0;
            score += SCORE_SMALL_DOT;
            dots_eaten++;
        }
        else if (map[ny][nx] == 3)
        {
            map[ny][nx] = 0;
            score += SCORE_BIG_DOT;
            dots_eaten++;
        }
    }
}

void moveGhostAI(Entity *ghost, int ghost_id)
{
    if (game_tick < GHOST_RELEASE[ghost_id])
    {
        return;
    }

    static const int dx[4] = {0, 0, -1, 1};
    static const int dy[4] = {-1, 1, 0, 0};
    static const Direction dirs[4] = {UP, DOWN, LEFT, RIGHT};

    int tx, ty;

    if (ghost->in_house)
    {
        tx = 13;
        ty = 11;
        if (ghost->y <= 11)
        {
            ghost->in_house = 0;
        }
    }
    else
    {
        tx = pacman.x;
        ty = pacman.y;
        if (ghost_id == 1)
        {

            switch (pacman.current_dir)
            {
            case UP:
                ty -= 4;
                break;

            case DOWN:
                ty += 4;
                break;

            case LEFT:
                tx -= 4;
                break;

            case RIGHT:
                tx += 4;
                break;

            default:
                break;
            }
        }
        else if (ghost_id == 2)
        {
            int mid_x = pacman.x + (pacman.current_dir == RIGHT ? 2 : pacman.current_dir == LEFT ? -2
                                                                                                 : 0);
            int mid_y = pacman.y + (pacman.current_dir == DOWN ? 2 : pacman.current_dir == UP ? -2
                                                                                              : 0);
            tx = 2 * mid_x - ghosts[0].x;
            ty = 2 * mid_y - ghosts[0].y;
        }
        else if (ghost_id == 3)
        {
            if (absi(ghost->x - pacman.x) + absi(ghost->y - pacman.y) < 8)
            {
                tx = 0;
                ty = MAP_HEIGHT - 1;
            }
        }
    }

    Direction opp = oppositeDir(ghost->current_dir);
    Direction best = NONE;
    int best_dist = 99999;

    for (int i = 0; i < 4; i++)
    {
        if (dirs[i] == opp)
        {
            continue;
        }

        int nx = ghost->x + dx[i];
        int ny = ghost->y + dy[i];

        if (nx < 0)
        {
            nx = MAP_WIDTH - 1;
        }
        else if (nx >= MAP_WIDTH)
        {
            nx = 0;
        }
        if (ny < 0 || ny >= MAP_HEIGHT || map[ny][nx] == 1)
            continue;

        int dist = absi(nx - tx) + absi(ny - ty);
        if (dist < best_dist)
        {
            best_dist = dist;
            best = dirs[i];
        }
    }

    if (best == NONE)
    {
        for (int i = 0; i < 4; i++)
        {
            int nx = ghost->x + dx[i];
            int ny = ghost->y + dy[i];

            if (nx < 0)
            {
                nx = MAP_WIDTH - 1;
            }
            else if (nx >= MAP_WIDTH)
            {
                nx = 0;
            }

            if (ny < 0 || ny >= MAP_HEIGHT)
            {
                continue;
            }

            if (map[ny][nx] != 1)
            {
                best = dirs[i];
                break;
            }
        }
    }

    if (best != NONE)
    {
        ghost->current_dir = best;
        moveEntity(ghost);
    }
}

void handleCollision(void)
{
    lives--;

    uint32_t saved_color = pacman.color;
    pacman.color = 0xFF0000;
    drawEntity(&pacman);
    sleep(20);
    pacman.color = saved_color;

    eraseEntity(&pacman);
    for (int i = 0; i < NUM_GHOSTS; i++)
        eraseEntity(&ghosts[i]);

    if (lives <= 0)
    {
        currentState = GAME_OVER;
        return;
    }

    respawnEntities(currentState == PLAYING_2P ? 1 : 0);
    drawHUD();
    game_tick = 0;
    sleep(30);
}

void gameLoop(void)
{
    drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);
    drawMap();
    drawHUD();

    while (currentState != GAME_OVER && currentState != WIN)
    {
        char key = getCharNoWait();
        if (key != 0)
        {
            if (key == 'w')
                pacman.current_dir = UP;
            else if (key == 's')
                pacman.current_dir = DOWN;
            else if (key == 'a')
                pacman.current_dir = LEFT;
            else if (key == 'd')
                pacman.current_dir = RIGHT;
            else if (key == 'q')
                currentState = GAME_OVER;

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
        }

        eraseEntity(&pacman);
        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            eraseEntity(&ghosts[i]);
        }

        moveEntity(&pacman);

        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            if (ghosts[i].is_player_2)
            {
                moveEntity(&ghosts[i]);
            }
            else if (game_tick % GHOST_MOVE_INTERVAL == 0)
            {
                moveGhostAI(&ghosts[i], i);
            }
        }

        if (pacman.current_dir != NONE)
        {
            pacman_mouth_frame = 1 - pacman_mouth_frame;
        }
        else
        {
            pacman_mouth_frame = 0;
        }

        drawEntity(&pacman);
        for (int i = 0; i < NUM_GHOSTS; i++)
            drawEntity(&ghosts[i]);

        drawHUD();

        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            if (pacman.x == ghosts[i].x && pacman.y == ghosts[i].y)
            {
                handleCollision();
                if (currentState == GAME_OVER)
                {
                    break;
                }
                drawMap();
                drawEntity(&pacman);
                for (int j = 0; j < NUM_GHOSTS; j++)
                {
                    drawEntity(&ghosts[j]);
                }
                break;
            }
        }

        if (dots_eaten >= total_dots)
        {
            currentState = WIN;
        }

        game_tick++;
        sleep(2);
    }
}

static void printInt(int num)
{
    if (num == 0)
    {
        putChar('0');
        return;
    }

    char buf[12];
    int i = 0;
    
    while (num > 0)
    {
        buf[i++] = (char)('0' + num % 10);
        num /= 10;
    }
    
    for (int j = i - 1; j >= 0; j--){
        putChar(buf[j]);
    }
}

void startPacman(void)
{
    while (1)
    {
        currentState = MENU;

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
            puts("  1. Un Jugador  (Humano vs IA)");
            setTextColor(0xFF0000, 0x000000);
            puts("  2. Dos Jugadores (Humano vs Humano)\n");
            setTextColor(0xFFFFFF, 0x000000);
            puts("  Q. Salir a la terminal");

            char key = getChar();
            if (key == '1'){
                currentState = PLAYING_1P;
            }
            else if (key == '2'){
                currentState = PLAYING_2P;
            }
            else if (key == 'q' || key == 'Q')
            {
                clearScreen();
                setTextSize(10);
                setTextColor(0xFFFFFF, 0x000000);
                return;
            }
        }

        clearScreen();
        setTextSize(10);
        setTextColor(0xFFFFFF, 0x000000);

        initGame(currentState == PLAYING_2P ? 1 : 0);
        gameLoop();

        drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);
        clearScreen();
        setTextSize(28);

        if (currentState == WIN)
        {
            setTextColor(COLOR_PACMAN, 0x000000);
            puts("\n\n   YOU WIN!   \n");
        }
        else
        {
            setTextColor(0xFF0000, 0x000000);
            puts("\n\n  GAME OVER  \n");
        }

        setTextSize(16);
        setTextColor(0xFFFFFF, 0x000000);
        puts("  Score: ");
        printInt(score);
        putChar('\n');

        sleep(300);
    }
}