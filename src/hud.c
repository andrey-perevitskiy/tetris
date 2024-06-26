#include "hud.h"

static sfVector2f pos [BLKQ];

void
hud_draw_next (struct ttr * t, unsigned mlt, sfRenderWindow * w)
{
    ttr_get_pos(t, pos);
    ttr_set_pos(t, (sfVector2f) {WSIZEX - WOFFSET + BLKSIZE,
        BLKSIZE + BLKSIZE * 2.f * mlt});
    ttr_set_scale(t, (sfVector2f) {0.5f, 0.5f});
    ttr_draw(t, w);
    ttr_set_scale(t, (sfVector2f) {1.f, 1.f});
    ttr_set_pos(t, pos[0]);
}

void
hud_draw_hold (struct ttr * t, sfRenderWindow * w)
{
    ttr_get_pos(t, pos);
    ttr_set_pos(t, (sfVector2f) {WOFFSET - BLKSIZE * 2.f, BLKSIZE});
    ttr_set_scale(t, (sfVector2f) {0.5f, 0.5f});
    ttr_draw(t, w);
    ttr_set_scale(t, (sfVector2f) {1.f, 1.f});
    ttr_set_pos(t, pos[0]);
}
