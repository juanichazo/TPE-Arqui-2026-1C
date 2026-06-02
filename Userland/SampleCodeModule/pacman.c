#include <stdint.h>
#include <libc.h>
#include <pacman_audio.h>
#include <pacman_utils.h>

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
    int vis_x, vis_y;
    int prev_vis_x, prev_vis_y;
    Direction next_dir;
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
        int lx = HUD_X + i * 28;
        int ly = HUD_Y + 62;
        for (int y = 0; y < 12; y++)
        {
            for (int x = 0; x < 12; x++)
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
    int thick = 2;

    switch (tile_type)
    {
    case TILE_WALL_H:
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor);
        break;

    case TILE_WALL_V:
        drawRect(px + mid - 1, py, thick, BLOCK_SIZE, wallColor);
        break;

    case TILE_CORNER_TL:                                                 
        drawRect(px + mid - 1, py + mid - 1, mid + 1, thick, wallColor); 
        drawRect(px + mid - 1, py + mid - 1, thick, mid + 1, wallColor); 
        break;

    case TILE_CORNER_TR:                                                 
        drawRect(px, py + mid - 1, mid + 1, thick, wallColor);           
        drawRect(px + mid - 1, py + mid - 1, thick, mid + 1, wallColor); 
        break;

    case TILE_CORNER_BL:                                                 
        drawRect(px + mid - 1, py + mid - 1, mid + 1, thick, wallColor); 
        drawRect(px + mid - 1, py, thick, mid, wallColor);              
        break;

    case TILE_CORNER_BR:                                       
        drawRect(px, py + mid - 1, mid + 1, thick, wallColor); 
        drawRect(px + mid - 1, py, thick, mid, wallColor);  
        break;

    case TILE_T_UP:                                               
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor); 
        drawRect(px + mid - 1, py, thick, mid, wallColor);  
        break;

    case TILE_T_DOWN:                                             
        drawRect(px, py + mid - 1, BLOCK_SIZE, thick, wallColor); 
        drawRect(px + mid - 1, py + mid, thick, mid, wallColor);  
        break;

    default:
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
    int (*bmp)[12] = (pacman_mouth_frame == 0) ? pacman_bmp_open : pacman_bmp_closed;
    int px = e->vis_x;
    int py = e->vis_y;

    if (e == &pacman){
        for (int y = 0; y < 12; y++){
            for (int x = 0; x < 12; x++){
                if (!bmp[y][x]){
                    continue;
                }

                int tx = x, ty = y;

                switch (e->current_dir){
                case LEFT:
                    tx = 11 - x;
                    break;

                case DOWN:
                    tx = 11 - y;
                    ty = x;
                    break;
                case UP:
                    tx = y;
                    ty = 11 - x;
                    break;
                default:
                    break;
                }
                drawRect(px + tx, py + ty, 1, 1, e->color);
            }
        }
    }
    else{
    if (e->returning){
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 12; x++)
                if (ghost_eyes_only_bmp[y][x])
                    drawRect(px + x, py + y, 1, 1, 0xFFFFFF);
        return;
    }

    if (e->frightened){
        uint32_t body_color;
        if (e->frighten_timer < FRIGHTEN_FLASH_THRESHOLD && (game_tick / 4) % 2 == 0)
            body_color = 0xFFFFFF;
        else
            body_color = COLOR_FRIGHTENED;

        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 12; x++)
                if (ghost_bmp[y][x])
                    drawRect(px + x, py + y, 1, 1, body_color);

        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 12; x++){
                if (ghost_scared_eyes_bmp[y][x])
                    drawRect(px + x, py + y, 1, 1, 0xFF8080);
                if (ghost_scared_mouth_bmp[y][x])
                    drawRect(px + x, py + y, 1, 1, 0xFF8080);
            }
    }
    else{
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 12; x++)
                if (ghost_bmp[y][x])
                    drawRect(px + x, py + y, 1, 1, e->color);

        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 12; x++){
                if (ghost_eyes_bmp[y][x])
                    drawRect(px + x, py + y, 1, 1, 0xFFFFFF);
                if (ghost_pupil_bmp[y][x])
                    drawRect(px + x, py + y, 1, 1, 0x0066FF);
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
    pacman.vis_x = pacman.x * BLOCK_SIZE;
    pacman.vis_y = pacman.y * BLOCK_SIZE;
    pacman.prev_vis_x = pacman.vis_x;
    pacman.prev_vis_y = pacman.vis_y;
    pacman.next_dir = NONE;

    for (int i = 0; i < NUM_GHOSTS; i++)
    {
        ghosts[i].vis_x = ghosts[i].x * BLOCK_SIZE;
        ghosts[i].vis_y = ghosts[i].y * BLOCK_SIZE;
        ghosts[i].prev_vis_x = ghosts[i].vis_x;
        ghosts[i].prev_vis_y = ghosts[i].vis_y;
        ghosts[i].next_dir = NONE;
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

            playBackgroundMelody(frightenedLoopFrequencies, frightenedLoopDurations, FRIGHTENED_NOTE_COUNT);
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
            ghost->current_dir = valid[(ghost->x * 7 + ghost->y * 13 + game_tick) % count]; // pseudo-random
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
    playBackgroundMelody(deathFrequencies, deathDurations, DEATH_NOTE_COUNT);
    lives--;

    // Limpiar todos los sprites antes del flash de muerte.
    // La colisión se detecta en espacio lógico pero los fantasmas pueden estar
    // en posiciones vis distintas, dejando artifacts superpuestos con pacman.
    drawMap();
    for (int i = 0; i < NUM_GHOSTS; i++)
        drawEntity(&ghosts[i]);

    uint32_t saved_color = pacman.color;
    pacman.color = 0xFF0000;
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

static void markVisDirty(int vx, int vy)
{
    int tx = vx / BLOCK_SIZE;
    int ty = vy / BLOCK_SIZE;
    markDirty(tx,     ty);
    markDirty(tx + 1, ty);
    markDirty(tx,     ty + 1);
    markDirty(tx + 1, ty + 1);
}

static void renderFrame()
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

static void advanceVis(Entity *e)
{
    e->prev_vis_x = e->vis_x;
    e->prev_vis_y = e->vis_y;

    int tx = e->x * BLOCK_SIZE;
    int ty = e->y * BLOCK_SIZE;

    // Túnel: si la distancia es mayor que media pantalla, snap instantáneo.
    // Animar el cruce causaría que vis_x salga del mapa, generando artifacts.
    int half_w = (MAP_WIDTH * BLOCK_SIZE) / 2;
    if (tx - e->vis_x > half_w || e->vis_x - tx > half_w)
        e->vis_x = tx;

    if      (e->vis_x < tx) { e->vis_x += VIS_SPEED; if (e->vis_x > tx) e->vis_x = tx; }
    else if (e->vis_x > tx) { e->vis_x -= VIS_SPEED; if (e->vis_x < tx) e->vis_x = tx; }

    if      (e->vis_y < ty) { e->vis_y += VIS_SPEED; if (e->vis_y > ty) e->vis_y = ty; }
    else if (e->vis_y > ty) { e->vis_y -= VIS_SPEED; if (e->vis_y < ty) e->vis_y = ty; }
}

static int isVisAligned(Entity *e)
{
    return e->vis_x == e->x * BLOCK_SIZE && e->vis_y == e->y * BLOCK_SIZE;
}

static int dirPassable(int x, int y, Direction d)
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

void gameLoop(void)
{
    drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);
    drawMap();
    drawHUD();
    playMelody(melodyFrequencies, melodyDurations, MELODY_NOTE_COUNT);
    last_score = score;
    last_lives = lives;

    while (currentState != GAME_OVER && currentState != WIN)
    {
        char key = getCharNoWait();
        if (key != 0)
        {
            if      (key == 'w') pacman.next_dir = UP;
            else if (key == 's') pacman.next_dir = DOWN;
            else if (key == 'a') pacman.next_dir = LEFT;
            else if (key == 'd') pacman.next_dir = RIGHT;
            else if (key == 'q') currentState = GAME_OVER;

            if (ghosts[0].is_player_2)
            {
                if      (key == 'i') ghosts[0].next_dir = UP;
                else if (key == 'k') ghosts[0].next_dir = DOWN;
                else if (key == 'j') ghosts[0].next_dir = LEFT;
                else if (key == 'l') ghosts[0].next_dir = RIGHT;
            }
        }

        advanceVis(&pacman);
        for (int i = 0; i < NUM_GHOSTS; i++)
            advanceVis(&ghosts[i]);

        if (isVisAligned(&pacman))
        {
            if (pacman.next_dir != NONE && dirPassable(pacman.x, pacman.y, pacman.next_dir))
            {
                pacman.current_dir = pacman.next_dir;
                pacman.next_dir = NONE;
            }
            moveEntity(&pacman);

            if (pacman.current_dir != NONE)
                pacman_mouth_frame = 1 - pacman_mouth_frame;
            else
                pacman_mouth_frame = 0;

            for (int i = 0; i < NUM_GHOSTS; i++)
            {
                if (ghosts[i].frighten_timer > 0)
                {
                    ghosts[i].frighten_timer--;
                    if (ghosts[i].frighten_timer == 0)
                        ghosts[i].frightened = 0;
                }
            }

            if (dots_eaten >= total_dots)
                currentState = WIN;

            game_tick++;
        }

        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            if (!isVisAligned(&ghosts[i])) continue;

            if (ghosts[i].returning)
            {
                moveGhostAI(&ghosts[i], i);
            }
            else if (ghosts[i].is_player_2)
            {
                if (ghosts[i].next_dir != NONE && dirPassable(ghosts[i].x, ghosts[i].y, ghosts[i].next_dir))
                {
                    ghosts[i].current_dir = ghosts[i].next_dir;
                    ghosts[i].next_dir = NONE;
                }
                moveEntity(&ghosts[i]);
            }
            else
            {
                moveGhostAI(&ghosts[i], i);
            }
        }

        for (int i = 0; i < NUM_GHOSTS; i++)
        {
            int same_tile = (pacman.x == ghosts[i].x && pacman.y == ghosts[i].y);
            int crossed   = (pacman.x == ghosts[i].prev_x && pacman.y == ghosts[i].prev_y &&
                             pacman.prev_x == ghosts[i].x  && pacman.prev_y == ghosts[i].y);

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
                    pacman.vis_x = pacman.x * BLOCK_SIZE;
                    pacman.vis_y = pacman.y * BLOCK_SIZE;
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

        renderFrame();
        drawHUDIfChanged();
        sleep(1);
    }
}

void startPacman(void)
{
    while (1)
    {
        currentState = MENU;
        drawMenuScreen();

        while (currentState == MENU)
        {

            char key = getCharNoWait();
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