#include "snake.h"
#include "level.h"

int snake_move(struct Snake *snake, int x, int y, struct Level *level)
{
    if (level_get_tile(level, x, y) != 0)
        return 0;

    int last_x = snake->x;
    int last_y = snake->y;
    level_set_tile(level, last_x, last_y, 0);
    level_set_tile(level, x, y, snake->tile_id);
    snake->x = x;
    snake->y = y;

    if (snake->child)
        snake_move(snake->child, last_x, last_y, level);

    return 1;
}
