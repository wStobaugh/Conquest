#ifndef GAME_LOOP_INITIALIZATION_H
#define GAME_LOOP_INITIALIZATION_H
#include "../utils/game_structs.h"

GameHandle *game_init();
int initialize_event_bus(GameHandle *gh);
#endif // GAME_LOOP_INITIALIZATION_H