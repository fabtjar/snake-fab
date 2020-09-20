#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

struct Level
{
    int tiles[100];
    int width;
    int length;
};
struct Level level_create(char *level_file);
int level_get_tile(struct Level *level, int x, int y);
bool level_set_tile(struct Level *level, int x, int y, int tile_id);
int level_get_tile_index(struct Level *level, int tile_id);
int level_get_tile_count(struct Level *level, int tile_id);

#endif
