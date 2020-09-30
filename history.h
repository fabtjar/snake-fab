#ifndef HISTORY_H
#define HISTORY_H

#include "level.h"

void history_init(void);
void history_update(void);
void history_undo(Level *level);

#endif