#include <stdint.h>
#include <libc.h>

#define BIG_SCALE   8  
#define BIG_GAP     4
#define BIG_W       (5 * BIG_SCALE + BIG_GAP) 
#define BIG_H       (7 * BIG_SCALE)
#define Y_RESOLUTION 768
#define X_RESOLUTION 1024

#define COLOR_PACMAN 0xFFFF00
#define COLOR_BLINKY 0xFF0000
#define COLOR_PINKY 0xFFB8FF
#define COLOR_INKY 0x00FFFF
#define COLOR_CLYDE 0xFFB852

static const uint8_t font5x7[][7] = {
 {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}, // A
 {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E},
 {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},
 {0x1E,0x09,0x09,0x09,0x09,0x09,0x1E},
 {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F},
 {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},
 {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F},
 {0x11,0x11,0x11,0x1F,0x11,0x11,0x11},
 {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E},
 {0x07,0x02,0x02,0x02,0x02,0x12,0x0C},
 {0x11,0x12,0x14,0x18,0x14,0x12,0x11},
 {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},
 {0x11,0x1B,0x15,0x11,0x11,0x11,0x11},
 {0x11,0x19,0x15,0x13,0x11,0x11,0x11},
 {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E},
 {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10},
 {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D},
 {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},
 {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E},
 {0x1F,0x04,0x04,0x04,0x04,0x04,0x04},
 {0x11,0x11,0x11,0x11,0x11,0x11,0x0E},
 {0x11,0x11,0x11,0x11,0x11,0x0A,0x04},
 {0x11,0x11,0x11,0x15,0x15,0x1B,0x11},
 {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11},
 {0x11,0x11,0x0A,0x04,0x04,0x04,0x04},
 {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}, // Z
 {0x00,0x00,0x00,0x00,0x00,0x00,0x00},  // espacio (índice 26) */
 {0x04,0x04,0x04,0x04,0x04,0x00,0x04},  // ! (índice 27) */
 {0x00,0x00,0x00,0x1F,0x00,0x00,0x00},  // - (índice 28) */
 {0x00,0x00,0x00,0x00,0x00,0x00,0x04},  // . (índice 29) */
 {0x00,0x04,0x00,0x00,0x04,0x00,0x00},  // : 
 {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, // digitos
 {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, 
 {0x0E,0x11,0x01,0x06,0x08,0x10,0x1F}, 
 {0x1F,0x01,0x02,0x06,0x01,0x11,0x0E}, 
 {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, 
 {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, 
 {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, 
 {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},  
 {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, 
 {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C},
};

static int charIndex(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= '0' && c <= '9') return (c - '0') + 31; 
    
    if (c == ' ')  return 26;
    if (c == '!')  return 27;
    if (c == '-')  return 28;
    if (c == '.')  return 29;
    if (c == ':')  return 30;
    
    return 26; // espacio default
}


static void drawBigChar(char c, int px, int py, uint32_t color, int scale)
{
    int idx = charIndex(c);
    for (int row = 0; row < 7; row++)
    {
        uint8_t bits = font5x7[idx][row];
        for (int col = 0; col < 5; col++)
        {
            if ((bits >> (4 - col)) & 1)
            {
                int x1 = px + col * scale;
                int y1 = py + row * scale;
                drawRectangle(x1, y1, x1 + scale - 1, y1 + scale - 1, color);
            }
        }
    }
}


static int textWidth(const char *s, int scale)
{
    int len = 0;
    while (*s++) len++;
    return len * (5 * scale + 4);
}


static void drawBigTextCentered(const char *s, int py, uint32_t color, int scale)
{
    int w = textWidth(s, scale);
    int px = (X_RESOLUTION - w) / 2;
    while (*s)
    {
        drawBigChar(*s, px, py, color, scale);
        px += 5 * scale + 4;
        s++;
    }
}

static void drawBigNumberCentered(int num, int py, uint32_t color, int scale){
    char buf[16];
    int i = 0;
    if (num == 0) { buf[i++] = '0'; }
    else {
        int tmp = num, cnt = 0, rev[10];
        while (tmp > 0) { rev[cnt++] = tmp % 10; tmp /= 10; }
        for (int j = cnt - 1; j >= 0; j--) buf[i++] = '0' + rev[j];
    }
    buf[i] = '\0';
    drawBigTextCentered(buf, py, color, scale);
}

static void drawDotBorder(uint32_t color)
{
    int dot = 6, gap = 18;

    for (int x = gap; x < X_RESOLUTION - gap; x += gap)
    {
        drawRectangle(x, 20, x + dot, 20 + dot, color);
        drawRectangle(x, Y_RESOLUTION - 28, x + dot, Y_RESOLUTION - 22, color);
    }

    for (int y = gap; y < Y_RESOLUTION - gap; y += gap)
    {
        drawRectangle(20, y, 20 + dot, y + dot, color);
        drawRectangle(X_RESOLUTION - 28, y, X_RESOLUTION - 22, y + dot, color);
    }
}

static void drawDecoPacman(int cx, int cy, int size, uint32_t color, int open)
{
    static const int bmp_open[8][8] = {
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}};
    static const int bmp_closed[8][8] = {
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,0,0}
    };
    const int (*bmp)[8] = open ? bmp_open : bmp_closed;
    int ps = size / 8;
    int ox = cx - size / 2;
    int oy = cy - size / 2;
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            if (bmp[y][x])
                drawRectangle(ox + x*ps, oy + y*ps,
                              ox + x*ps + ps - 1, oy + y*ps + ps - 1, color);
}


static void drawDecoGhost(int cx, int cy, int size, uint32_t color)
{
    static const int bmp[8][8] = {
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,0,0,1,1,0,0,1} 
    };
    int ps = size / 8;
    int ox = cx - size / 2;
    int oy = cy - size / 2;

    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            if (bmp[y][x])
                drawRectangle(ox + x*ps, oy + y*ps,
                              ox + x*ps + ps - 1, oy + y*ps + ps - 1, color);

    drawRectangle(ox + 1*ps, oy + 2*ps, ox + 3*ps - 1, oy + 4*ps - 1, 0xFFFFFF);
    drawRectangle(ox + 5*ps, oy + 2*ps, ox + 7*ps - 1, oy + 4*ps - 1, 0xFFFFFF);

    drawRectangle(ox + 2*ps, oy + 3*ps, ox + 3*ps - 1, oy + 4*ps - 1, 0x0000FF);
    drawRectangle(ox + 6*ps, oy + 3*ps, ox + 7*ps - 1, oy + 4*ps - 1, 0x0000FF);
}

static void drawHLine(int y, int x1, int x2, int thick, uint32_t color)
{
    drawRectangle(x1, y, x2, y + thick - 1, color);
}


void drawMenuScreen(){
    drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);

    drawDotBorder(COLOR_PACMAN);

    drawBigTextCentered("PAC-MAN", 72, 0x0000AA, 10);
    drawBigTextCentered("PAC-MAN", 68, COLOR_PACMAN, 10);

    drawHLine(155, 80, X_RESOLUTION - 80, 3, COLOR_PACMAN);

    drawDecoPacman(120, 110, 64, COLOR_PACMAN, 1);

    drawDecoGhost(X_RESOLUTION - 210, 110, 48, COLOR_BLINKY);
    drawDecoGhost(X_RESOLUTION - 145, 110, 48, COLOR_PINKY);
    drawDecoGhost(X_RESOLUTION - 80,  110, 48, COLOR_INKY);

    drawBigTextCentered("SELECT GAME MODE", 185, 0xFFFFFF, 5);
    drawHLine(230, 200, X_RESOLUTION - 200, 2, 0x444444);

    drawRectangle(220, 250, X_RESOLUTION - 220, 330, 0x001133);
    drawRectangle(220, 250, X_RESOLUTION - 220, 252, 0x00FFFF);
    drawRectangle(220, 328, X_RESOLUTION - 220, 330, 0x00FFFF);

    drawBigTextCentered("1 PLAYER", 262, 0x00FFFF, 6);

    drawRectangle(220, 355, X_RESOLUTION - 220, 435, 0x1A0011);
    drawRectangle(220, 355, X_RESOLUTION - 220, 357, 0xFF0000);
    drawRectangle(220, 433, X_RESOLUTION - 220, 435, 0xFF0000);

    drawBigTextCentered("2 PLAYERS", 367, 0xFF0000, 6);

    drawBigTextCentered("Q - QUIT", 465, 0x666666, 4);

    drawHLine(510, 80, X_RESOLUTION - 80, 2, 0x333333);
    drawBigTextCentered("WASD TO MOVE", 525, 0x444444, 3);
    drawBigTextCentered("IJKL FOR PLAYER 2", 555, 0x444444, 3);

    int dot_y = 620;
    uint32_t dot_colors[4] = {COLOR_PACMAN, COLOR_BLINKY, COLOR_PINKY, COLOR_INKY};
    for (int i = 0; i < 20; i++)
    {
        int dx = 120 + i * 40;
        drawRectangle(dx, dot_y, dx + 8, dot_y + 8, dot_colors[i % 4]);
    }

}

void drawGameOverScreen(int final_score){
    drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);

    drawDotBorder(0xFF0000);

    drawDecoGhost(180, 160, 80, COLOR_BLINKY);
    drawDecoGhost(X_RESOLUTION - 180, 160, 80, COLOR_BLINKY);

    drawBigTextCentered("GAME", 222, 0x550000, 12);
    drawBigTextCentered("GAME", 218, 0xFF0000, 12);

    drawBigTextCentered("OVER", 340, 0x550000, 12);
    drawBigTextCentered("OVER", 336, 0xFF0000, 12);

    drawHLine(462, 100, X_RESOLUTION - 100, 3, 0xFF0000);

    drawBigTextCentered("FINAL SCORE", 480, 0xFFFFFF, 5);
    drawBigNumberCentered(final_score, 530, COLOR_PACMAN, 9);

    uint32_t ghost_colors[4] = {COLOR_BLINKY, COLOR_PINKY, COLOR_INKY, COLOR_CLYDE};
    for (int i = 0; i < 16; i++)
    {
        int dx = 80 + i * 56;
        drawRectangle(dx, 660, dx + 10, 670, ghost_colors[i % 4]);
    }
}


void drawWinScreen(int final_score) {
    drawRectangle(0, 0, X_RESOLUTION - 1, Y_RESOLUTION - 1, 0x000000);

    drawDotBorder(COLOR_PACMAN);

    drawDecoGhost(160,  130, 64, 0x0000CC);
    drawDecoGhost(280,  130, 48, 0x0000AA);
    drawDecoGhost(X_RESOLUTION - 160, 130, 64, 0x0000CC);
    drawDecoGhost(X_RESOLUTION - 280, 130, 48, 0x0000AA);

    drawBigTextCentered("YOU", 210, 0xAA8800, 12);
    drawBigTextCentered("YOU", 206, COLOR_PACMAN, 12);

    drawBigTextCentered("WIN!", 328, 0xAA8800, 12);
    drawBigTextCentered("WIN!", 324, 0xFFFF00, 12);

    uint32_t star_colors[] = {0xFFFFFF, COLOR_PACMAN, 0xFFFFFF, COLOR_INKY, 0xFFFFFF};
    int star_xs[] = {80, 200, X_RESOLUTION/2, X_RESOLUTION-200, X_RESOLUTION-80};
    int star_ys[] = {270, 200, 195, 200, 270};
    for (int i = 0; i < 5; i++)
    {
        int sx = star_xs[i], sy = star_ys[i];
        int ss = 5;
        drawRectangle(sx - ss, sy, sx + ss, sy + 2, star_colors[i]);
        drawRectangle(sx, sy - ss, sx + 2, sy + ss, star_colors[i]);
    }

    drawHLine(454, 100, X_RESOLUTION - 100, 3, COLOR_PACMAN);

    drawBigTextCentered("FINAL SCORE", 472, 0xFFFFFF, 5);
    drawBigNumberCentered(final_score, 520, COLOR_PACMAN, 9);

    uint32_t dot_colors[] = {COLOR_PACMAN, COLOR_BLINKY, COLOR_PINKY,
                              COLOR_INKY,   COLOR_CLYDE,  0xFFFFFF};
    for (int i = 0; i < 18; i++)
    {
        int dx = 92 + i * 48;
        drawRectangle(dx, 636, dx + 12, 648, dot_colors[i % 6]);
    }

    drawBigTextCentered("CONGRATULATIONS!", 665, 0xFFFFFF, 5);
}