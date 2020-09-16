#include "level.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#define GAME_WIDTH 200
#define GAME_HEIGHT 200
#define GAME_SCALE 3

#define JSON_BUFFER_SIZE 2048
#define TILE_SIZE 20

struct Snake
{
    int x, y, tile_id;
    struct Snake *child;
};

int snake_move(struct Snake *snake, int x, int y, struct Level *level)
{
    if (level_get_tile(level, x, y) != 0)
        return 0;

    int last_x = snake->x;
    int last_y = snake->y;
    level_set_tile(level, last_x, last_y, 0);
    level_set_tile(level, x, y, snake->tile_id);
    snake->x = x;
    snake->y = y;

    if (snake->child)
        snake_move(snake->child, last_x, last_y, level);

    return 1;
}

int main()
{
    struct Level level = level_create("assets/level.map");

    struct Snake player = {};
    player.tile_id = 4;
    int snake_body_count = 0;
    double player_angle = 90;

    for (int i = 0; i < level.length; i++)
    {
        int tile_id = level.tiles[i];

        if (tile_id == player.tile_id)
        {
            player.x = i % level.width;
            player.y = i / level.width;
        }
        else if (tile_id == 3)
        {
            snake_body_count++;
        }
    }

    struct Snake snake_bodies[snake_body_count];
    int snake_body_i = 0;
    for (int i = 0; i < level.length; i++)
    {
        if (level.tiles[i] == 3)
        {
            struct Snake body = {};
            body.tile_id = 3;
            body.x = i % level.width;
            body.y = i / level.width;
            snake_bodies[snake_body_i++] = body;
        }
    }

    struct Snake *snake = &player;
    while (snake)
    {
        for (int i = 0; i < snake_body_count; i++)
        {
            struct Snake *body = &snake_bodies[i];
            int x_diff = abs(snake->x - body->x);
            int y_diff = abs(snake->y - body->y);
            int is_adjacent = x_diff + y_diff == 1;
            if (is_adjacent && !body->child)
            {
                snake->child = body;
                if (i % 2 == 0)
                {
                    snake->child->tile_id += 2;
                    level_set_tile(&level, snake->child->x, snake->child->y, snake->child->tile_id);
                }
                break;
            }
        }
        snake = snake->child;
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    SDL_Window *window = SDL_CreateWindow("Snake Fab", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GAME_WIDTH * GAME_SCALE, GAME_HEIGHT * GAME_SCALE, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetScale(renderer, GAME_SCALE, GAME_SCALE);

    SDL_Surface *surface = IMG_Load("assets/tiles.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texture_tile_width;
    SDL_QueryTexture(texture, NULL, NULL, &texture_tile_width, NULL);
    texture_tile_width /= TILE_SIZE;
    SDL_FreeSurface(surface);

    SDL_Rect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
    SDL_Rect dest_rect = {0, 0, TILE_SIZE, TILE_SIZE};

    int input_x, input_y;

    int quit = 0;
    while (!quit)
    {
        input_x = input_y = 0;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    input_y--;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    input_y++;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    input_x--;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    input_x++;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        int is_on_ground = 0;
        struct Snake *snake = &player;
        while (snake && !is_on_ground)
        {
            if (level_get_tile(&level, snake->x, snake->y + 1) == 1)
                is_on_ground = 1;
            snake = snake->child;
        }

        if (!is_on_ground)
        {
            SDL_Delay(100);
            struct Snake *snake = &player;
            while (snake)
            {
                level_set_tile(&level, snake->x, snake->y, 0);
                snake = snake->child;
            }
            snake = &player;
            while (snake)
            {
                snake->y++;
                level_set_tile(&level, snake->x, snake->y, snake->tile_id);
                snake = snake->child;
            }
        }
        else if (input_x != 0 || input_y != 0)
        {
            if (input_x != 0)
                input_y = 0;
            int move_x = player.x + input_x;
            int move_y = player.y + input_y;

            int snake_moved = snake_move(&player, move_x, move_y, &level);
            if (snake_moved)
            {
                if (input_x > 0)
                    player_angle = 90;
                else if (input_x < 0)
                    player_angle = 270;
                else if (input_y > 0)
                    player_angle = 180;
                else if (input_y < 0)
                    player_angle = 0;
            }
        }

        for (int i = 0; i < level.length; i++)
        {
            dest_rect.x = i % level.width * TILE_SIZE;
            dest_rect.y = i / level.width * TILE_SIZE;

            int tile_id = level.tiles[i];

            if (tile_id == 0 && (i % 2 + (i / 10) % 2) % 2 == 0)
                tile_id += 2;

            src_rect.x = tile_id % texture_tile_width * TILE_SIZE;
            src_rect.y = tile_id / texture_tile_width * TILE_SIZE;

            if (tile_id == player.tile_id)
                SDL_RenderCopyEx(renderer, texture, &src_rect, &dest_rect, player_angle, NULL, SDL_FLIP_NONE);
            else
                SDL_RenderCopy(renderer, texture, &src_rect, &dest_rect);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();
}