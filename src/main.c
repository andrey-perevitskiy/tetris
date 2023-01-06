#include <stdio.h>
#include <time.h>
#include "bag.h"

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

int
main (void)
{
    sfVideoMode mode = {
        WSIZEX, WSIZEY, 32
    };
    sfRenderWindow * win;
    sfEvent evt;
    sfTexture * blk_txt, * blk_ghost_txt;
    sfClock * clock = sfClock_create();
    float t = 0.f;
    struct ttr * ttr [(FSIZEX / 2) * (FSIZEY / 2)], * ttr_ghost,
        * ttr_hold = NULL;
    unsigned i, q = 0;
    sfBool shouldFixTtr = sfFalse, onHold = sfFalse, canHold = sfTrue,
        isTopOut;
    struct bag bag = {0};

    win = sfRenderWindow_create(mode, "Tetris", sfClose, NULL);
    if (win == NULL) {
        puts("Failed to create window.");
        return -1;
    }

    // Main menu



    ///////////////////////////////////////////////////////////////////////////

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
    while (sfRenderWindow_isOpen(win)) {
        t += sfClock_getElapsedTime(clock).microseconds;
        sfClock_restart(clock);

        while (sfRenderWindow_pollEvent(win, &evt))
            switch (evt.type) {
                case sfEvtClosed:
                    sfRenderWindow_close(win);
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
                            while (ttr_hold->rot_state != ROT_STATE_ZERO)
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

                // Fix the tetromino
                q++;
                ttr[q] = bag_next(&bag, blk_txt);
                if (!ttr_test_kick(ttr, q + 1)) {
                    ttr[q]->dir = DIR_UP;
                    while (ttr_check_inside_another(
                            (const struct ttr **) ttr, q + 1))
                        ttr_move(ttr[q]);

                    // Check game over
                    sfVector2f pos [BLKQ];
                    ttr_get_pos(ttr[q], pos);
                    isTopOut = sfTrue;
                    for (i = 0; i < BLKQ; i++)
                        if (pos[i].y >= BLKSIZE) {
                            isTopOut = sfFalse;
                            break;
                        }
                    if (isTopOut)
                        printf("Game over.\n");
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

        sfRenderWindow_clear(win, sfBlack);
        grid_draw(win, sfColor_fromRGB(0x20, 0x20, 0x20));
        ttr_draw(ttr_ghost, win);
        for (i = 0; i < q + 1; i++)
            ttr_draw(ttr[i], win);
        bag_draw_items(&bag, win);
        if (ttr_hold != NULL)
            hud_draw_hold(ttr_hold, win);
        sfRenderWindow_display(win);
    }

    sfTexture_destroy(blk_txt);
    sfTexture_destroy(blk_ghost_txt);
    for (i = 0; i < q + 1; i++)
        ttr_destroy(ttr[i]);
    ttr_destroy(ttr_ghost);
    sfRenderWindow_destroy(win);

    return 0;
}
