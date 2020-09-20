#include "level.h"
#include "player.h"
#include "snake.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

#define GAME_WIDTH 200
#define GAME_HEIGHT 200
#define GAME_SCALE 3

#define JSON_BUFFER_SIZE 2048
#define TILE_SIZE 20

#define PLAYER_DIR_UP 0
#define PLAYER_DIR_DOWN 180
#define PLAYER_DIR_LEFT 270
#define PLAYER_DIR_RIGHT 90

#define PLAYER_SNAKE_BODY_MAX 10

int main()
{
    struct Level level = level_create("assets/level.map");

    struct Player player;
    player_create(&player, 4);

    struct Snake snake_bodies[PLAYER_SNAKE_BODY_MAX];
    player_load_from_level(&player, snake_bodies, &level);

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

    bool quit = false;
    while (!quit)
    {
        input_x = input_y = 0;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
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

        bool is_on_ground = player_check_on_ground(&player, &level);

        if (!is_on_ground)
        {
            SDL_Delay(100);
            player_fall(&player, &level);
        }
        else if (input_x != 0 || input_y != 0)
        {
            player_move(&player, input_x, input_y, &level);
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

            if (tile_id == player.head.tile_id)
                SDL_RenderCopyEx(renderer, texture, &src_rect, &dest_rect, player.angle, NULL, SDL_FLIP_NONE);
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
