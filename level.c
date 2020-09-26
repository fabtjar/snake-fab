#include "level.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

Level level_create(char *level_file)
{
    FILE *file = fopen(level_file, "r");
    char buffer[255];
    char level_tiles[255];
    int level_i = 0;
    while (!feof(file))
    {
        fgets(buffer, 255, file);
        int buffer_length = strlen(buffer);
        for (int i = 0; i < buffer_length; i++)
        {
            char c = buffer[i];
            if (c == ' ' || c == '\n')
                continue;

            switch (c)
            {
            case 'A':
                c = '5';
                break;
            case 'a':
                c = '4';
                break;
            case 'B':
                c = '9';
                break;
            case 'b':
                c = '8';
                break;
            case 'C':
                c = '=';
                break;
            case 'c':
                c = '<';
                break;
            }

            level_tiles[level_i++] = c;
        }
    }
    fclose(file);
    level_tiles[level_i] = '\0';
    Level level;
    level.length = strlen(level_tiles);
    level.width = 10;

    for (int i = 0; i < level.length; i++)
        level.tiles[i] = level_tiles[i] - '0';

    return level;
}

int level_get_tile(Level *level, int x, int y)
{
    if (x < 0 || x > level->width - 1 || y < 0 || y > level->length / level->width - 1)
        return -1;

    return level->tiles[x % level->width + y * level->width];
}

bool level_set_tile(Level *level, int x, int y, int tile_id)
{
    if (x < 0 || x > level->width - 1 || y < 0 || y > level->length / level->width - 1)
        return false;

    level->tiles[x % level->width + y * level->width] = tile_id;

    return true;
}

int level_get_tile_index(Level *level, int tile_id)
{
    for (int i = 0; i < level->length; i++)
    {
        if (level->tiles[i] == tile_id)
            return i;
    }
    return -1;
}

int level_get_tile_count(Level *level, int tile_id)
{
    int tile_count = 0;
    for (int i = 0; i < level->length; i++)
    {
        if (level->tiles[i] == tile_id)
            tile_count++;
    }
    return tile_count;
}
