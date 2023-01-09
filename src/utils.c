#include "utils.h"

void
grid_draw (sfRenderWindow * w, sfColor c, unsigned offset)
{
    int i;
#define SIZEX (FSIZEX + 1) * 2
#define SIZEY (FSIZEY + 1) * 2
    // You want a constant expression, C++ compiler? Come on, get it!
    sfVertex x [SIZEX], y [SIZEY];

    for (i = 0; i < SIZEX; i += 2) {
        x[i].position = (sfVector2f) {offset + i / 2 * BLKSIZE, 0};
        x[i + 1].position = (sfVector2f) {offset + i / 2 * BLKSIZE, WSIZEY};
        x[i + 1].color = x[i].color = c;
    }
    for (i = 0; i < SIZEY; i += 2) {
        y[i].position = (sfVector2f) {offset, i / 2 * BLKSIZE};
        y[i + 1].position = (sfVector2f) {WSIZEX - offset, i / 2 * BLKSIZE};
        y[i + 1].color = y[i].color = c;
    }
    sfRenderWindow_drawPrimitives(w, x, SIZEX, sfLines, NULL);
    sfRenderWindow_drawPrimitives(w, y, SIZEY, sfLines, NULL);
}
