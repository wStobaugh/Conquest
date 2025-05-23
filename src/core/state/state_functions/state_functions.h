#ifndef STATE_FUNCTIONS_H
#define STATE_FUNCTIONS_H

#include "menu/menu_state.h"
#include "play/play_state.h"
#include "pause/pause_state.h"
#include "quit/quit_state.h"

typedef struct StateVTable {
  void (*enter)(void *user_data);
  void (*update)(void *user_data);
  void (*exit)(void *user_data);
} StateVTable;

enum GameState { GS_MENU = 0, GS_PLAY = 1, GS_QUIT = 2, GS_PAUSE = 3 };

typedef struct GameStateObject {
  StateVTable *vtable;
  enum GameState type;
} GameStateObject;

// Object holds array of GameStateObjects
typedef struct GameStates{
    GameStateObject *states;
    int num_states;
} GameStates;

GameStates *init_game_states(void);
GameStateObject *get_state_object(GameStates *states, enum GameState state);
StateVTable *get_state_vtable(GameStates *states, enum GameState state);

#endif
