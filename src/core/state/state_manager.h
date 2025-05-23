#ifndef CONQUEST_STATE_MANAGER_H
#define CONQUEST_STATE_MANAGER_H
#include "../../gui/menu/menu.h"
#include <SDL2/SDL.h>
#include "../services/service_manager.h"
#include "../resources/resource_manager.h"
#include "../render/render_service.h"
#include "state_functions/state_functions.h"
// forward declarations
struct RenderService;
struct StateVTable;
enum GameState;
struct GameStates;
struct GameStateObject;

typedef struct StateManager {
  Menu *menu;
  ServiceManager *services;
  GameStates *states;
  GameStateObject *current_state;
} StateManager;

StateManager *sm_create(SDL_Renderer *ren, int w, int h, ResourceManager *resource_manager);
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

// State transition function
void sm_enter(StateManager *sm, enum GameState new_state);

#endif // CONQUEST_STATE_MANAGER_H