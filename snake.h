#ifndef SNAKE_H
#define SNAKE_H

#include "level.h"
#include <stdbool.h>

struct Snake
{
    int x, y, tile_id;
    struct Snake *child;
};
bool snake_move(struct Snake *snake, int x, int y, struct Level *level);
void snake_find_bodies(struct Snake *snake, struct Snake snake_bodies[], struct Level *level);

#endif
