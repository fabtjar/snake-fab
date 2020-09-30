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
int positions_count;

void history_terminate(char *message);

void history_init(void)
{
    int snake_count = 0;
    for (int i = 0; i < PLAYER_COUNT; i++)
        for (Snake *snake = &players[i].head; snake; snake = snake->child)
            snake_count++;

    // X/Y for every snake block on the screen.
    positions_count = snake_count * 2;

    history_update();
}

void history_update(void)
{
    HistoryNode *history = malloc(sizeof(HistoryNode));
    if (!history)
        history_terminate("Error creating HistoryNode in history_update.");

    history->prev = NULL;

    history->positions = malloc(positions_count * sizeof(int));
    if (!history->positions)
    {
        free(history);
        history_terminate("Error creating HistoryNode positions in history_update.");
    }

    if (current_history)
        history->prev = current_history;
    current_history = history;

    int i = 0;
    for (int j = 0; j < PLAYER_COUNT; j++)
    {
        for (Snake *snake = &players[j].head; snake; snake = snake->child)
        {
            history->positions[i++] = snake->x;
            history->positions[i++] = snake->y;
        }
    }
}

void history_undo(Level *level)
{
    if (!current_history->prev)
        return;

    HistoryNode *old_history = current_history;

    current_history = current_history->prev;
    free(old_history->positions);
    free(old_history);

    for (int i = 0; i < PLAYER_COUNT; i++)
        player_set_level_tile(&players[i], level, true);

    int i = 0;
    for (int j = 0; j < PLAYER_COUNT; j++)
    {
        for (Snake *snake = &players[j].head; snake; snake = snake->child)
        {
            snake->x = current_history->positions[i++];
            snake->y = current_history->positions[i++];
        }
        player_set_level_tile(&players[j], level, false);
    }
}

void history_terminate(char *message)
{
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}