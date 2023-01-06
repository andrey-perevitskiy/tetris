#ifndef BAG_H

#include <time.h>
#include "hud.h"
#include "utils.h"

#define ITEMQ 7
#define ITEMDSPLQ 6

#if ITEMDSPLQ >= ITEMQ
#   error ITEMDSPLQ >= ITEMQ
#endif

struct bag {
    enum ttr_form forms [ITEMQ];
    unsigned lim_I;
    unsigned lim_SZ;
    unsigned cursor;
    struct ttr * ttr [ITEMQ];
};

void bag_gen_item (struct bag * b, unsigned idx, const sfTexture * t);
struct ttr * bag_next (struct bag * b, const sfTexture * t);
void bag_draw_items (struct bag * b, sfRenderWindow * w);

#define BAG_H
#endif
