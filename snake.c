#include "snake.h"
#include "level.h"
#include <stdbool.h>

#define BODY_TILE_OFFSET -1
#define BODY_TILE_DARKER_OFFSET 2

// Check the level to see if a tile at the given position id matches body_tile_id.
// If it is add it to the offets array and increase offset_n by 1.
void add_find_body_offsets(int x, int y, int offsets_x[], int offsets_y[], int *offsets_n, Level *level, int body_tile_id);

bool snake_move(Snake *snake, int x, int y, Level *level)
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

// Recursively find an adjacent tile of the same snake body and set it as the snake's child.
void snake_find_bodies(Snake *snake, Snake snake_bodies[], Level *level)
{
    const int body_tile_id = snake->tile_id + BODY_TILE_OFFSET;
    int snakes_found = 0;

    Snake *parent = snake;
    while (parent)
    {
        int offsets_n = 0;
        int offsets_x[4];
        int offsets_y[4];

        add_find_body_offsets(parent->x, parent->y - 1, offsets_x, offsets_y, &offsets_n, level, body_tile_id);
        add_find_body_offsets(parent->x, parent->y + 1, offsets_x, offsets_y, &offsets_n, level, body_tile_id);
        add_find_body_offsets(parent->x - 1, parent->y, offsets_x, offsets_y, &offsets_n, level, body_tile_id);
        add_find_body_offsets(parent->x + 1, parent->y, offsets_x, offsets_y, &offsets_n, level, body_tile_id);

        int body_x, body_y;

        // Only set the child if it's not already in the snake's body.
        bool body_found = false;
        for (int i = 0; i < offsets_n; i++)
        {
            bool already_used = false;
            for (int j = 0; j < snakes_found; j++)
            {
                Snake *body = &snake_bodies[j];
                if (body->x == offsets_x[i] && body->y == offsets_y[i])
                {
                    already_used = true;
                    break;
                }
            }
            if (!already_used)
            {
                body_x = offsets_x[i];
                body_y = offsets_y[i];
                body_found = true;
                break;
            }
        }

        // Must be the tail.
        if (!body_found)
            return;

        Snake body = {
            .tile_id = body_tile_id,
            .x = body_x,
            .y = body_y,
        };

        // Every other body tile will be darker.
        if (snakes_found % 2 == 0)
        {
            body.tile_id += BODY_TILE_DARKER_OFFSET;
            level_set_tile(level, body.x, body.y, body.tile_id);
        }

        snake_bodies[snakes_found] = body;
        parent->child = &snake_bodies[snakes_found];
        snakes_found++;

        parent = parent->child;
    }
}

void add_find_body_offsets(int x, int y, int offsets_x[], int offsets_y[], int *offsets_n, Level *level, int body_tile_id)
{
    if (level_get_tile(level, x, y) == body_tile_id)
    {
        offsets_x[*offsets_n] = x;
        offsets_y[*offsets_n] = y;
        (*offsets_n)++;
    }
}