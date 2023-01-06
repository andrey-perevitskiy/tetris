#include "bag.h"

void
bag_gen_item (struct bag * b, unsigned idx, const sfTexture * t)
{
    if (b->lim_I == 12) {
        b->forms[idx] = TTR_FORM_I;
        b->lim_I = 0;
    }
    else if (b->lim_SZ == 4) {
        do
            b->forms[idx] = RAND(TTR_FORM_I, TTR_FORM_L);
        while (b->forms[idx] == TTR_FORM_S || b->forms[idx] == TTR_FORM_Z);
        b->lim_SZ = 0;
    }
    else
        b->forms[idx] = RAND(TTR_FORM_I, TTR_FORM_L);
    switch (b->forms[idx]) {
        case TTR_FORM_I:
            b->lim_I = 0;
            break;

        case TTR_FORM_S:
        case TTR_FORM_Z:
            b->lim_SZ++;
            b->lim_I++;
            break;

        default:
            b->lim_I++;
            break;
    }
    b->ttr[idx] = ttr_alloc(b->forms[idx], t);
}

struct ttr *
bag_next (struct bag * b, const sfTexture * t)
{
    if (b->cursor > 0) {
        bag_gen_item(b, b->cursor - 1, t);
        if (b->cursor == ITEMQ)
            b->cursor = 0;
    }
    return b->ttr[b->cursor++];
}

void
bag_draw_items (struct bag * b, sfRenderWindow * w)
{
    unsigned i;

    if (b->cursor > ITEMQ - ITEMDSPLQ) {
        for (i = b->cursor; i < ITEMQ; i++)
            hud_draw_next(b->ttr[i], i - b->cursor, w);
        for (i = 0; i < ITEMDSPLQ - (ITEMQ - b->cursor); i++)
            hud_draw_next(b->ttr[i], i + ITEMQ - b->cursor, w);
    }
    else
        for (i = b->cursor; i < b->cursor + ITEMDSPLQ; i++)
            hud_draw_next(b->ttr[i], i - b->cursor, w);
}
