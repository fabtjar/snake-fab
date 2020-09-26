#include "player.h"
#include "level.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

void player_set_init_angle(Player *player);
void player_check_on_ground(Player *player, Level *level);
bool player_push(Player *player, int dir_x, int dir_y, Level *level, bool check_push, int player_pushing_id);

void player_create(Player *player, int id)
{
    player->id = id;
    player->head.tile_id = id * 4 + 5;
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

bool player_check_all_on_ground(Level *level)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
        players[i].on_ground = false;

    for (int i = 0; i < PLAYER_COUNT; i++)
        if (!players[i].on_ground)
            player_check_on_ground(&players[i], level);

    for (int i = 0; i < PLAYER_COUNT; i++)
        if (!players[i].on_ground)
            return false;

    return true;
}

void player_check_on_ground(Player *player, Level *level)
{
    for (Snake *snake = &player->head; snake; snake = snake->child)
    {
        if (level_get_tile(level, snake->x, snake->y + 1) == 1)
        {
            player->on_ground = true;
            break;
        }
    }

    if (!player->on_ground)
        return;

    for (Snake *snake = &player->head; snake; snake = snake->child)
    {
        int over_tile_id = level_get_tile(level, snake->x, snake->y - 1);
        if (over_tile_id > 1)
        {
            Player *other = player_find_from_tile(over_tile_id);
            if (other->id == player->id || other->on_ground)
                continue;

            other->on_ground = true;
            player_check_on_ground(other, level);
        }
    }
}

void player_update_falling(Level *level)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
        if (!players[i].on_ground)
            player_set_level_tile(&players[i], level, true);

    for (int i = 0; i < PLAYER_COUNT; i++)
        if (!players[i].on_ground)
            for (Snake *snake = &players[i].head; snake; snake = snake->child)
                snake->y++;

    for (int i = 0; i < PLAYER_COUNT; i++)
        if (!players[i].on_ground)
            player_set_level_tile(&players[i], level, false);
}

Player *player_find_from_tile(int tile_id)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
        if (player_is_own_tile(&players[i], tile_id))
            return &players[i];
    return NULL;
}

void player_set_level_tile(Player *player, Level *level, bool clear_tile)
{
    for (Snake *snake = &player->head; snake; snake = snake->child)
        level_set_tile(level, snake->x, snake->y, clear_tile ? 0 : snake->tile_id);
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
        player_set_level_tile(&players[i], level, false);

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

    player_set_level_tile(player, level, true);

    for (Snake *snake = &player->head; snake; snake = snake->child)
    {
        // Positions will be update on the level after all players are
        // cleared their posiition on the level to avoid overriding.
        snake->x += dir_x;
        snake->y += dir_y;
    }

    return true;
}
