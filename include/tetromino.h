#ifndef TETROMINO_H

#include <stdio.h>
#include <stdlib.h>
#include <SFML/Graphics.h>
#include "consts.h"
#include <math.h>

enum dir {
    DIR_LEFT = 0,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN
};

enum rot_state {
    ROT_STATE_ZERO = 0,
    ROT_STATE_90,
    ROT_STATE_180,
    ROT_STATE_270
};

struct blk {
    sfSprite * s;
    sfVector2f dvn;
};

struct ttr {
    enum ttr_form form;
    struct blk * blk;
    unsigned q;
    enum dir dir;
    enum rot_state rot_state;
};

struct ttr * ttr_alloc (enum ttr_form form, const sfTexture * blk_txt);
void ttr_get_pos (struct ttr * t, sfVector2f pos [BLKQ]);
void ttr_set_pos (struct ttr * t, sfVector2f pos);
void ttr_set_scale (struct ttr * t, sfVector2f scale);
void ttr_set_color (struct ttr * t, sfColor c);
void ttr_copy_pos (struct ttr * dest, const struct ttr * src);
void ttr_swap (struct ttr ** src, struct ttr ** dest);
void ttr_move (struct ttr * t);
void ttr_move_pos (struct ttr * t, sfVector2f pos);
void ttr_rotate_90 (struct ttr * t);
void ttr_drop (struct ttr * t_base, const struct ttr ** t, unsigned q);
void ttr_hard_drop (struct ttr * t_base, const struct ttr ** t, unsigned q);
sfBool ttr_check_collide_all_static (struct ttr ** t, unsigned q);
sfBool ttr_test_kick (struct ttr ** t, unsigned q);
sfBool ttr_check_collide_ground (const struct ttr * t);
sfBool ttr_check_collide_walls (const struct ttr * t);
sfBool ttr_check_collide_another (const struct ttr * t_base, const struct ttr ** t, unsigned q);
sfBool ttr_check_inside_another (const struct ttr ** t, unsigned q);
void ttr_line_clear (struct ttr ** t, unsigned * q);
void ttr_draw (const struct ttr * t, sfRenderWindow * w);
void ttr_destroy (struct ttr * t);

#define TETROMINO_H
#endif
