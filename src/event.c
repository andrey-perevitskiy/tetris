#include "event.h"
#include "bag.h"
#include <string.h>
#include "text_slide.h"
#include <SFML/Audio.h>

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

static void
text_init (sfText * text, const char * str, sfFont * font, int csize)
{
    sfText_setFont(text, font);
    sfText_setCharacterSize(text, csize);
    sfText_setString(text, str);
}

static void
text_center (sfText * text)
{
    sfFloatRect rect;

    rect = sfText_getLocalBounds(text);
    rect.left += sfText_getGlobalBounds(text).width / 2;
    sfText_setOrigin(text, (sfVector2f) {rect.left, rect.top});
    sfText_setColor(text, sfWhite);
}

static sfVideoMode
get_resolution (enum win_res res, sfBool borders)
{
    unsigned int width;
    sfVideoMode mode;

    if (borders)
        width = WSIZEX;
    else
        width = WSIZEX - WOFFSET * 2;

    mode.bitsPerPixel = 32;
    switch (res) {
    case RES_1x:
        mode.width = width;
        mode.height = WSIZEY;
        break;
    case RES_2x:
        mode.width = width * 2;
        mode.height = WSIZEY * 2;
        break;
    case RES_4x:
        mode.width = width * 4;
        mode.height = WSIZEY * 4;
        break;
    case RES_8x:
        mode.width = width * 8;
        mode.height = WSIZEY * 8;
        break;
    }

    return mode;
}

int
evt_menu (struct options * opts)
{
    sfVideoMode mode;
    sfRenderWindow * w;
    sfEvent evt;
    sfTexture * logo_txt;
    sfSprite * logo_spr;
    sfFont * font;
    sfText * newgame_text, * options_text, * exit_text;
    sfText * texts [3];
    sfFloatRect bounds, mouse_rect;
    int i;
    sfBool is_menu_item_selected = sfFalse, is_exit = sfFalse;
    sfVector2f mouse_pos_mapped;

    mode = get_resolution(opts->res, sfFalse);
    w = sfRenderWindow_create(mode, "Tetris", sfClose, NULL);
    if (w == NULL) {
        puts("Failed to create window");
        return -1;
    }
    sfRenderWindow_setPosition(w,
        (sfVector2i) {sfVideoMode_getDesktopMode().width / 2 - mode.width / 2,
            sfVideoMode_getDesktopMode().height / 2 - mode.height / 2});

    logo_txt = sfTexture_createFromFile("res/logo.png", NULL);
    if (logo_txt == NULL) {
        puts("Failed to load logo");
        return -1;
    }
    logo_spr = sfSprite_create();
    sfSprite_setTexture(logo_spr, logo_txt, sfFalse);

    font = sfFont_createFromFile("res/font.ttf");
    newgame_text = sfText_create();
    text_init(newgame_text, "New game", font, 16);
    text_center(newgame_text);
    sfText_setPosition(newgame_text, (sfVector2f) {mode.width / 2, 170});
    texts[0] = newgame_text;
    options_text = sfText_create();
    text_init(options_text, "Options", font, 16);
    text_center(options_text);
    sfText_setPosition(options_text, (sfVector2f) {mode.width / 2, 195});
    texts[1] = options_text;
    exit_text = sfText_create();
    text_init(exit_text, "Exit", font, 16);
    text_center(exit_text);
    sfText_setPosition(exit_text, (sfVector2f) {mode.width / 2, 220});
    texts[2] = exit_text;

    while (sfRenderWindow_isOpen(w)) {
        if (is_exit)
            break;
        while (sfRenderWindow_pollEvent(w, &evt))
            switch (evt.type) {
            case sfEvtClosed:
                sfRenderWindow_close(w);
                break;
            case sfEvtMouseMoved:
                mouse_pos_mapped = sfRenderWindow_mapPixelToCoords(w,
                    (sfVector2i) {evt.mouseMove.x, evt.mouseMove.y}, NULL);
                evt.mouseMove.x = mouse_pos_mapped.x;
                evt.mouseMove.y = mouse_pos_mapped.y;
                for (i = 0; i < 3; i++) {
                    if (!is_menu_item_selected) {
                        sfRenderWindow_setMouseCursor(w,
                            sfCursor_createFromSystem(sfCursorArrow));
                        sfText_setColor(texts[i], sfWhite);
                    }
                    bounds = sfText_getGlobalBounds(texts[i]);
                    mouse_rect = (sfFloatRect) {
                        evt.mouseMove.x, evt.mouseMove.y, 1.f, 1.f
                    };
                    if (sfFloatRect_intersects(&bounds, &mouse_rect, NULL)) {
                        sfRenderWindow_setMouseCursor(w,
                            sfCursor_createFromSystem(sfCursorHand));
                        sfText_setColor(texts[i], sfGreen);
                        is_menu_item_selected = sfTrue;
                        break;
                    }
                }
                if (i == 3)
                    is_menu_item_selected = sfFalse;
                break;
            case sfEvtMouseButtonPressed:
                for (i = 0; i < 3; i++) {
                    sfColor color = sfText_getColor(texts[i]);

                    if (color.r == sfGreen.r && color.g == sfGreen.g
                            && color.b == sfGreen.b && color.a == sfGreen.a) {
                        if (strcmp(sfText_getString(texts[i]), "New game") == 0) {
                            sfRenderWindow_setVisible(w, sfFalse);
                            evt_game(opts);
                            sfRenderWindow_setVisible(w, sfTrue);
                        }
                        else if (strcmp(sfText_getString(texts[i]), "Options") == 0) {
                            sfRenderWindow_setVisible(w, sfFalse);
                            evt_options(opts);
                            sfRenderWindow_setVisible(w, sfTrue);
                            mode = get_resolution(opts->res, sfFalse);
                            sfRenderWindow_setSize(w,
                                (sfVector2u) {mode.width, mode.height});
                            sfRenderWindow_setPosition(w,
                                (sfVector2i) {sfVideoMode_getDesktopMode().width
                                    / 2 - mode.width / 2,
                                    sfVideoMode_getDesktopMode().height
                                    / 2 - mode.height / 2});
                        }
                        else
                            is_exit = sfTrue;
                        sfRenderWindow_setMouseCursor(w,
                            sfCursor_createFromSystem(sfCursorArrow));
                        sfText_setColor(texts[i], sfWhite);
                        break;
                    }
                }
                break;
            default:
                break;
            }

        sfRenderWindow_clear(w, sfBlack);
        sfRenderWindow_drawSprite(w, logo_spr, NULL);
        sfRenderWindow_drawText(w, newgame_text, NULL);
        sfRenderWindow_drawText(w, options_text, NULL);
        sfRenderWindow_drawText(w, exit_text, NULL);
        sfRenderWindow_display(w);
    }

    sfText_destroy(newgame_text);
    sfText_destroy(options_text);
    sfText_destroy(exit_text);
    sfFont_destroy(font);
    sfSprite_destroy(logo_spr);
    sfTexture_destroy(logo_txt);
    sfRenderWindow_destroy(w);
    free(opts->blk_txt_path);
    free(opts->sound_path);

    return 0;
}

void
evt_game (struct options * opts)
{
    sfVideoMode mode;
    sfRenderWindow * w;
    sfEvent evt;
    sfTexture * blk_txt, * blk_ghost_txt;
    sfClock * clock = sfClock_create();
    float t = 0.f;
    struct ttr * ttr [(FSIZEX / 2) * (FSIZEY / 2)], * ttr_ghost,
        * ttr_hold = NULL;
    unsigned i, q = 0;
    sfBool shouldFixTtr = sfFalse, onHold = sfFalse, canHold = sfTrue,
        isTopOut = sfFalse;
    struct bag bag = {0};
    sfSound * sound;
    sfSoundBuffer * soundbuf;

    mode = get_resolution(RES_1x, sfTrue);
    w = sfRenderWindow_create(mode, "Tetris", sfClose, NULL);
    if (w == NULL) {
        puts("Failed to create window");
        return;
    }

    // Load block texture
    blk_txt = sfTexture_createFromFile(opts->blk_txt_path,
        &(sfIntRect) {0, 0, BLKSIZE, BLKSIZE});
    if (blk_txt == NULL) {
        puts("Failed to load block texture");
        return;
    }

    // Load ghost block texture
    blk_ghost_txt = sfTexture_createFromFile("res/block_ghost.png", NULL);
    if (blk_ghost_txt == NULL) {
        puts("Failed to load ghost block texture");
        return;
    }

    mode = get_resolution(opts->res, sfTrue);
    sfRenderWindow_setSize(w, (sfVector2u) {mode.width, mode.height});
    sfRenderWindow_setPosition(w,
        (sfVector2i) {sfVideoMode_getDesktopMode().width / 2 - mode.width / 2,
            sfVideoMode_getDesktopMode().height / 2 - mode.height / 2});

    srand(time(NULL));
    for (i = 0; i < ITEMQ; i++)
        bag_gen_item(&bag, i, blk_txt);
    ttr[q] = bag_next(&bag, blk_txt);

    // Load sound
    sound = sfSound_create();
    soundbuf = sfSoundBuffer_createFromFile(opts->sound_path);
    sfSound_setBuffer(sound, soundbuf);
    sfSound_setLoop(sound, sfTrue);
    sfSound_setVolume(sound, 10.f);
    sfSound_play(sound);

    ttr_ghost = ttr_alloc(ttr[q]->form, blk_ghost_txt);
    ttr_set_color(ttr_ghost, sfColor_fromRGB(0x88, 0x88, 0x88));
    ttr_copy_pos(ttr_ghost, ttr[q]);
    ttr_hard_drop(ttr_ghost, (const struct ttr **) ttr, q + 1);
    while (sfRenderWindow_isOpen(w)) {
        t += sfClock_getElapsedTime(clock).microseconds;
        sfClock_restart(clock);
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
                        ttr_hard_drop(ttr_ghost, (const struct ttr **) ttr,
                            q + 1);
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
                        ttr_hard_drop(ttr_ghost, (const struct ttr **) ttr,
                            q + 1);
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
                        ttr_hard_drop(ttr_ghost, (const struct ttr **) ttr,
                            q + 1);
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
                    ttr_hard_drop(ttr[q], (const struct ttr **) ttr, q + 1);
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
                        if (ttr_hold == NULL) {
                            ttr_hold = ttr[q];
                            ttr[q] = bag_next(&bag, blk_txt);
                            ttr_copy_pos(ttr_hold, ttr[q]);
                        }
                        else {
                            ttr_swap(&ttr[q], &ttr_hold);
                            ttr_copy_pos(ttr[q], ttr_hold);
                            ttr_test_kick(ttr, q + 1);
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

        if (t > 1000000.f / (FSIZEY * opts->bps)
                || (shouldFixTtr && t > 200000.f)) {
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
        grid_draw(w, sfColor_fromRGB(0x20, 0x20, 0x20), WOFFSET);
        ttr_draw(ttr_ghost, w);
        for (i = 0; i < q + 1; i++)
            ttr_draw(ttr[i], w);
        bag_draw_items(&bag, w);
        if (ttr_hold != NULL)
            hud_draw_hold(ttr_hold, w);
        sfRenderWindow_display(w);
    }

    sfSound_destroy(sound);
    sfSoundBuffer_destroy(soundbuf);
    sfTexture_destroy(blk_txt);
    sfTexture_destroy(blk_ghost_txt);
    for (i = 0; i < q + 1; i++)
        ttr_destroy(ttr[i]);
    ttr_destroy(ttr_ghost);
    sfRenderWindow_destroy(w);
}

void
evt_options (struct options * opts)
{
    sfVideoMode mode;
    sfRenderWindow * w;
    sfEvent evt;
    sfText * resolution_lbl_text, * resolution_text, * speed_lbl_text,
        * speed_text, * blk_txt_text, * sound_lbl_text, * sound_text,
        * save_and_exit_text;
    sfTexture * logo_txt, * bar_txt, * selector_txt, * blk_txt;
    sfSprite * logo_spr, * bar_spr, * selector_spr, * blk_spr;
    sfFont * font;
    char resolution_str [16] = {'\0'}, speed_str [16] = {'\0'};
    sfBool is_mouse_button_hold = sfFalse, is_selector_move = sfFalse,
        is_menu_item_selected = sfFalse, is_exit = sfFalse;
    sfFloatRect bounds, mouse_rect;
    float mp_prev_x;
    const float speeds [SPEED_RANGE] = {
        0.05f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.f
    };
    sfVector2f mouse_pos_mapped;
    int i;
    sfClock * clock = sfClock_create();
    struct text_slide sound_ts = {0};
    enum win_res res = opts->res;
    float bps = opts->bps;

    mode = get_resolution(RES_1x, sfFalse);
    w = sfRenderWindow_create(mode, "Tetris", sfClose, NULL);
    if (w == NULL) {
        puts("Failed to create window");
        return;
    }

    logo_txt = sfTexture_createFromFile("res/logo.png", NULL);
    if (logo_txt == NULL) {
        puts("Failed to load logo");
        return;
    }
    logo_spr = sfSprite_create();
    sfSprite_setTexture(logo_spr, logo_txt, sfFalse);

    font = sfFont_createFromFile("res/font.ttf");
    resolution_lbl_text = sfText_create();
    text_init(resolution_lbl_text, "Resolution", font, 12);
    text_center(resolution_lbl_text);
    sfText_setPosition(resolution_lbl_text, (sfVector2f) {mode.width / 2, 120});
    snprintf(resolution_str, sizeof(resolution_str), "%ux%u",
        get_resolution(opts->res, sfFalse).width,
        get_resolution(opts->res, sfFalse).height);
    resolution_text = sfText_create();
    text_init(resolution_text, resolution_str, font, 12);
    text_center(resolution_text);
    sfText_setPosition(resolution_text, (sfVector2f) {mode.width / 2, 140});

    // Speed changer section
    speed_lbl_text = sfText_create();
    text_init(speed_lbl_text, "Blocks per second", font, 12);
    text_center(speed_lbl_text);
    sfText_setPosition(speed_lbl_text, (sfVector2f) {mode.width / 2, 160});
    bar_txt = sfTexture_createFromFile("res/bar.png", NULL);
    if (bar_txt == NULL) {
        puts("Failed to load bar texture");
        return;
    }
    bar_spr = sfSprite_create();
    sfSprite_setTexture(bar_spr, bar_txt, sfFalse);
    sfSprite_setPosition(bar_spr, (sfVector2f) {20, 180});
    selector_txt = sfTexture_createFromFile("res/selector.png", NULL);
    if (selector_txt == NULL) {
        puts("Failed to load selector texture");
        return;
    }
    selector_spr = sfSprite_create();
    sfSprite_setTexture(selector_spr, selector_txt, sfFalse);
    for (i = 0; i < SPEED_RANGE; i++)
        if (opts->bps == speeds[i])
            break;
    sfSprite_setPosition(selector_spr,
        (sfVector2f) {ceil(20.f + sfSprite_getLocalBounds(bar_spr).width
        / SPEED_RANGE * i), 180 - 2});
    speed_text = sfText_create();
    snprintf(speed_str, sizeof(speed_str), "%g", opts->bps);
    text_init(speed_text, speed_str, font, 12);
    text_center(speed_text);
    sfText_setPosition(speed_text,
        (sfVector2f) {sfSprite_getPosition(selector_spr).x
        + sfSprite_getLocalBounds(selector_spr).width / 2, 200});

    // Block texture section
    blk_txt_text = sfText_create();
    text_init(blk_txt_text, "Block texture", font, 12);
    text_center(blk_txt_text);
    sfText_setPosition(blk_txt_text, (sfVector2f) {mode.width / 2, 220});
    blk_txt = sfTexture_createFromFile(opts->blk_txt_path,
        &(sfIntRect) {0, 0, BLKSIZE, BLKSIZE});
    if (blk_txt == NULL) {
        puts("Failed to load block texture");
        return;
    }
    blk_spr = sfSprite_create();
    sfSprite_setTexture(blk_spr, blk_txt, sfFalse);
    sfSprite_setPosition(blk_spr, (sfVector2f) {mode.width / 2
        - sfSprite_getLocalBounds(blk_spr).width / 2, 240 - 2});

    // Soundtrack section
    sound_lbl_text = sfText_create();
    text_init(sound_lbl_text, "Soundtrack", font, 12);
    text_center(sound_lbl_text);
    sfText_setPosition(sound_lbl_text, (sfVector2f) {mode.width / 2, 260});
    sound_text = sfText_create();
    text_init(sound_text, opts->sound_path, font, 12);
    text_center(sound_text);
    sfText_setPosition(sound_text, (sfVector2f) {mode.width / 2, 280});
    sound_ts.frame_pos = 20.f;
    sound_ts.frame_size = get_resolution(RES_1x, sfFalse).width - 40.f;
    sound_ts.text = sound_text;
    text_slide_init(&sound_ts, opts->sound_path);

    save_and_exit_text = sfText_create();
    text_init(save_and_exit_text, "Save and exit", font, 12);
    text_center(save_and_exit_text);
    sfText_setPosition(save_and_exit_text, (sfVector2f) {mode.width / 2, 300});

    mode = get_resolution(opts->res, sfFalse);
    sfRenderWindow_setSize(w, (sfVector2u) {mode.width, mode.height});
    sfRenderWindow_setPosition(w,
        (sfVector2i) {sfVideoMode_getDesktopMode().width / 2 - mode.width / 2,
        sfVideoMode_getDesktopMode().height / 2 - mode.height / 2});

    while (sfRenderWindow_isOpen(w)) {
        if (is_exit)
            break;
        while (sfRenderWindow_pollEvent(w, &evt))
            switch (evt.type) {
            case sfEvtClosed:
                sfRenderWindow_close(w);
                break;
            case sfEvtMouseMoved:
                mouse_pos_mapped = sfRenderWindow_mapPixelToCoords(w,
                    (sfVector2i) {evt.mouseMove.x, evt.mouseMove.y}, NULL);
                evt.mouseMove.x = mouse_pos_mapped.x;
                evt.mouseMove.y = mouse_pos_mapped.y;
                if (!is_menu_item_selected) {
                    sfRenderWindow_setMouseCursor(w,
                        sfCursor_createFromSystem(sfCursorArrow));
                    sfText_setColor(resolution_text, sfWhite);
                    sfSprite_setColor(blk_spr, sfColor_fromRGB(255, 255, 255));
                    sfText_setColor(sound_text, sfWhite);
                    sfText_setColor(save_and_exit_text, sfWhite);
                }
                if (is_mouse_button_hold) {
                    if (is_selector_move
                            && sfSprite_getPosition(selector_spr).x
                            + evt.mouseMove.x - mp_prev_x
                            >= sfSprite_getPosition(bar_spr).x
                            && sfSprite_getPosition(selector_spr).x
                            + evt.mouseMove.x - mp_prev_x
                            <= sfSprite_getLocalBounds(bar_spr).width
                            + sfSprite_getPosition(bar_spr).x
                            - sfSprite_getLocalBounds(selector_spr).width) {
                        sfSprite_move(selector_spr, (sfVector2f) {
                            evt.mouseMove.x - mp_prev_x, 0});
                        bps = speeds[(int) ((
                            sfSprite_getPosition(selector_spr).x
                            + sfSprite_getLocalBounds(selector_spr).width / 2
                            - sfSprite_getPosition(bar_spr).x)
                            / (sfSprite_getLocalBounds(bar_spr).width
                            / SPEED_RANGE))];
                        snprintf(speed_str, sizeof(speed_str), "%g", bps);
                        sfText_setString(speed_text, speed_str);
                        text_center(speed_text);
                        sfText_setPosition(speed_text, (sfVector2f) {
                            sfSprite_getPosition(selector_spr).x
                            + sfSprite_getLocalBounds(selector_spr).width
                            / 2, 200});
                    }
                    mp_prev_x = evt.mouseMove.x;
                }
                else {
                    mouse_rect = (sfFloatRect) {
                        evt.mouseMove.x, evt.mouseMove.y, 1.f, 1.f
                    };
                    bounds = sfText_getGlobalBounds(resolution_text);
                    if (sfFloatRect_intersects(&bounds, &mouse_rect, NULL)) {
                        sfRenderWindow_setMouseCursor(w,
                            sfCursor_createFromSystem(sfCursorHand));
                        sfText_setColor(resolution_text, sfGreen);
                        is_menu_item_selected = sfTrue;
                        break;
                    }
                    bounds = sfText_getGlobalBounds(save_and_exit_text);
                    if (sfFloatRect_intersects(&bounds, &mouse_rect, NULL)) {
                        sfRenderWindow_setMouseCursor(w,
                            sfCursor_createFromSystem(sfCursorHand));
                        sfText_setColor(save_and_exit_text, sfGreen);
                        is_menu_item_selected = sfTrue;
                        break;
                    }
                    is_menu_item_selected = sfFalse;
                }
                break;
            case sfEvtMouseButtonPressed:
                mouse_pos_mapped = sfRenderWindow_mapPixelToCoords(w,
                    (sfVector2i) {evt.mouseButton.x, evt.mouseButton.y}, NULL);
                evt.mouseButton.x = mouse_pos_mapped.x;
                evt.mouseButton.y = mouse_pos_mapped.y;
                is_mouse_button_hold = sfTrue;
                mouse_rect = (sfFloatRect) {
                    evt.mouseButton.x, evt.mouseButton.y, 1.f, 1.f
                };
                bounds = sfText_getGlobalBounds(resolution_text);
                if (sfFloatRect_intersects(&bounds, &mouse_rect, NULL)) {
                    res++;
                    if (res > RES_8x)
                        res = RES_1x;
                    mode = get_resolution(res, sfFalse);
                    snprintf(resolution_str, sizeof(resolution_str),
                        "%ux%u", mode.width, mode.height);
                    sfText_setString(resolution_text, resolution_str);
                    text_center(resolution_text);
                }
                bounds = sfSprite_getGlobalBounds(selector_spr);
                if (sfFloatRect_intersects(&bounds, &mouse_rect, NULL))
                    is_selector_move = sfTrue;
                bounds = sfText_getGlobalBounds(save_and_exit_text);
                if (sfFloatRect_intersects(&bounds, &mouse_rect, NULL)) {
                    opts->res = res;
                    opts->bps = bps;
                    is_exit = sfTrue;
                }
                mp_prev_x = evt.mouseButton.x;
                break;
            case sfEvtMouseButtonReleased:
                is_mouse_button_hold = sfFalse;
                is_selector_move = sfFalse;
                break;
            default:
                break;
            }

        text_slide_update(&sound_ts, clock);
        sfClock_restart(clock);

        sfRenderWindow_clear(w, sfBlack);
        sfRenderWindow_drawSprite(w, logo_spr, NULL);
        sfRenderWindow_drawText(w, resolution_lbl_text, NULL);
        sfRenderWindow_drawText(w, resolution_text, NULL);
        sfRenderWindow_drawText(w, speed_lbl_text, NULL);
        sfRenderWindow_drawSprite(w, bar_spr, NULL);
        sfRenderWindow_drawSprite(w, selector_spr, NULL);
        sfRenderWindow_drawText(w, speed_text, NULL);
        sfRenderWindow_drawText(w, blk_txt_text, NULL);
        sfRenderWindow_drawSprite(w, blk_spr, NULL);
        sfRenderWindow_drawText(w, sound_lbl_text, NULL);
        sfRenderWindow_drawText(w, sound_text, NULL);
        sfRenderWindow_drawText(w, save_and_exit_text, NULL);
        sfRenderWindow_display(w);
    }

    sfSprite_destroy(logo_spr);
    sfText_destroy(resolution_lbl_text);
    sfText_destroy(resolution_text);
    sfText_destroy(speed_lbl_text);
    sfSprite_destroy(bar_spr);
    sfSprite_destroy(selector_spr);
    sfText_destroy(speed_text);
    sfText_destroy(blk_txt_text);
    sfSprite_destroy(blk_spr);
    sfText_destroy(sound_lbl_text);
    sfText_destroy(sound_text);
    sfText_destroy(save_and_exit_text);
    sfRenderWindow_destroy(w);
    text_slide_free(&sound_ts);
}
