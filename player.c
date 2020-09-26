#include "player.h"
#include "level.h"
#include <math.h>
#include <stdbool.h>

void player_set_init_angle(Player *player);
bool player_push(Player *player, int dir_x, int dir_y, Level *level, bool check_push, int player_pushing_id);

void player_create(Player *player, int id)
{
    player->id = id;
    player->head.tile_id = 4 + id * 3;
    player->angle = PLAYER_DIR_RIGHT;
    player->on_ground = false;
}

void player_load_from_level(Player *player, Snake snake_bodies[], Level *level)
{
    const int level_index = level_get_tile_index(level, player->head.tile_id);
    player->head.x = level_index % level->width;
    player->head.y = level_index / level->width;

    snake_find_bodies(&player->head, snake_bodies, level);
    player_set_init_angle(player);
}

void player_set_init_angle(Player *player)
{
    // Can't determine an angle with just a head.
    if (!player->head.child)
        return;

    double diff_x = player->head.x - player->head.child->x;
    double diff_y = player->head.y - player->head.child->y;
    player->angle = (int)(atan2(diff_y, diff_x) * 180 / M_PI) + 90; // +90 as sprite facing up
}

bool player_is_own_tile(Player *player, int tile_id)
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

bool player_check_on_ground(Player *player, Level *level)
{
    bool on_ground = false;
    Snake *snake = &player->head;
    while (snake && !on_ground)
    {
        int under_tile_id = level_get_tile(level, snake->x, snake->y + 1);
        if (under_tile_id != 0 && !player_is_own_tile(player, under_tile_id))
            on_ground = true;
        snake = snake->child;
    }
    player->on_ground = on_ground;
    return on_ground;
}

void player_fall(Player *player, Level *level)
{
    if (!player->on_ground)
    {
        Snake *snake = &player->head;
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

void player_update(Player *player, int input_x, int input_y, Level *level)
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

bool player_move(Player *player, int x, int y, Level *level)
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
        if (!player_push(&players[other_id], dir_x, dir_y, level, true, player->id))
            return false;
    }

    // Some players would have been pushed and
    // cleared their position from the level.
    // Update level to show moved positions.
    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        Snake *snake = &players[i].head;
        while (snake)
        {
            level_set_tile(level, snake->x, snake->y, snake->tile_id);
            snake = snake->child;
        }
    }

    snake_move(&player->head, x, y, level);

    return true;
}

bool player_push(Player *player, int dir_x, int dir_y, Level *level, bool check_push, int player_pushing_id)
{
    Snake *snake;

    // Can't be pushed into walls.
    snake = &player->head;
    while (snake)
    {
        if (level_get_tile(level, snake->x + dir_x, snake->y + dir_y) == 1)
            return false;
        snake = snake->child;
    }

    // Check being pushed into other players.
    snake = &player->head;
    while (snake)
    {
        int tile_id = level_get_tile(level, snake->x + dir_x, snake->y + dir_y);

        // Another player.
        if (check_push && tile_id > 1 && !player_is_own_tile(player, tile_id))
        {
            int other_id = player_get_from_tile(tile_id);

            // The original pusher can't be pushed.
            if (other_id == player_pushing_id)
                return false;

            // If the other player can't be pushed then nor can this one.
            if (!player_push(&players[other_id], dir_x, dir_y, level, false, player_pushing_id))
                return false;
        }

        snake = snake->child;
    }

    snake = &player->head;
    while (snake)
    {
        // Clear position off the level.
        level_set_tile(level, snake->x, snake->y, 0);

        // Positions will be update on the level after all players are
        // cleared their posiition on the level to avoid overriding.
        snake->x += dir_x;
        snake->y += dir_y;

        snake = snake->child;
    }

    return true;
}
