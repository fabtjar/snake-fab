#ifndef PLAYER_H
#define PLAYER_H

#include "snake.h"

#define PLAYER_DIR_UP 0
#define PLAYER_DIR_DOWN 180
#define PLAYER_DIR_LEFT 270
#define PLAYER_DIR_RIGHT 90

#define PLAYER_SNAKE_BODY_MAX 10
#define PLAYER_COUNT 2

struct Player
{
    struct Snake head;
    int angle;
    bool on_ground;
};
struct Player players[PLAYER_COUNT];
void player_create(struct Player *player, int tile_id);
void player_load_from_level(struct Player *player, struct Snake snake_bodies[], struct Level *level);
bool player_is_own_tile(struct Player *player, int tile_id);
bool player_check_on_ground(struct Player *player, struct Level *level);
void player_fall(struct Player *player, struct Level *level);
void player_update(struct Player *player, int input_x, int input_y, struct Level *level);
bool player_move(struct Player *player, int x, int y, struct Level *level);

#endif
