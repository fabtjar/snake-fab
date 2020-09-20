#ifndef PLAYER_H
#define PLAYER_H

#include "snake.h"

struct Player
{
    struct Snake head;
    int angle;
    bool on_ground;
};
void player_create(struct Player *player, int tile_id);
void player_load_from_level(struct Player *player, struct Snake snake_bodies[], struct Level *level);
bool player_check_on_ground(struct Player *player, struct Level *level);
void player_fall(struct Player *player, struct Level *level);
void player_move(struct Player *player, int input_x, int input_y, struct Level *level);

#endif
