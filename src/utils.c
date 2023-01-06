#include "utils.h"

void
grid_draw (sfRenderWindow * w, sfColor c)
{
    int i;
    sfVector2i size = (sfVector2i) {(FSIZEX + 1) * 2, (FSIZEY + 1) * 2};
    sfVertex x [size.x], y [size.y];

    for (i = 0; i < size.x; i += 2) {
        x[i].position = (sfVector2f) {WOFFSET + i / 2 * BLKSIZE, 0};
        x[i + 1].position = (sfVector2f) {WOFFSET + i / 2 * BLKSIZE, WSIZEY};
        x[i + 1].color = x[i].color = c;
    }
    for (i = 0; i < size.y; i += 2) {
        y[i].position = (sfVector2f) {WOFFSET, i / 2 * BLKSIZE};
        y[i + 1].position = (sfVector2f) {WSIZEX - WOFFSET, i / 2 * BLKSIZE};
        y[i + 1].color = y[i].color = c;
    }
    sfRenderWindow_drawPrimitives(w, x, size.x, sfLines, NULL);
    sfRenderWindow_drawPrimitives(w, y, size.y, sfLines, NULL);
}
