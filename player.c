#include "player.h"
#include "level.h"
#include <stdbool.h>

#define PLAYER_DIR_UP 0
#define PLAYER_DIR_DOWN 180
#define PLAYER_DIR_LEFT 270
#define PLAYER_DIR_RIGHT 90

bool player_push(struct Player *player, int dir_x, int dir_y, struct Level *level);

void player_create(struct Player *player, int tile_id)
{
    player->head.tile_id = tile_id;
    player->angle = PLAYER_DIR_RIGHT;
    player->on_ground = false;
}

void player_load_from_level(struct Player *player, struct Snake snake_bodies[], struct Level *level)
{
    const int level_index = level_get_tile_index(level, player->head.tile_id);
    player->head.x = level_index % level->width;
    player->head.y = level_index / level->width;

    snake_find_bodies(&player->head, snake_bodies, level);
}

bool player_is_own_tile(struct Player *player, int tile_id)
{
    int player_tile_id = player->head.tile_id;
    return tile_id >= player_tile_id - 1 && tile_id <= player_tile_id + 1;
}

int player_get_from_tile(int tile_id)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
        if (player_is_own_tile(&players[i], tile_id))
            return i;
    return -1;
}

bool player_check_on_ground(struct Player *player, struct Level *level)
{
    bool on_ground = false;
    struct Snake *snake = &player->head;
    while (snake && !on_ground)
    {
        int under_tile_id = level_get_tile(level, snake->x, snake->y + 1);
        if (under_tile_id != 0 && !player_is_own_tile(player, under_tile_id))
        {
            player->on_ground = true;
            return true;
        }
        snake = snake->child;
    }
    player->on_ground = false;
    return false;
}

void player_fall(struct Player *player, struct Level *level)
{
    if (!player->on_ground)
    {
        struct Snake *snake = &player->head;
        while (snake)
        {
            level_set_tile(level, snake->x, snake->y, 0);
            snake = snake->child;
        }
        snake = &player->head;
        while (snake)
        {
            snake->y++;
            level_set_tile(level, snake->x, snake->y, snake->tile_id);
            snake = snake->child;
        }
    }
}

void player_update(struct Player *player, int input_x, int input_y, struct Level *level)
{
    if (input_x != 0)
        input_y = 0;
    int move_x = player->head.x + input_x;
    int move_y = player->head.y + input_y;

    bool moved = player_move(player, move_x, move_y, level);
    if (moved)
    {
        if (input_x > 0)
            player->angle = PLAYER_DIR_RIGHT;
        else if (input_x < 0)
            player->angle = PLAYER_DIR_LEFT;
        else if (input_y > 0)
            player->angle = PLAYER_DIR_DOWN;
        else if (input_y < 0)
            player->angle = PLAYER_DIR_UP;
    }
}

bool player_move(struct Player *player, int x, int y, struct Level *level)
{
    int tile_id = level_get_tile(level, x, y);

    // Can't move into walls.
    if (tile_id == 1)
        return false;

    // Check moving into other players.
    if (tile_id > 1)
    {
        // Can't move into self.
        if (player_is_own_tile(player, tile_id))
            return false;

        // Move if the other player can move there too.
        int other_id = player_get_from_tile(tile_id);
        int dir_x = x - player->head.x;
        int dir_y = y - player->head.y;
        if (!player_push(&players[other_id], dir_x, dir_y, level))
            return false;
    }

    snake_move(&player->head, x, y, level);

    return true;
}

bool player_push(struct Player *player, int dir_x, int dir_y, struct Level *level)
{
    int tile_id;

    struct Snake *snake = &player->head;
    while (snake)
    {
        tile_id = level_get_tile(level, snake->x + dir_x, snake->y + dir_y);
        if (tile_id == 0 || player_is_own_tile(player, tile_id))
            snake = snake->child;
        else
            return false;
    }

    snake = &player->head;
    while (snake)
    {
        level_set_tile(level, snake->x, snake->y, 0);
        snake->x += dir_x;
        snake->y += dir_y;
        level_set_tile(level, snake->x, snake->y, snake->tile_id);
        snake = snake->child;
    }

    return true;
}