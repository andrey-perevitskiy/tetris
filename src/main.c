#include "event.h"
#include "consts.h"
#include <string.h>

int
main (void)
{
    struct options opts = {
        RES_1x, 0.1f, strdup("res/block.png"), strdup("res/sound.ogg")
    };

    return evt_menu(&opts);
}
