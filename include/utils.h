#ifndef UTILS_H

#include <stdlib.h>
#include <SFML/Graphics.h>
#include "consts.h"

#define RAND(min, max) ((min) + rand() % ((max) - (min) + 1))

void grid_draw (sfRenderWindow * w, sfColor c);

#define UTILS_H
#endif
