#include <libc.h>
#include <pacman_map.h>
#include "pacman_logic.h"
#include "pacman_render.h"

/* ---- Datos de fantasmas ---- */
static const int      GHOST_RELEASE[NUM_GHOSTS]        = {0, 20, 40, 60};
static const int      GHOST_START_X[NUM_GHOSTS]        = {13, 11, 13, 15};
static const int      GHOST_START_Y[NUM_GHOSTS]        = {11, 14, 14, 14};
static const int      GHOST_START_IN_HOUSE[NUM_GHOSTS] = {0, 1, 1, 1};
static const uint32_t GHOST_COLORS[NUM_GHOSTS]         = {COLOR_BLINKY, COLOR_PINKY,
                                                           COLOR_INKY, COLOR_CLYDE};

/* ---- Utilidades ---- */
static int tileToLogicValue(int tile)
{
    switch (tile)
    {
    case TILE_PELLET:       return 2;
    case TILE_POWER_PELLET: return 3;
    case TILE_WALL_H: case TILE_WALL_V:
    case TILE_CORNER_TL: case TILE_CORNER_TR:
    case TILE_CORNER_BL: case TILE_CORNER_BR:
    case TILE_T_UP: case TILE_T_DOWN: return 1;
    default: return 0;
    }
}

static int absi(int n) { return n < 0 ? -n : n; }

static Direction oppositeDir(Direction d)
{
    switch (d)
    {
    case UP:    return DOWN;
    case DOWN:  return UP;
    case LEFT:  return RIGHT;
    case RIGHT: return LEFT;
    default:    return NONE;
    }
}

int dirPassable(int x, int y, Direction d)
{
    static const int ddx[4] = {0, 0, -1, 1};
    static const int ddy[4] = {-1, 1, 0, 0};
    int nx = x + ddx[d];
    int ny = y + ddy[d];
    if (nx < 0) nx = MAP_WIDTH - 1;
    else if (nx >= MAP_WIDTH) nx = 0;
    if (ny < 0 || ny >= MAP_HEIGHT) return 0;
    return map[ny][nx] != 1;
}

/* Busca la mejor dirección hacia (tx,ty) usando distancia Manhattan.
   gate_passable=1 permite que el fantasma cruce la puerta de la casa. */
static Direction findBestDir(Entity *ghost, int tx, int ty, int gate_passable)
{
    static const int dx[4]       = {0, 0, -1, 1};
    static const int dy[4]       = {-1, 1, 0, 0};
    static const Direction dirs[4] = {UP, DOWN, LEFT, RIGHT};

    Direction opp  = oppositeDir(ghost->current_dir);
    Direction best = NONE;
    int best_dist  = 99999;

    for (int i = 0; i < 4; i++)
    {
        if (dirs[i] == opp && ghost->current_dir != NONE) continue;

        int nx = ghost->x + dx[i];
        int ny = ghost->y + dy[i];
        if (nx < 0) nx = MAP_WIDTH - 1;
        else if (nx >= MAP_WIDTH) nx = 0;
        if (ny < 0 || ny >= MAP_HEIGHT) continue;

        int is_wall = (map[ny][nx] == 1);
        if (pacman_tilemap[ny + TILEMAP_ROW_OFFSET][nx] == TILE_GHOST_GATE)
            is_wall = gate_passable ? 0 : 1;
        if (is_wall) continue;

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
            if (dirs[i] == opp && ghost->current_dir != NONE) continue;
            int nx = ghost->x + dx[i];
            int ny = ghost->y + dy[i];
            if (nx < 0) nx = MAP_WIDTH - 1;
            else if (nx >= MAP_WIDTH) nx = 0;
            if (ny < 0 || ny >= MAP_HEIGHT) continue;
            if (map[ny][nx] != 1) { best = dirs[i]; break; }
        }
        if (best == NONE) best = opp;
    }
    return best;
}

/* ---- Movimiento ---- */
void moveEntity(Entity *e)
{
    if (e->current_dir == NONE) return;

    e->prev_x = e->x;
    e->prev_y = e->y;

    int nx = e->x, ny = e->y;
    switch (e->current_dir)
    {
    case UP:    ny--; break;
    case DOWN:  ny++; break;
    case LEFT:  nx--; break;
    case RIGHT: nx++; break;
    default: break;
    }

    if (nx < 0)           nx = MAP_WIDTH - 1;
    else if (nx >= MAP_WIDTH) nx = 0;
    if (map[ny][nx] == 1) return;

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
                ghosts[i].frightened    = 1;
                ghosts[i].frighten_timer = FRIGHTEN_DURATION;
            }
        }
    }
}

void moveGhostAI(Entity *ghost, int ghost_id)
{
    static const int dx[4]       = {0, 0, -1, 1};
    static const int dy[4]       = {-1, 1, 0, 0};
    static const Direction dirs[4] = {UP, DOWN, LEFT, RIGHT};

    if (ghost->returning)
    {
        Direction best = findBestDir(ghost, 13, 14, 1);
        if (best != NONE) { ghost->current_dir = best; moveEntity(ghost); }
        if (absi(ghost->x - 13) + absi(ghost->y - 14) <= 1)
        {
            ghost->returning   = 0;
            ghost->in_house    = 1;
            ghost->x           = 13;
            ghost->y           = 14;
            ghost->current_dir = NONE;
        }
        return;
    }

    if (game_tick < GHOST_RELEASE[ghost_id]) return;

    if (ghost->frightened && !ghost->in_house)
    {
        Direction opp = oppositeDir(ghost->current_dir);
        Direction valid[4];
        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            if (dirs[i] == opp && ghost->current_dir != NONE) continue;
            int nx = ghost->x + dx[i];
            int ny = ghost->y + dy[i];
            if (nx < 0) nx = MAP_WIDTH - 1;
            else if (nx >= MAP_WIDTH) nx = 0;
            if (ny < 0 || ny >= MAP_HEIGHT) continue;
            if (map[ny][nx] == 1) continue;
            if (pacman_tilemap[ny + TILEMAP_ROW_OFFSET][nx] == TILE_GHOST_GATE) continue;
            valid[count++] = dirs[i];
        }
        ghost->current_dir = (count > 0)
            ? valid[(ghost->x * 7 + ghost->y * 13 + game_tick) % count]
            : opp;
        moveEntity(ghost);
        return;
    }

    int tx, ty;
    if (ghost->in_house)
    {
        tx = 13; ty = 11;
        if (ghost->y <= 11) ghost->in_house = 0;
    }
    else
    {
        tx = pacman.x; ty = pacman.y;
        if (ghost_id == 1) /* Pinky: apunta 4 tiles adelante */
        {
            switch (pacman.current_dir)
            {
            case UP:    ty -= 4; break;
            case DOWN:  ty += 4; break;
            case LEFT:  tx -= 4; break;
            case RIGHT: tx += 4; break;
            default: break;
            }
        }
        else if (ghost_id == 2) /* Inky: flanqueo con Blinky */
        {
            int mid_x = pacman.x + (pacman.current_dir == RIGHT ?  2 :
                                    pacman.current_dir == LEFT  ? -2 : 0);
            int mid_y = pacman.y + (pacman.current_dir == DOWN  ?  2 :
                                    pacman.current_dir == UP    ? -2 : 0);
            tx = 2 * mid_x - ghosts[0].x;
            ty = 2 * mid_y - ghosts[0].y;
        }
        else if (ghost_id == 3) /* Clyde: dispersión si está cerca */
        {
            if (absi(ghost->x - pacman.x) + absi(ghost->y - pacman.y) < 8)
            { tx = 0; ty = MAP_HEIGHT - 1; }
        }
    }

    Direction best = findBestDir(ghost, tx, ty, ghost->in_house);
    if (best != NONE) { ghost->current_dir = best; moveEntity(ghost); }
}

/* ---- Inicialización ---- */
static void respawnEntities(int two_players_mode)
{
    pacman.x           = 13;
    pacman.y           = 23;
    pacman.current_dir = NONE;
    pacman.color       = COLOR_PACMAN;
    pacman.prev_x      = pacman.x;
    pacman.prev_y      = pacman.y;
    pacman.vis_x       = pacman.x * BLOCK_SIZE;
    pacman.vis_y       = pacman.y * BLOCK_SIZE;
    pacman.prev_vis_x  = pacman.vis_x;
    pacman.prev_vis_y  = pacman.vis_y;
    pacman.next_dir    = NONE;

    for (int i = 0; i < NUM_GHOSTS; i++)
    {
        ghosts[i].x             = GHOST_START_X[i];
        ghosts[i].y             = GHOST_START_Y[i];
        ghosts[i].color         = GHOST_COLORS[i];
        ghosts[i].in_house      = GHOST_START_IN_HOUSE[i];
        ghosts[i].current_dir   = NONE;
        ghosts[i].is_player_2   = (i == 0) ? two_players_mode : 0;
        ghosts[i].prev_x        = ghosts[i].x;
        ghosts[i].prev_y        = ghosts[i].y;
        ghosts[i].frightened    = 0;
        ghosts[i].frighten_timer = 0;
        ghosts[i].returning     = 0;
        ghosts[i].vis_x         = ghosts[i].x * BLOCK_SIZE;
        ghosts[i].vis_y         = ghosts[i].y * BLOCK_SIZE;
        ghosts[i].prev_vis_x    = ghosts[i].vis_x;
        ghosts[i].prev_vis_y    = ghosts[i].vis_y;
        ghosts[i].next_dir      = NONE;
    }
}

void initGame(int two_players_mode)
{
    score = 0; lives = STARTING_LIVES; game_tick = 0;
    dots_eaten = 0; total_dots = 0;

    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            map[y][x] = tileToLogicValue(pacman_tilemap[y + TILEMAP_ROW_OFFSET][x]);
            if (map[y][x] == 2 || map[y][x] == 3) total_dots++;
        }

    respawnEntities(two_players_mode);
}

/* ---- Colisión ---- */
void handleCollision(void)
{
    lives--;

    drawMap();
    for (int i = 0; i < NUM_GHOSTS; i++)
        drawEntity(&ghosts[i]);

    uint32_t saved = pacman.color;
    pacman.color = 0xFF0000;
    drawEntity(&pacman);
    sleep(20);
    pacman.color = saved;

    if (lives <= 0) { currentState = GAME_OVER; return; }

    respawnEntities(currentState == PLAYING_2P);
    drawHUD();
    game_tick = 0;
    sleep(30);
}
