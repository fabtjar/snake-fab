#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

typedef struct Level
{
    int tiles[100];
    int width;
    int length;
} Level;
Level level_create(char *level_file);
int level_get_tile(Level *level, int x, int y);
bool level_set_tile(Level *level, int x, int y, int tile_id);
int level_get_tile_index(Level *level, int tile_id);
int level_get_tile_count(Level *level, int tile_id);

#endif
