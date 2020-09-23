#ifndef SNAKE_H
#define SNAKE_H

#include "level.h"
#include <stdbool.h>

typedef struct Snake
{
    int x, y, tile_id;
    struct Snake *child;
} Snake;
bool snake_move(Snake *snake, int x, int y, Level *level);
void snake_find_bodies(Snake *snake, Snake snake_bodies[], Level *level);

#endif
