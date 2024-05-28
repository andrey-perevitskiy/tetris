#ifndef CONSTS_H

#define BLKSIZE 16
#define BLKQ 4
#define FSIZEX 10
#define FSIZEY 20
#define WOFFSET (BLKSIZE / 2 * 6)
#define WSIZEX (WOFFSET + BLKSIZE * FSIZEX + WOFFSET)
#define WSIZEY (BLKSIZE * FSIZEY)
#define SPEED_RANGE 11

enum ttr_form {
    TTR_FORM_I = 0,
    TTR_FORM_O,
    TTR_FORM_T,
    TTR_FORM_S,
    TTR_FORM_Z,
    TTR_FORM_J,
    TTR_FORM_L
};

enum win_res {
    RES_1x = 0,
    RES_2x,
    RES_4x,
    RES_8x
};

#define CONSTS_H
#endif
