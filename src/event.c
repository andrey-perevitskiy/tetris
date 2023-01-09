#include "event.h"

static void
ghost_realloc (struct ttr ** t_base, const sfTexture * blk_ghost_txt,
    struct ttr ** t, unsigned q)
{
    ttr_destroy(*t_base);
    *t_base = ttr_alloc(t[q - 1]->form, blk_ghost_txt);
    ttr_set_color(*t_base, sfColor_fromRGB(0x88, 0x88, 0x88));
    ttr_copy_pos(*t_base, t[q - 1]);
    ttr_hard_drop(*t_base, (const struct ttr **) t, q);
}

static sfBool
check_gameover (const struct ttr * t)
{
    sfVector2f pos [BLKQ];
    sfBool flag;
    unsigned i;

    ttr_get_pos(t, pos);
    flag = sfTrue;
    for (i = 0; i < BLKQ; i++)
        if (pos[i].y >= BLKSIZE) {
            flag = sfFalse;
            break;
        }

    return flag;
}

int
evt_newgame (void)
{
    sfVideoMode mode = {
        WSIZEX - WOFFSET * 2, WSIZEY, 32
    };
    sfRenderWindow * w;
    sfEvent evt;
    sfTexture * logo_txt, * blk_txt, * blk_ghost_txt;
    sfSprite * logo_spr;
    sfClock * clock = sfClock_create();
    float t = 0.f;
    struct ttr * ttr [(FSIZEX / 2) * (FSIZEY / 2)], * ttr_ghost,
        * ttr_hold = NULL;
    unsigned i, q = 0;
    sfBool isGreeting = sfTrue, shouldFixTtr = sfFalse, onHold = sfFalse,
        canHold = sfTrue, isTopOut = sfFalse;
    struct bag bag = {0};

    w = sfRenderWindow_create(mode, "Tetris", sfClose, NULL);
    if (w == NULL) {
        puts("Failed to create window.");
        return -1;
    }

    // Load logo texture
    logo_txt = sfTexture_createFromFile("res/logo.png", NULL);
    if (logo_txt == NULL) {
        puts("Failed to load logo texture.");
        return -1;
    }
    logo_spr = sfSprite_create();
    sfSprite_setTexture(logo_spr, logo_txt, sfFalse);

    // Load block texture
    blk_txt = sfTexture_createFromFile("res/block.png", NULL);
    if (blk_txt == NULL) {
        puts("Failed to load block texture.");
        return -1;
    }

    // Load ghost block texture
    blk_ghost_txt = sfTexture_createFromFile("res/block_ghost.png", NULL);
    if (blk_ghost_txt == NULL) {
        puts("Failed to load ghost block texture.");
        return -1;
    }

    srand(time(NULL));
    for (i = 0; i < ITEMQ; i++)
        bag_gen_item(&bag, i, blk_txt);
    ttr[q] = bag_next(&bag, blk_txt);

    ttr_ghost = ttr_alloc(ttr[q]->form, blk_ghost_txt);
    ttr_set_color(ttr_ghost, sfColor_fromRGB(0x88, 0x88, 0x88));
    ttr_copy_pos(ttr_ghost, ttr[q]);
    ttr_hard_drop(ttr_ghost, (const struct ttr **) ttr, q + 1);
    while (sfRenderWindow_isOpen(w)) {
        t += sfClock_getElapsedTime(clock).microseconds;
        sfClock_restart(clock);

        if (isGreeting)
            if (t > 3000000.f) {
                isGreeting = sfFalse;
                sfTexture_destroy(logo_txt);
                sfSprite_destroy(logo_spr);
                sfRenderWindow_destroy(w);
                mode.width = WSIZEX;
                mode.height = WSIZEY;
                w = sfRenderWindow_create(mode, "Tetris", sfClose, NULL);
            }
            else
                goto greeting;

        while (sfRenderWindow_pollEvent(w, &evt))
            switch (evt.type) {
                case sfEvtClosed:
                    sfRenderWindow_close(w);
                    break;

                case sfEvtKeyPressed:
                    switch (evt.key.code) {
                        case sfKeyLeft:
                            ttr[q]->dir = DIR_LEFT;
                            if (!ttr_check_collide_walls(ttr[q])
                                    && !ttr_check_collide_another(NULL,
                                        (const struct ttr **) ttr, q + 1)) {
                                ttr_move(ttr[q]);

                                ttr_copy_pos(ttr_ghost, ttr[q]);
                                ttr_hard_drop(ttr_ghost,
                                    (const struct ttr **) ttr, q + 1);

                                if (shouldFixTtr) {
                                    shouldFixTtr = sfFalse;
                                    t = 0.f;
                                }
                            }
                            break;

                        case sfKeyRight:
                            ttr[q]->dir = DIR_RIGHT;
                            if (!ttr_check_collide_walls(ttr[q])
                                    && !ttr_check_collide_another(NULL,
                                        (const struct ttr **) ttr, q + 1)) {
                                ttr_move(ttr[q]);

                                ttr_copy_pos(ttr_ghost, ttr[q]);
                                ttr_hard_drop(ttr_ghost,
                                    (const struct ttr **) ttr, q + 1);

                                if (shouldFixTtr) {
                                    shouldFixTtr = sfFalse;
                                    t = 0.f;
                                }
                            }
                            break;

                        case sfKeyUp:
                            ttr[q]->dir = DIR_UP;
                            ttr_rotate_90(ttr[q]);
                            if (ttr_test_kick(ttr, q + 1)) {
                                ttr_rotate_90(ttr_ghost);
                                ttr_copy_pos(ttr_ghost, ttr[q]);
                                ttr_hard_drop(ttr_ghost,
                                    (const struct ttr **) ttr, q + 1);

                                if (shouldFixTtr) {
                                    shouldFixTtr = sfFalse;
                                    t = 0.f;
                                }
                            }
                            else {
                                ttr_rotate_90(ttr[q]);
                                ttr_rotate_90(ttr[q]);
                                ttr_rotate_90(ttr[q]);
                            }
                            break;

                        case sfKeyDown:
                            ttr_drop(ttr[q], (const struct ttr **) ttr, q + 1);
                            break;

                        case sfKeySpace:
                            if (shouldFixTtr)
                                break;

                            ttr_hard_drop(ttr[q],
                                (const struct ttr **) ttr, q + 1);
                            shouldFixTtr = sfTrue;
                            t = 0.f;
                            break;

                        case sfKeyLShift:
                            if (!canHold)
                                break;

                            if (onHold) {
                                ttr_swap(&ttr[q], &ttr_hold);
                                ttr_copy_pos(ttr[q], ttr_hold);
                                ttr_test_kick(ttr, q + 1);

                                ghost_realloc(&ttr_ghost, blk_ghost_txt, ttr, q + 1);

                                onHold = sfFalse;
                                canHold = sfFalse;
                            }
                            else {
                                if (ttr_hold != NULL) {
                                    ttr_swap(&ttr[q], &ttr_hold);
                                    ttr_copy_pos(ttr[q], ttr_hold);
                                    ttr_test_kick(ttr, q + 1);
                                }
                                else {
                                    ttr_hold = ttr[q];
                                    ttr[q] = bag_next(&bag, blk_txt);
                                    ttr_copy_pos(ttr_hold, ttr[q]);
                                }

                                ghost_realloc(&ttr_ghost, blk_ghost_txt, ttr, q + 1);

                                onHold = sfTrue;
                                canHold = sfFalse;
                            }
                            while (ttr_hold->rot_state != RSTATE_ZERO)
                                ttr_rotate_90(ttr_hold);
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }

        if (t > 1000000.f || (shouldFixTtr && t > 200000.f)) {
            if (shouldFixTtr) {
                shouldFixTtr = sfFalse;
                canHold = sfTrue;

                ttr_line_clear(ttr, &q);

                // Fix the active tetromino
                q++;
                ttr[q] = bag_next(&bag, blk_txt);
                if (!ttr_test_kick(ttr, q + 1)) {
                    ttr[q]->dir = DIR_UP;
                    while (ttr_check_inside_another(
                            (const struct ttr **) ttr, q + 1))
                        ttr_move(ttr[q]);

                    // Check game over
                    isTopOut = check_gameover(ttr[q]);
                }

                ghost_realloc(&ttr_ghost, blk_ghost_txt, ttr, q + 1);
            }
            else {
                ttr[q]->dir = DIR_DOWN;
                if (!ttr_check_collide_ground(ttr[q])
                        && !ttr_check_collide_another(NULL,
                            (const struct ttr **) ttr, q + 1))
                    ttr_move(ttr[q]);
                else
                    shouldFixTtr = sfTrue;
            }

            t = 0.f;
        }

        if (isTopOut)
            break;

        sfRenderWindow_clear(w, sfBlack);
        if (isGreeting)
greeting:
            sfRenderWindow_drawSprite(w, logo_spr, NULL);
        else {
            grid_draw(w, sfColor_fromRGB(0x20, 0x20, 0x20), WOFFSET);
            ttr_draw(ttr_ghost, w);
            for (i = 0; i < q + 1; i++)
                ttr_draw(ttr[i], w);
            bag_draw_items(&bag, w);
            if (ttr_hold != NULL)
                hud_draw_hold(ttr_hold, w);
        }
        sfRenderWindow_display(w);
    }

    sfTexture_destroy(blk_txt);
    sfTexture_destroy(blk_ghost_txt);
    for (i = 0; i < q + 1; i++)
        ttr_destroy(ttr[i]);
    ttr_destroy(ttr_ghost);
    sfRenderWindow_destroy(w);

    return 0;
}
