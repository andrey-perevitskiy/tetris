#ifndef EVENT_H

#include <SFML/Window/VideoMode.h>
#include "consts.h"

struct options {
    enum win_res res;
    float bps;
    char * blk_txt_path;
    char * sound_path;
};

int evt_menu (struct options * opts);
void evt_game (struct options * opts);
void evt_options (struct options * opts);

#define EVENT_H
#endif