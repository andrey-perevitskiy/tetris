#include "tetromino.h"

struct ttr *
ttr_alloc (enum ttr_form form, const sfTexture * blk_txt)
{
    unsigned i;
    struct ttr * t = malloc(sizeof(struct ttr));
    sfColor color;

    t->form = form;
    t->blk = malloc(sizeof(struct blk) * BLKQ);
    t->dir = DIR_DOWN;
    t->rot_state = RSTATE_ZERO;
    switch (form) {
         /* [3][0][1][2]
          */
        case TTR_FORM_I:
            t->blk[0].dvn = (sfVector2f) {0.f, 0.f};
            t->blk[1].dvn = (sfVector2f) {1.f, 0.f};
            t->blk[2].dvn = (sfVector2f) {2.f, 0.f};
            t->blk[3].dvn = (sfVector2f) {-1.f, 0.f};
            color = sfCyan;
            break;

         /* [1][2]
          * [0][3]
          */
        case TTR_FORM_O:
            t->blk[0].dvn = (sfVector2f) {0.f, 0.f};
            t->blk[1].dvn = (sfVector2f) {0.f, -1.f};
            t->blk[2].dvn = (sfVector2f) {1.f, -1.f};
            t->blk[3].dvn = (sfVector2f) {1.f, 0.f};
            color = sfYellow;
            break;

        /*    [1]
         * [3][0][2]
         */
        case TTR_FORM_T:
            t->blk[0].dvn = (sfVector2f) {0.f, 0.f};
            t->blk[1].dvn = (sfVector2f) {0.f, -1.f};
            t->blk[2].dvn = (sfVector2f) {1.f, 0.f};
            t->blk[3].dvn = (sfVector2f) {-1.f, 0.f};
            color = sfColor_fromRGB(0xa0, 0x20, 0xf0);
            break;

         /*    [1][2]
          * [3][0]
          */
        case TTR_FORM_S:
            t->blk[0].dvn = (sfVector2f) {0.f, 0.f};
            t->blk[1].dvn = (sfVector2f) {0.f, -1.f};
            t->blk[2].dvn = (sfVector2f) {1.f, -1.f};
            t->blk[3].dvn = (sfVector2f) {-1.f, 0.f};
            color = sfGreen;
            break;

        /* [3][1]
         *    [0][2]
         */
        case TTR_FORM_Z:
            t->blk[0].dvn = (sfVector2f) {0.f, 0.f};
            t->blk[1].dvn = (sfVector2f) {0.f, -1.f};
            t->blk[2].dvn = (sfVector2f) {1.f, 0.f};
            t->blk[3].dvn = (sfVector2f) {-1.f, -1.f};
            color = sfColor_fromRGB(0xff, 0x57, 0x33);
            break;

        /* [3]
         * [2][0][1]
         */
        case TTR_FORM_J:
            t->blk[0].dvn = (sfVector2f) {0.f, 0.f};
            t->blk[1].dvn = (sfVector2f) {1.f, 0.f};
            t->blk[2].dvn = (sfVector2f) {-1.f, 0.f};
            t->blk[3].dvn = (sfVector2f) {-1.f, -1.f};
            color = sfColor_fromRGB(0x41, 0x69, 0xe1);
            break;

        /*       [2]
         * [3][0][1]
         */
        case TTR_FORM_L:
            t->blk[0].dvn = (sfVector2f) {0.f, 0.f};
            t->blk[1].dvn = (sfVector2f) {1.f, 0.f};
            t->blk[2].dvn = (sfVector2f) {1.f, -1.f};
            t->blk[3].dvn = (sfVector2f) {-1.f, 0.f};
            color = sfColor_fromRGB(0xff, 0xa5, 0x00);
            break;
    }
    for (i = 0; i < BLKQ; i++) {
        t->blk[i].s = sfSprite_create();
        sfSprite_setTexture(t->blk[i].s, blk_txt, sfTrue);
        sfSprite_setPosition(t->blk[i].s,
            (sfVector2f) {WOFFSET + (t->blk[i].dvn.x + FSIZEX / 2 - 1) * BLKSIZE,
            (t->blk[i].dvn.y + 1) * BLKSIZE});
        sfSprite_setColor(t->blk[i].s, color);
    }

    return t;
}

void
ttr_get_pos (const struct ttr * t, sfVector2f pos [BLKQ])
{
    unsigned i;

    for (i = 0; i < BLKQ; i++)
        pos[i] = sfSprite_getPosition(t->blk[i].s);
}

void
ttr_set_pos (struct ttr * t, sfVector2f pos)
{
    unsigned i;

    for (i = 0; i < BLKQ; i++)
        sfSprite_setPosition(t->blk[i].s, (sfVector2f) {
            pos.x + t->blk[i].dvn.x * BLKSIZE,
            pos.y + t->blk[i].dvn.y * BLKSIZE});
}

void
ttr_set_scale (struct ttr * t, sfVector2f scale)
{
    unsigned i;

    for (i = 0; i < BLKQ; i++) {
        sfVector2f pos = sfSprite_getPosition(t->blk[i].s);
        sfVector2f scale_old = sfSprite_getScale(t->blk[i].s);

        sfSprite_setScale(t->blk[i].s, scale);
        sfSprite_setPosition(t->blk[i].s, (sfVector2f) {
            pos.x - t->blk[i].dvn.x * BLKSIZE * scale.x / scale_old.x,
            pos.y - t->blk[i].dvn.y * BLKSIZE * scale.y / scale_old.y});
    }
}

void
ttr_set_color (struct ttr * t, sfColor c)
{
    unsigned i;

    for (i = 0; i < BLKQ; i++)
        sfSprite_setColor(t->blk[i].s, c);
}

void
ttr_copy_pos (struct ttr * dest, const struct ttr * src)
{
    unsigned i;
    sfVector2f pos;

    for (i = 0; i < BLKQ; i++) {
        pos = sfSprite_getPosition(src->blk[0].s);
        sfSprite_setPosition(dest->blk[i].s, (sfVector2f) {
            pos.x + dest->blk[i].dvn.x * BLKSIZE,
            pos.y + dest->blk[i].dvn.y * BLKSIZE});
    }
}

void
ttr_swap (struct ttr ** src, struct ttr ** dest)
{
    struct ttr * buf;

    buf = *src;
    *src = *dest;
    *dest = buf;
}

void
ttr_move (struct ttr * t)
{
    unsigned i;
    sfVector2f sign;

    switch (t->dir) {
        case DIR_LEFT:
            sign = (sfVector2f) {-1.f, 0.f};
            break;

        case DIR_RIGHT:
            sign = (sfVector2f) {1.f, 0.f};
            break;

        case DIR_UP:
            sign = (sfVector2f) {0.f, -1.f};
            break;

        case DIR_DOWN:
            sign = (sfVector2f) {0.f, 1.f};
            break;
    }

    for (i = 0; i < BLKQ; i++) {
        sfVector2f pos = sfSprite_getPosition(t->blk[i].s);
        pos.x += BLKSIZE * sign.x;
        pos.y += BLKSIZE * sign.y;
        sfSprite_setPosition(t->blk[i].s, pos);
    }
}

void
ttr_move_dvn (struct ttr * t, sfVector2f dvn)
{
    unsigned i;

    for (i = 0; i < BLKQ; i++) {
        sfVector2f pos = sfSprite_getPosition(t->blk[i].s);
        pos.x += dvn.x * BLKSIZE;
        pos.y += dvn.y * BLKSIZE;
        sfSprite_setPosition(t->blk[i].s, pos);
    }
}

void
ttr_rotate_90 (struct ttr * t)
{
    unsigned i;
    sfVector2f pos;

    if (t->form == TTR_FORM_O)
        return;

    if (t->form == TTR_FORM_I) {
        switch (t->rot_state) {
            case RSTATE_ZERO:
                t->dir = DIR_RIGHT;
                break;

            case RSTATE_90:
                t->dir = DIR_DOWN;
                break;

            case RSTATE_180:
                t->dir = DIR_LEFT;
                break;

            case RSTATE_270:
                t->dir = DIR_UP;
                break;
        }
        ttr_move(t);
    }

    pos = sfSprite_getPosition(t->blk[0].s);
    for (i = 0; i < BLKQ; i++) {
        float x = t->blk[i].dvn.x;
        float y = t->blk[i].dvn.y;
        t->blk[i].dvn.x = x * cos(M_PI_2) - y * sin(M_PI_2);
        t->blk[i].dvn.y = x * sin(M_PI_2) + y * cos(M_PI_2);
        sfSprite_setPosition(t->blk[i].s,
            (sfVector2f) {pos.x + t->blk[i].dvn.x * BLKSIZE,
            pos.y + t->blk[i].dvn.y * BLKSIZE});
    }

    if (t->rot_state == RSTATE_270)
        t->rot_state = RSTATE_ZERO;
    else
        t->rot_state++;
}

void
ttr_drop (struct ttr * t_base, const struct ttr ** t, unsigned q)
{
    t_base->dir = DIR_DOWN;
    if (!ttr_check_collide_ground(t_base)
            && !ttr_check_collide_another(t_base, t, q))
        ttr_move(t_base);
}

void
ttr_hard_drop (struct ttr * t_base, const struct ttr ** t, unsigned q)
{
    t_base->dir = DIR_DOWN;
    while (!ttr_check_collide_ground(t_base)
            && !ttr_check_collide_another(t_base, t, q))
        ttr_move(t_base);
}

sfBool
ttr_check_collide_all_static (struct ttr ** t, unsigned q)
{
    t[q - 1]->dir = DIR_UP;
    return ttr_check_collide_ground(t[q - 1])
        || ttr_check_collide_walls(t[q - 1])
        || ttr_check_inside_another((const struct ttr **) t, q);
}

sfBool
ttr_test_kick (struct ttr ** t, unsigned q)
{
    unsigned i;
    sfVector2f dvn [BLKQ];
    sfBool canRotate = sfTrue;

    switch (t[q - 1]->rot_state) {
        case RSTATE_ZERO:
            // Wall kick data
            if (t[q - 1]->form == TTR_FORM_I) {
                dvn[0] = (sfVector2f) {-2.f, 0.f};
                dvn[1] = (sfVector2f) {1.f, 0.f};
                dvn[2] = (sfVector2f) {-2.f, 1.f};
                dvn[3] = (sfVector2f) {1.f, -2.f};
            }
            else {
                dvn[0] = (sfVector2f) {-1.f, 0.f};
                dvn[1] = (sfVector2f) {-1.f, -1.f};
                dvn[2] = (sfVector2f) {0.f, 2.f};
                dvn[3] = (sfVector2f) {-1.f, 2.f};
            }

            if (ttr_check_collide_all_static(t, q))
                for (i = 0; i < BLKQ; i++) {
                    ttr_move_dvn(t[q - 1], dvn[i]);
                    if (canRotate = !ttr_check_collide_all_static(t, q))
                        break;
                    else
                        ttr_move_dvn(t[q - 1],
                            (sfVector2f) {dvn[i].x * -1, dvn[i].y * -1});
                }
            break;

        case RSTATE_90:
            // Wall kick data
            if (t[q - 1]->form == TTR_FORM_I) {
                dvn[0] = (sfVector2f) {-1.f, 0.f};
                dvn[1] = (sfVector2f) {2.f, 0.f};
                dvn[2] = (sfVector2f) {-1.f, -2.f};
                dvn[3] = (sfVector2f) {2.f, 1.f};
            }
            else {
                dvn[0] = (sfVector2f) {1.f, 0.f};
                dvn[1] = (sfVector2f) {1.f, 1.f};
                dvn[2] = (sfVector2f) {0.f, -2.f};
                dvn[3] = (sfVector2f) {1.f, -2.f};
            }

            if (ttr_check_collide_all_static(t, q))
                for (i = 0; i < BLKQ; i++) {
                    ttr_move_dvn(t[q - 1], dvn[i]);
                    if (canRotate = !ttr_check_collide_all_static(t, q))
                        break;
                    else
                        ttr_move_dvn(t[q - 1],
                            (sfVector2f) {dvn[i].x * -1, dvn[i].y * -1});
                }
            break;

        case RSTATE_180:
            // Wall kick data
            if (t[q - 1]->form == TTR_FORM_I) {
                dvn[0] = (sfVector2f) {2.f, 0.f};
                dvn[1] = (sfVector2f) {-1.f, 0.f};
                dvn[2] = (sfVector2f) {2.f, -1.f};
                dvn[3] = (sfVector2f) {-1.f, 2.f};
            }
            else {
                dvn[0] = (sfVector2f) {1.f, 0.f};
                dvn[1] = (sfVector2f) {1.f, -1.f};
                dvn[2] = (sfVector2f) {0.f, 2.f};
                dvn[3] = (sfVector2f) {1.f, 2.f};
            }

            if (ttr_check_collide_all_static(t, q))
                for (i = 0; i < BLKQ; i++) {
                    ttr_move_dvn(t[q - 1], dvn[i]);
                    if (canRotate = !ttr_check_collide_all_static(t, q))
                        break;
                    else
                        ttr_move_dvn(t[q - 1],
                            (sfVector2f) {dvn[i].x * -1, dvn[i].y * -1});
                }
            break;

        case RSTATE_270:
            // Wall kick data
            if (t[q - 1]->form == TTR_FORM_I) {
                dvn[0] = (sfVector2f) {-1.f, 0.f};
                dvn[1] = (sfVector2f) {2.f, 0.f};
                dvn[2] = (sfVector2f) {-1.f, -1.f};
                dvn[3] = (sfVector2f) {2.f, 1.f};
            }
            else {
                dvn[0] = (sfVector2f) {-1.f, 0.f};
                dvn[1] = (sfVector2f) {-1.f, 1.f};
                dvn[2] = (sfVector2f) {0.f, -2.f};
                dvn[3] = (sfVector2f) {-1.f, -2.f};
            }

            if (ttr_check_collide_all_static(t, q))
                for (i = 0; i < BLKQ; i++) {
                    ttr_move_dvn(t[q - 1], dvn[i]);
                    if (canRotate = !ttr_check_collide_all_static(t, q))
                        break;
                    else
                        ttr_move_dvn(t[q - 1],
                            (sfVector2f) {dvn[i].x * -1, dvn[i].y * -1});
                }
            break;
    }

    return canRotate;
}

sfBool
ttr_check_collide_ground (const struct ttr * t)
{
    unsigned i;

    for (i = 0; i < BLKQ; i++) {
        sfVector2f pos = sfSprite_getPosition(t->blk[i].s);
        switch (t->dir) {
            case DIR_UP:
                if (pos.y > WSIZEY - BLKSIZE)
                    return sfTrue;
                break;

            default:
                if (pos.y == WSIZEY - BLKSIZE)
                    return sfTrue;
                break;
        }
    }

    return sfFalse;
}

sfBool
ttr_check_collide_walls (const struct ttr * t)
{
    unsigned i;

    for (i = 0; i < BLKQ; i++) {
        sfVector2f pos = sfSprite_getPosition(t->blk[i].s);
        switch (t->dir) {
            case DIR_LEFT:
                if (pos.x == WOFFSET)
                    return sfTrue;
                break;

            case DIR_RIGHT:
                if (pos.x == WSIZEX - BLKSIZE - WOFFSET)
                    return sfTrue;
                break;

            case DIR_UP:
                if (pos.x < WOFFSET || pos.x > WSIZEX - BLKSIZE - WOFFSET)
                    return sfTrue;
                break;

            default:
                break;
        }
    }

    return sfFalse;
}

sfBool
ttr_check_collide_another (const struct ttr * t_base, const struct ttr ** t, unsigned q)
{
    unsigned i, j, k;
    const struct ttr * base;

    if (q == 1)
        return sfFalse;

    if (t_base == NULL)
        base = t[q - 1];
    else
        base = t_base;

    for (i = 0; i < q - 1; i++)
        for (j = 0; j < BLKQ; j++) {
            sfVector2f pos_active = sfSprite_getPosition(base->blk[j].s);
            for (k = 0; k < BLKQ; k++) {
                if (t[i]->blk[k].s == NULL)
                    continue;

                sfVector2f pos = sfSprite_getPosition(t[i]->blk[k].s);
                switch (base->dir) {
                    case DIR_LEFT:
                        if (pos_active.x - BLKSIZE == pos.x
                                && pos_active.y == pos.y)
                            return sfTrue;
                        break;

                    case DIR_RIGHT:
                        if (pos_active.x + BLKSIZE == pos.x
                                && pos_active.y == pos.y)
                            return sfTrue;
                        break;

                    case DIR_DOWN:
                        if (pos_active.x == pos.x
                                && pos_active.y + BLKSIZE == pos.y)
                            return sfTrue;
                        break;

                    default:
                        break;
                }
            }
        }

    return sfFalse;
}

sfBool
ttr_check_inside_another (const struct ttr ** t, unsigned q)
{
    unsigned i, j, k;

    if (q == 1)
        return sfFalse;

    for (i = 0; i < q - 1; i++)
        for (j = 0; j < BLKQ; j++) {
            sfVector2f pos_active = sfSprite_getPosition(t[q - 1]->blk[j].s);
            for (k = 0; k < BLKQ; k++) {
                if (t[i]->blk[k].s == NULL)
                    continue;

                sfVector2f pos = sfSprite_getPosition(t[i]->blk[k].s);
                if (pos_active.x == pos.x && pos_active.y == pos.y)
                    return sfTrue;
            }
        }

    return sfFalse;
}

void
ttr_line_clear (struct ttr ** t, unsigned * q)
{
    unsigned i, j, k, l;
    sfBool isLine, isExists;
    sfSprite ** blks_spr [FSIZEX];
    struct ttr * buf;

    for (i = 0; i < FSIZEY; i++) {
        for (j = 0; j < FSIZEX; j++) {
            isLine = sfFalse;
            for (k = 0; k < *q + 1; k++)
                for (l = 0; l < BLKQ; l++) {
                    if (t[k]->blk[l].s == NULL)
                        continue;

                    sfVector2f pos = sfSprite_getPosition(t[k]->blk[l].s);
                    if (pos.x == WOFFSET + j * BLKSIZE && pos.y == i * BLKSIZE) {
                        blks_spr[j] = &t[k]->blk[l].s;
                        isLine = sfTrue;
                        break;
                    }
                }
            if (!isLine)
                break;
        }
        if (!isLine)
            continue;

        for (j = 0; j < FSIZEX; j++) {
            // Animation of the explosion of the block.
            sfSprite_destroy(*blks_spr[j]);
            *blks_spr[j] = NULL;
        }

        for (j = 0; j < *q + 1; j++)
            for (k = 0; k < BLKQ; k++) {
                if (t[j]->blk[k].s == NULL)
                    continue;

                sfVector2f pos = sfSprite_getPosition(t[j]->blk[k].s);
                if (pos.y < i * BLKSIZE)
                    sfSprite_setPosition(t[j]->blk[k].s,
                        (sfVector2f) {pos.x, pos.y + BLKSIZE});
            }
    }

    k = -1;
    for (i = 0; i < *q + 1; i++) {
        isExists = sfFalse;
        for (j = 0; j < BLKQ; j++)
            if (t[i]->blk[j].s != NULL) {
                isExists = sfTrue;
                break;
            }
        if (isExists) {
            k++;
            buf = t[k];
            t[k] = t[i];
            t[i] = buf;
        }
    }

    if ((int) k != -1)
        *q -= *q - k;
}

void
ttr_draw (const struct ttr * t, sfRenderWindow * w)
{
    unsigned i;

    for (i = 0; i < BLKQ; i++)
        if (t->blk[i].s != NULL)
            sfRenderWindow_drawSprite(w, t->blk[i].s, NULL);
}

void
ttr_destroy (struct ttr * t)
{
    unsigned i;

    for (i = 0; i < BLKQ; i++)
        if (t->blk[i].s != NULL)
            sfSprite_destroy(t->blk[i].s);

    free(t->blk);
    free(t);
}
