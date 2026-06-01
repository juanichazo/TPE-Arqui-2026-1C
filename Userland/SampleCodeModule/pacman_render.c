#include <stdint.h>
#include <libc.h>
#include <pacman_map.h>
#include "pacman_render.h"

#define SPRITE_SIZE 12

/* ---- HUD (privado a este módulo) ---- */
#define DPIX  4
#define DGAP  2
#define DSTEP (3 * DPIX + DGAP)
#define HUD_X 10
#define HUD_Y 20

/* ---- Sprites ---- */
static int pacman_bmp_open[SPRITE_SIZE][SPRITE_SIZE] = {
    {0,0,0,1,1,1,1,1,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0},
    {1,1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,1,1,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,1,0,0,0,0},
};

static int pacman_bmp_closed[SPRITE_SIZE][SPRITE_SIZE] = {
    {0,0,0,1,1,1,1,1,1,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,1,1,1,1,1,1,0,0,0},
};

static int ghost_bmp[SPRITE_SIZE][SPRITE_SIZE] = {
    {0,0,1,1,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,0,0,1,1,0,0,1,1,1},
    {1,0,0,0,0,1,1,0,0,0,0,1},
};

static int ghost_eyes_bmp[SPRITE_SIZE][SPRITE_SIZE] = {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,0,1,1,0,0,0},
    {0,1,1,1,1,0,1,1,1,1,0,0},
    {0,1,1,1,1,0,1,1,1,1,0,0},
    {0,0,1,1,0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
};

static int ghost_pupil_bmp[SPRITE_SIZE][SPRITE_SIZE] = {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,0,1,0,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
};

static int ghost_scared_eyes_bmp[SPRITE_SIZE][SPRITE_SIZE] = {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
};

static int ghost_scared_mouth_bmp[SPRITE_SIZE][SPRITE_SIZE] = {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,0,1,1,0,1,1,1,0},
    {0,1,0,0,1,0,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
};

static int ghost_eyes_only_bmp[SPRITE_SIZE][SPRITE_SIZE] = {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,0,0,0,1,1,0,0,0},
    {0,1,1,0,0,0,0,1,1,0,0,0},
    {0,0,1,0,0,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
};

static const uint8_t digit_bmp[10][5] = {
    {7,5,5,5,7}, {2,6,2,2,7}, {7,1,7,4,7}, {7,1,7,1,7}, {5,5,7,1,1},
    {7,4,7,1,7}, {7,4,7,5,7}, {7,1,1,1,1}, {7,5,7,5,7}, {7,5,7,1,7},
};

/* ---- Estado interno ---- */
static int dirty[MAP_HEIGHT][MAP_WIDTH];
static int last_score = -1;
static int last_lives  = -1;

/* ---- Primitiva base ---- */
void drawRect(int startX, int startY, int width, int height, uint32_t color)
{
    int x1 = (startX * SCALE) + OFFSET_X;
    int y1 = (startY * SCALE) + OFFSET_Y;
    int x2 = x1 + (width  * SCALE) - 1;
    int y2 = y1 + (height * SCALE) - 1;
    drawRectangle(x1, y1, x2, y2, color);
}

/* Dibuja todos los píxeles activos de un bitmap con el color dado.
   Elimina el loop 12×12 que se repetía en cada estado del fantasma. */
static void drawBitmap(int px, int py, int bmp[SPRITE_SIZE][SPRITE_SIZE], uint32_t color)
{
    for (int y = 0; y < SPRITE_SIZE; y++)
        for (int x = 0; x < SPRITE_SIZE; x++)
            if (bmp[y][x])
                drawRect(px + x, py + y, 1, 1, color);
}

/* ---- Mapa ---- */
static void drawWallTile(int px, int py, int tile_type)
{
    uint32_t wallColor = 0x0000FF;
    int mid   = BLOCK_SIZE / 2;
    int thick = 2;
    drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, 0x000000);
    switch (tile_type)
    {
    case TILE_WALL_H:
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor); break;
    case TILE_WALL_V:
        drawRect(px + mid - 1, py, thick, BLOCK_SIZE, wallColor); break;
    case TILE_CORNER_TL:
        drawRect(px + mid - 1, py + mid - 1, mid + 1, thick, wallColor);
        drawRect(px + mid - 1, py + mid - 1, thick, mid + 1, wallColor); break;
    case TILE_CORNER_TR:
        drawRect(px, py + mid - 1, mid + 1, thick, wallColor);
        drawRect(px + mid - 1, py + mid - 1, thick, mid + 1, wallColor); break;
    case TILE_CORNER_BL:
        drawRect(px + mid - 1, py + mid - 1, mid + 1, thick, wallColor);
        drawRect(px + mid - 1, py, thick, mid, wallColor); break;
    case TILE_CORNER_BR:
        drawRect(px, py + mid - 1, mid + 1, thick, wallColor);
        drawRect(px + mid - 1, py, thick, mid, wallColor); break;
    case TILE_T_UP:
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor);
        drawRect(px + mid - 1, py, thick, mid, wallColor); break;
    case TILE_T_DOWN:
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor);
        drawRect(px + mid - 1, py + mid, thick, mid, wallColor); break;
    default:
        drawRect(px + mid - 1, py + mid - 1, thick, thick, wallColor); break;
    }
}

static void redrawTile(int x, int y)
{
    int px = x * BLOCK_SIZE;
    int py = y * BLOCK_SIZE;
    if (map[y][x] == 1)
        drawWallTile(px, py, pacman_tilemap[y + TILEMAP_ROW_OFFSET][x]);
    else
    {
        drawRect(px, py, BLOCK_SIZE, BLOCK_SIZE, 0x000000);
        if      (map[y][x] == 2) drawRect(px + 5, py + 5, 2, 2, 0xFFFFFF);
        else if (map[y][x] == 3) drawRect(px + 4, py + 4, 4, 4, 0xFFFFFF);
    }
}

void drawMap(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            redrawTile(x, y);
}

/* ---- HUD ---- */
static void drawDigit(int digit, int px, int py, uint32_t color)
{
    for (int row = 0; row < 5; row++)
    {
        uint8_t bits = digit_bmp[digit][row];
        for (int col = 0; col < 3; col++)
        {
            uint32_t c = ((bits >> (2 - col)) & 1) ? color : 0x000000;
            drawRectangle(px + col*DPIX, py + row*DPIX,
                          px + col*DPIX + DPIX - 1, py + row*DPIX + DPIX - 1, c);
        }
    }
}

static void drawNumber(int num, int px, int py, uint32_t color)
{
    char digs[8];
    int cnt = 0;
    if (num == 0) { drawDigit(0, px, py, color); return; }
    int tmp = num;
    while (tmp > 0 && cnt < 7) { digs[cnt++] = (char)(tmp % 10); tmp /= 10; }
    for (int i = cnt - 1; i >= 0; i--)
        drawDigit(digs[i], px + (cnt - 1 - i) * DSTEP, py, color);
}

void drawHUD(void)
{
    drawRectangle(HUD_X, HUD_Y,      HUD_X + 140, HUD_Y + 6,  0xFFFFFF);
    drawRectangle(HUD_X, HUD_Y + 10, HUD_X + 140, HUD_Y + 40, 0x000000);
    drawNumber(score, HUD_X, HUD_Y + 12, 0xFFFFFF);

    drawRectangle(HUD_X, HUD_Y + 50, HUD_X + 140, HUD_Y + 56, COLOR_PACMAN);
    drawRectangle(HUD_X, HUD_Y + 60, HUD_X + 140, HUD_Y + 90, 0x000000);

    for (int i = 0; i < lives && i < 5; i++)
    {
        int lx = HUD_X + i * 28;
        int ly = HUD_Y + 62;
        for (int y = 0; y < SPRITE_SIZE; y++)
            for (int x = 0; x < SPRITE_SIZE; x++)
                if (pacman_bmp_closed[y][x])
                    drawRectangle(lx + x*2, ly + y*2, lx + x*2+1, ly + y*2+1, COLOR_PACMAN);
    }

    last_score = score;
    last_lives = lives;
}

void drawHUDIfChanged(void)
{
    if (score != last_score || lives != last_lives)
        drawHUD();
}

/* ---- Entidades ---- */
void drawEntity(Entity *e)
{
    int px = e->vis_x;
    int py = e->vis_y;

    if (e == &pacman)
    {
        int (*bmp)[SPRITE_SIZE] = (pacman_mouth_frame == 0) ? pacman_bmp_open : pacman_bmp_closed;
        for (int y = 0; y < SPRITE_SIZE; y++)
        {
            for (int x = 0; x < SPRITE_SIZE; x++)
            {
                if (!bmp[y][x]) continue;
                int tx = x, ty = y;
                switch (e->current_dir)
                {
                case LEFT:  tx = SPRITE_SIZE - 1 - x; break;
                case DOWN:  tx = SPRITE_SIZE - 1 - y; ty = x; break;
                case UP:    tx = y; ty = SPRITE_SIZE - 1 - x; break;
                default: break;
                }
                drawRect(px + tx, py + ty, 1, 1, e->color);
            }
        }
        return;
    }

    if (e->returning)
    {
        drawBitmap(px, py, ghost_eyes_only_bmp, 0xFFFFFF);
        return;
    }

    if (e->frightened)
    {
        uint32_t body = (e->frighten_timer < FRIGHTEN_FLASH_THRESHOLD && (game_tick / 4) % 2 == 0)
                        ? 0xFFFFFF : COLOR_FRIGHTENED;
        drawBitmap(px, py, ghost_bmp,              body);
        drawBitmap(px, py, ghost_scared_eyes_bmp,  0xFF8080);
        drawBitmap(px, py, ghost_scared_mouth_bmp, 0xFF8080);
        return;
    }

    drawBitmap(px, py, ghost_bmp,       e->color);
    drawBitmap(px, py, ghost_eyes_bmp,  0xFFFFFF);
    drawBitmap(px, py, ghost_pupil_bmp, 0x0066FF);
}

/* ---- Sistema de tiles sucios ---- */
static void markDirty(int x, int y)
{
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
        dirty[y][x] = 1;
}

static void markVisDirty(int vx, int vy)
{
    int tx = vx / BLOCK_SIZE;
    int ty = vy / BLOCK_SIZE;
    markDirty(tx,     ty);
    markDirty(tx + 1, ty);
    markDirty(tx,     ty + 1);
    markDirty(tx + 1, ty + 1);
}

void renderFrame(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            dirty[y][x] = 0;

    markVisDirty(pacman.prev_vis_x, pacman.prev_vis_y);
    markVisDirty(pacman.vis_x,      pacman.vis_y);
    for (int i = 0; i < NUM_GHOSTS; i++)
    {
        markVisDirty(ghosts[i].prev_vis_x, ghosts[i].prev_vis_y);
        markVisDirty(ghosts[i].vis_x,      ghosts[i].vis_y);
    }

    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            if (dirty[y][x])
                redrawTile(x, y);

    drawEntity(&pacman);
    for (int i = 0; i < NUM_GHOSTS; i++)
        drawEntity(&ghosts[i]);
}

/* ---- Interpolación visual ---- */
void advanceVis(Entity *e)
{
    e->prev_vis_x = e->vis_x;
    e->prev_vis_y = e->vis_y;

    int tx = e->x * BLOCK_SIZE;
    int ty = e->y * BLOCK_SIZE;

    /* Túnel: snap instantáneo para evitar animación off-screen */
    int half_w = (MAP_WIDTH * BLOCK_SIZE) / 2;
    if (tx - e->vis_x > half_w || e->vis_x - tx > half_w)
        e->vis_x = tx;

    if      (e->vis_x < tx) { e->vis_x += VIS_SPEED; if (e->vis_x > tx) e->vis_x = tx; }
    else if (e->vis_x > tx) { e->vis_x -= VIS_SPEED; if (e->vis_x < tx) e->vis_x = tx; }

    if      (e->vis_y < ty) { e->vis_y += VIS_SPEED; if (e->vis_y > ty) e->vis_y = ty; }
    else if (e->vis_y > ty) { e->vis_y -= VIS_SPEED; if (e->vis_y < ty) e->vis_y = ty; }
}

int isVisAligned(Entity *e)
{
    return e->vis_x == e->x * BLOCK_SIZE && e->vis_y == e->y * BLOCK_SIZE;
}
