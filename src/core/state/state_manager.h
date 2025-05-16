#ifndef CONQUEST_STATE_MANAGER_H
#define CONQUEST_STATE_MANAGER_H
#include "../../gui/menu/menu.h"
#include <SDL2/SDL.h>
#include "../services/service_manager.h"
#include "../resources/resource_manager.h"

enum GameState { GS_MENU, GS_PLAY, GS_QUIT, GS_PAUSE };

typedef struct StateManager {
  enum GameState state;
  Menu *menu;
  ServiceManager *services;
} StateManager;

StateManager *sm_create(SDL_Renderer *ren, int w, int h, const char *font_path, ResourceManager *resource_manager);
void sm_render(StateManager *sm, SDL_Renderer *ren);
void sm_destroy(StateManager *sm);

struct InputManager;                       /* forward-declare */
void sm_handle_input(StateManager *sm, const struct InputManager *im);

// Forward declarations for service managers
struct AudioManager;
struct SettingsManager;

// Initialize the menu background music
void sm_initialize_menu_music(StateManager *sm, struct AudioManager *am, struct SettingsManager *settings);

// Set the audio manager for the state manager's menu
void sm_set_audio_manager(StateManager *sm, struct AudioManager *am);

// Add this function declaration
void sm_set_services(StateManager *sm, ServiceManager *services);

#endif // CONQUEST_STATE_MANAGER_H