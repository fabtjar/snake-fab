struct Level
{
    int tiles[100];
    int width;
    int length;
};
struct Level level_create(char *level_file);
int level_get_tile(struct Level *level, int x, int y);
int level_set_tile(struct Level *level, int x, int y, int tile_id);
