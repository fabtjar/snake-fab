#include "history.h"
#include "level.h"
#include "player.h"
#include "snake.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct history_node
{
    int *positions;
    struct history_node *prev;
} HistoryNode;

HistoryNode *current_history;
int data_count;

void history_terminate(char *message);

void history_init(int active_player)
{
    int snake_count = 0;
    for (int i = 0; i < PLAYER_COUNT; i++)
        for (Snake *snake = &players[i].head; snake; snake = snake->child)
            snake_count++;

    // X/Y for every snake tile and
    // +1 for the active player.
    data_count = snake_count * 2 + 1;

    history_update(active_player);
}

void history_update(int active_player)
{
    HistoryNode *history = malloc(sizeof(HistoryNode));
    if (!history)
        history_terminate("Error creating HistoryNode in history_update.");

    history->prev = NULL;

    history->positions = malloc(data_count * sizeof(int));
    if (!history->positions)
    {
        free(history);
        history_terminate("Error creating HistoryNode positions in history_update.");
    }

    if (current_history)
        history->prev = current_history;
    current_history = history;

    history->positions[0] = active_player;

    int i = 1;
    for (int j = 0; j < PLAYER_COUNT; j++)
    {
        for (Snake *snake = &players[j].head; snake; snake = snake->child)
        {
            history->positions[i++] = snake->x;
            history->positions[i++] = snake->y;
        }
    }
}

bool history_goto_prev(void)
{
    if (!current_history->prev)
        return false;

    HistoryNode *old_history = current_history;

    current_history = current_history->prev;
    free(old_history->positions);
    free(old_history);

    return true;
}

void history_set_players(Level *level, int *active_player)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
        player_set_level_tile(&players[i], level, true);

    *active_player = current_history->positions[0];

    int i = 1;
    for (int j = 0; j < PLAYER_COUNT; j++)
    {
        Player *player = &players[j];
        for (Snake *snake = &player->head; snake; snake = snake->child)
        {
            snake->x = current_history->positions[i++];
            snake->y = current_history->positions[i++];
        }
        player_set_level_tile(player, level, false);
        player_set_head_angle(player);
    }
}

void history_undo(Level *level, int *active_player)
{
    history_goto_prev();
    history_set_players(level, active_player);
}

void history_restart(Level *level, int *active_player)
{
    while (history_goto_prev())
        ;
    history_set_players(level, active_player);
}

void history_terminate(char *message)
{
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}