#include "snake.h"
#include "level.h"
#include <stdbool.h>

bool snake_move(struct Snake *snake, int x, int y, struct Level *level)
{
    int last_x = snake->x;
    int last_y = snake->y;
    level_set_tile(level, last_x, last_y, 0);
    level_set_tile(level, x, y, snake->tile_id);
    snake->x = x;
    snake->y = y;

    if (snake->child)
        snake_move(snake->child, last_x, last_y, level);

    return true;
}

void snake_find_bodies(struct Snake *snake, struct Snake snake_bodies[], struct Level *level)
{
    const int body_tile_id = snake->tile_id - 1;
    int snakes_found = 0;

    struct Snake *parent = snake;
    while (parent)
    {
        int offset_x = 0;
        int offset_y = 0;

        if (level_get_tile(level, parent->x, parent->y - 1) == body_tile_id)
            offset_y = -1;
        else if (level_get_tile(level, parent->x, parent->y + 1) == body_tile_id)
            offset_y = 1;
        else if (level_get_tile(level, parent->x - 1, parent->y) == body_tile_id)
            offset_x = -1;
        else if (level_get_tile(level, parent->x + 1, parent->y) == body_tile_id)
            offset_x = 1;

        const int body_x = parent->x + offset_x;
        const int body_y = parent->y + offset_y;

        bool already_found_body = 0;
        for (int i = 0; i < snakes_found; i++)
        {
            struct Snake *body = &snake_bodies[i];
            if (body->x == body_x && body->y == body_y)
            {
                already_found_body = true;
                break;
            }
        }

        if ((offset_x != 0 || offset_y != 0) && !already_found_body)
        {
            struct Snake body = {};
            body.tile_id = body_tile_id;
            body.x = body_x;
            body.y = body_y;

            if (snakes_found % 2 == 0)
            {
                body.tile_id += 2;
                level_set_tile(level, body.x, body.y, body.tile_id);
            }

            snake_bodies[snakes_found] = body;
            parent->child = &snake_bodies[snakes_found];
            snakes_found++;
        }

        parent = parent->child;
    }
}
