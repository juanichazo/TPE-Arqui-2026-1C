#include <stdint.h>
#include <libc.h>
#include <pacman_audio.h>
#include <pacman_map.h>

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
#define TILEMAP_ROW_OFFSET 3

#define SCORE_SMALL_DOT 10
#define SCORE_BIG_DOT 50
#define STARTING_LIVES 3

#define GHOST_MOVE_INTERVAL 2
#define FRIGHTEN_DURATION 80
#define SCORE_GHOST_EAT 200
#define COLOR_FRIGHTENED 0x0000CC
#define FRIGHTEN_FLASH_THRESHOLD 20

static const int GHOST_RELEASE[4] = {0, 20, 40, 60};
static const int GHOST_START_X[NUM_GHOSTS] = {13, 11, 13, 15};
static const int GHOST_START_Y[NUM_GHOSTS] = {11, 14, 14, 14};
static const int GHOST_START_IN_HOUSE[NUM_GHOSTS] = {0, 1, 1, 1};

static int tileToLogicValue(int tile)
{
    switch (tile)
    {
    case TILE_PELLET:
        return 2;
    case TILE_POWER_PELLET:
        return 3;
    case TILE_WALL_H:
    case TILE_WALL_V:
    case TILE_CORNER_TL:
    case TILE_CORNER_TR:
    case TILE_CORNER_BL:
    case TILE_CORNER_BR:
    case TILE_T_UP:
    case TILE_T_DOWN:
        return 1;
    case TILE_EMPTY:
    case TILE_GHOST_GATE:
    case TILE_OUT_OF_BOUND:
    default:
        return 0;
    }
}

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

static int ghost_eyes_bmp[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}};

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
    int prev_x, prev_y;
    Direction current_dir;
    uint32_t color;
    int is_player_2;
    int in_house;
    int frightened;
    int frighten_timer;
    int returning;
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

void drawWallTile(int px, int py, int tile_type)
{
    uint32_t wallColor = 0x0000FF;

    drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, 0x000000);

    int mid = BLOCK_SIZE / 2;
    int thick = 2; // Grosor de la pared

    // Dibujamos según el tipo específico de pared
    switch (tile_type)
    {
    case TILE_WALL_H:
        // Línea horizontal de lado a lado
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor);
        break;

    case TILE_WALL_V:
        // Línea vertical de arriba a abajo
        drawRect(px + mid - 1, py, thick, BLOCK_SIZE, wallColor);
        break;

    case TILE_CORNER_TL:                                                 // Esquina Superior Izquierda (dobla hacia abajo y derecha)
        drawRect(px + mid - 1, py + mid - 1, mid + 1, thick, wallColor); // Derecha
        drawRect(px + mid - 1, py + mid - 1, thick, mid + 1, wallColor); // Abajo
        break;

    case TILE_CORNER_TR:                                                 // Esquina Superior Derecha (dobla hacia abajo e izquierda)
        drawRect(px, py + mid - 1, mid + 1, thick, wallColor);           // Izquierda
        drawRect(px + mid - 1, py + mid - 1, thick, mid + 1, wallColor); // Abajo
        break;

    case TILE_CORNER_BL:                                                 // Esquina Inferior Izquierda (dobla hacia arriba y derecha)
        drawRect(px + mid - 1, py + mid - 1, mid + 1, thick, wallColor); // Derecha
        drawRect(px + mid - 1, py, thick, mid, wallColor);               // Arriba
        break;

    case TILE_CORNER_BR:                                       // Esquina Inferior Derecha (dobla hacia arriba e izquierda)
        drawRect(px, py + mid - 1, mid + 1, thick, wallColor); // Izquierda
        drawRect(px + mid - 1, py, thick, mid, wallColor);     // Arriba
        break;

    case TILE_T_UP:                                               // Forma de T hacia arriba
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor); // Horizontal completa
        drawRect(px + mid - 1, py, thick, mid, wallColor);        // Palito hacia arriba
        break;

    case TILE_T_DOWN:                                             // Forma de T hacia abajo
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor); // Horizontal completa
        drawRect(px + mid - 1, py + mid, thick, mid, wallColor);  // Palito hacia abajo
        break;

    default:
        // Si es otro tipo de pared no especificado, dibujamos un puntito para debuggear
        drawRect(px + mid - 1, py + mid - 1, thick, thick, wallColor);
        break;
    }
}

void drawMap(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int px = x * BLOCK_SIZE;
            int py = y * BLOCK_SIZE;

            int tile = pacman_tilemap[y + TILEMAP_ROW_OFFSET][x];

            if (map[y][x] == 1)
            {
                drawWallTile(px, py, tile);
            }
            else
            {
                drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, 0x000000);
                if (map[y][x] == 2)
                {
                    drawRect(px + 5, py + 5, 2, 2, 0xFFFFFF);
                }
                else if (map[y][x] == 3)
                {
                    drawRect(px + 4, py + 4, 4, 4, 0xFFFFFF);
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
        if (e->returning)
        {
            for (int y = 0; y < 8; y++)
                for (int x = 0; x < 8; x++)
                    if (ghost_eyes_bmp[y][x])
                        drawRect(px + x, py + y, 1, 1, 0xFFFFFF);
            return;
        }

        uint32_t draw_color;
        if (e->frightened)
        {
            if (e->frighten_timer < FRIGHTEN_FLASH_THRESHOLD && (game_tick / 4) % 2 == 0)
                draw_color = 0xFFFFFF;
            else
                draw_color = COLOR_FRIGHTENED;
        }
        else
        {
            draw_color = e->color;
        }
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                if (ghost_bmp[y][x])
                {
                    drawRect(px + x, py + y, 1, 1, draw_color);
                }
            }
        }
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
        ghosts[i].prev_x = ghosts[i].x;
        ghosts[i].prev_y = ghosts[i].y;
        ghosts[i].frightened = 0;
        ghosts[i].frighten_timer = 0;
        ghosts[i].returning = 0;
    }

    pacman.prev_x = pacman.x;
    pacman.prev_y = pacman.y;
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
            map[y][x] = tileToLogicValue(pacman_tilemap[y + TILEMAP_ROW_OFFSET][x]);
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

    e->prev_x = e->x;
    e->prev_y = e->y;

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
            for (int i = 0; i < NUM_GHOSTS; i++)
            {
                ghosts[i].frightened = 1;
                ghosts[i].frighten_timer = FRIGHTEN_DURATION;
            }
        }
    }
}

void moveGhostAI(Entity *ghost, int ghost_id)
{
    static const int dx[4] = {0, 0, -1, 1};
    static const int dy[4] = {-1, 1, 0, 0};
    static const Direction dirs[4] = {UP, DOWN, LEFT, RIGHT};

    if (ghost->returning)
    {
        int tx = 13, ty = 14;
        Direction opp = oppositeDir(ghost->current_dir);
        Direction best = NONE;
        int best_dist = 99999;

        for (int i = 0; i < 4; i++)
        {
            if (dirs[i] == opp && ghost->current_dir != NONE) continue;
            int nx = ghost->x + dx[i];
            int ny = ghost->y + dy[i];
            if (nx < 0) nx = MAP_WIDTH - 1;
            else if (nx >= MAP_WIDTH) nx = 0;
            if (ny < 0 || ny >= MAP_HEIGHT) continue;
            if (map[ny][nx] == 1) continue;
            int dist = absi(nx - tx) + absi(ny - ty);
            if (dist < best_dist || (dist == best_dist && dirs[i] == ghost->current_dir))
            {
                best_dist = dist;
                best = dirs[i];
            }
        }
        if (best == NONE) best = opp;
        if (best != NONE)
        {
            ghost->current_dir = best;
            moveEntity(ghost);
        }

        if (absi(ghost->x - 13) + absi(ghost->y - 14) <= 1)
        {
            ghost->returning = 0;
            ghost->in_house = 1;
            ghost->x = 13;
            ghost->y = 14;
            ghost->current_dir = NONE;
        }
        return;
    }

    if (game_tick < GHOST_RELEASE[ghost_id])
    {
        return;
    }

    if (ghost->frightened && !ghost->in_house)
    {
        Direction opp = oppositeDir(ghost->current_dir);
        Direction valid[4];
        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            if (dirs[i] == opp && ghost->current_dir != NONE)
                continue;
            int nx = ghost->x + dx[i];
            int ny = ghost->y + dy[i];
            if (nx < 0) nx = MAP_WIDTH - 1;
            else if (nx >= MAP_WIDTH) nx = 0;
            if (ny < 0 || ny >= MAP_HEIGHT) continue;
            if (map[ny][nx] == 1) continue;
            if (pacman_tilemap[ny + TILEMAP_ROW_OFFSET][nx] == TILE_GHOST_GATE) continue;
            valid[count++] = dirs[i];
        }
        if (count > 0)
            ghost->current_dir = valid[(ghost->x * 7 + ghost->y * 13 + game_tick) % count];
        else
            ghost->current_dir = opp;
        moveEntity(ghost);
        return;
    }

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

        if (ghost_id == 1) // Pinky (emboscada)
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
        else if (ghost_id == 2) // Inky (flanqueo)
        {
            int mid_x = pacman.x + (pacman.current_dir == RIGHT ? 2 : pacman.current_dir == LEFT ? -2
                                                                                                 : 0);
            int mid_y = pacman.y + (pacman.current_dir == DOWN ? 2 : pacman.current_dir == UP ? -2
                                                                                              : 0);
            tx = 2 * mid_x - ghosts[0].x;
            ty = 2 * mid_y - ghosts[0].y;
        }
        else if (ghost_id == 3) // Clyde (dispersión)
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
        if (dirs[i] == opp && ghost->current_dir != NONE)
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

        if (ny < 0 || ny >= MAP_HEIGHT)
        {
            continue;
        }

        int is_wall = (map[ny][nx] == 1);

        if (pacman_tilemap[ny + TILEMAP_ROW_OFFSET][nx] == TILE_GHOST_GATE)
        {
            if (ghost->in_house)
            {
                is_wall = 0;
            }
            else
            {
                is_wall = 1; // para que el rojo no entre al rectángulo
            }
        }

        if (is_wall)
        {
            continue;
        }

        int dist = absi(nx - tx) + absi(ny - ty);

        if (dist < best_dist || (dist == best_dist && dirs[i] == ghost->current_dir))
        {
            best_dist = dist;
            best = dirs[i];
        }
    }

    if (best == NONE)
    {
        for (int i = 0; i < 4; i++)
        {
            if (dirs[i] == opp && ghost->current_dir != NONE)
            {
                continue;
            }

            int nx = ghost->x + dx[i];
            int ny = ghost->y + dy[i];

            if (nx < 0)
                nx = MAP_WIDTH - 1;
            else if (nx >= MAP_WIDTH)
                nx = 0;

            if (ny < 0 || ny >= MAP_HEIGHT)
                continue;

            if (map[ny][nx] != 1)
            {
                best = dirs[i];
                break;
            }
        }

        if (best == NONE)
        {
            best = opp;
        }
    }

    if (best != NONE)
    {
        ghost->current_dir = best;
        moveEntity(ghost);
    }
}

static void redrawTile(int x, int y)
{
    int px = x * BLOCK_SIZE;
    int py = y * BLOCK_SIZE;

    if (map[y][x] == 1)
    {
        drawWallTile(px, py, pacman_tilemap[y + TILEMAP_ROW_OFFSET][x]);
    }
    else
    {
        drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, 0x000000);
        if (map[y][x] == 2)
            drawRect(px + 5, py + 5, 2, 2, 0xFFFFFF);
        else if (map[y][x] == 3)
            drawRect(px + 4, py + 4, 4, 4, 0xFFFFFF);
    }
}

void handleCollision(void)
{
    lives--;

    uint32_t saved_color = pacman.color;
    pacman.color = 0xFF0000;
    redrawTile(pacman.x, pacman.y);
    drawEntity(&pacman);
    sleep(20);
    pacman.color = saved_color;

    if (lives <= 0)
    {
        currentState = GAME_OVER;
        return;
    }

    respawnEntities(currentState == PLAYING_2P);
    drawHUD();
    game_tick = 0;
    sleep(30);
}

static int dirty[MAP_HEIGHT][MAP_WIDTH];

static void markDirty(int x, int y)
{
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
        dirty[y][x] = 1;
}

static void renderFrame()
{
    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            dirty[y][x] = 0;

    markDirty(pacman.prev_x, pacman.prev_y);
    markDirty(pacman.x, pacman.y);
    for (int i = 0; i < NUM_GHOSTS; i++)
    {
        markDirty(ghosts[i].prev_x, ghosts[i].prev_y);
        markDirty(ghosts[i].x, ghosts[i].y);
    }

    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            if (dirty[y][x])
                redrawTile(x, y);

    drawEntity(&pacman);
    for (int i = 0; i < NUM_GHOSTS; i++)
        drawEntity(&ghosts[i]);
}

static int last_score = -1;
static int last_lives  = -1;

static void drawHUDIfChanged()
{
    if (score != last_score || lives != last_lives)
    {
        drawHUD();
        last_score = score;
        last_lives = lives;
    }
}

void gameLoop(void)
{
    drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);
    drawMap();
    drawHUD();
    last_score = score;
    last_lives = lives;

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

        moveEntity(&pacman);

        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            if (ghosts[i].returning)
                moveGhostAI(&ghosts[i], i);
            else if (ghosts[i].is_player_2)
                moveEntity(&ghosts[i]);
            else if (game_tick % GHOST_MOVE_INTERVAL == 0)
                moveGhostAI(&ghosts[i], i);
        }

        if (pacman.current_dir != NONE)
            pacman_mouth_frame = 1 - pacman_mouth_frame;
        else
            pacman_mouth_frame = 0;

        renderFrame();
        drawHUDIfChanged();

        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            int same_tile =
                (pacman.x == ghosts[i].x &&
                 pacman.y == ghosts[i].y);

            int crossed =
                (pacman.x == ghosts[i].prev_x &&
                 pacman.y == ghosts[i].prev_y &&
                 pacman.prev_x == ghosts[i].x &&
                 pacman.prev_y == ghosts[i].y);

            if (same_tile || crossed)
            {
                if (ghosts[i].frightened && !ghosts[i].in_house)
                {
                    score += SCORE_GHOST_EAT;
                    ghosts[i].frightened = 0;
                    ghosts[i].frighten_timer = 0;
                    ghosts[i].returning = 1;
                }
                else if (!ghosts[i].frightened && !ghosts[i].returning)
                {
                    handleCollision();

                    if (currentState == GAME_OVER)
                        break;

                    for (int j = 0; j < NUM_GHOSTS; j++)
                    {
                        ghosts[j].frightened = 0;
                        ghosts[j].frighten_timer = 0;
                        ghosts[j].returning = 0;
                    }

                    drawMap();
                    renderFrame();
                    drawHUD();
                    last_score = score;
                    last_lives = lives;

                    break;
                }
            }
        }

        if (dots_eaten >= total_dots)
            currentState = WIN;

        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            if (ghosts[i].frighten_timer > 0)
            {
                ghosts[i].frighten_timer--;
                if (ghosts[i].frighten_timer == 0)
                    ghosts[i].frightened = 0;
            }
        }

        game_tick++;
        sleep(2);
    }
}

void startPacman(void)
{
    while (1)
    {
        currentState = MENU;

        while (currentState == MENU)
        {
            drawMenuScreen();

            char key = getChar();
            if (key == '1')
            {
                currentState = PLAYING_1P;
            }
            else if (key == '2')
            {
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

        initGame(currentState == PLAYING_2P);

        playMelody(melodyFrequencies, melodyDurations, MELODY_NOTE_COUNT);
        gameLoop();

        drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);
        clearScreen();
        setTextSize(28);

        if(currentState == WIN) 
            drawWinScreen(score);
        else                    
            drawGameOverScreen(score);

        sleep(50);
        clearScreen();
        setTextSize(10);
        setTextColor(0xFFFFFF, 0x000000);

        return;
    }
}