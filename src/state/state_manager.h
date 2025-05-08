#ifndef CONQUEST_STATE_MANAGER_H
#define CONQUEST_STATE_MANAGER_H
#include "../gui/menu/menu.h"
#include <SDL2/SDL.h>

enum GameState { GS_MENU, GS_PLAY, GS_QUIT };

typedef struct StateManager {
  enum GameState state;
  Menu *menu;
} StateManager;

StateManager *sm_create(SDL_Renderer *ren, int w, int h, const char *font_path);
void sm_handle_event(StateManager *sm, const SDL_Event *e);
void sm_update(StateManager *sm);
void sm_render(StateManager *sm, SDL_Renderer *ren);
void sm_destroy(StateManager *sm);

#endif // CONQUEST_STATE_MANAGER_H