#ifndef SNAKE_H
#define SNAKE_H

#include "level.h"

struct Snake
{
    int x, y, tile_id;
    struct Snake *child;
};
int snake_move(struct Snake *snake, int x, int y, struct Level *level);

#endif
