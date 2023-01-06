#ifndef HUD_H

#include "tetromino.h"

void hud_draw_next (struct ttr * t, unsigned mlt, sfRenderWindow * w);
void hud_draw_hold (struct ttr * t, sfRenderWindow * w);

#define HUD_H
#endif
