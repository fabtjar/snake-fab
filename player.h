#ifndef PLAYER_H
#define PLAYER_H

#include "snake.h"

#define PLAYER_DIR_UP 0
#define PLAYER_DIR_DOWN 180
#define PLAYER_DIR_LEFT 270
#define PLAYER_DIR_RIGHT 90

#define PLAYER_SNAKE_BODY_MAX 10
#define PLAYER_COUNT 3

typedef struct Player
{
    Snake head;
    int angle;
    bool on_ground;
} Player;
Player players[PLAYER_COUNT];
void player_create(Player *player, int tile_id);
void player_load_from_level(Player *player, Snake snake_bodies[], Level *level);
bool player_is_own_tile(Player *player, int tile_id);
bool player_check_on_ground(Player *player, Level *level);
void player_fall(Player *player, Level *level);
void player_update(Player *player, int input_x, int input_y, Level *level);
bool player_move(Player *player, int x, int y, Level *level);

#endif
