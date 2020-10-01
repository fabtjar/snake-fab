#ifndef HISTORY_H
#define HISTORY_H

#include "level.h"

void history_init(int active_player);
void history_update(int active_player);
void history_undo(Level *level, int *active_player);
void history_restart(Level *level, int *active_player);

#endif
