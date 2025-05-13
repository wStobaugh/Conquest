/*
This file is part of the game engine project.


*/
#include "game_loop.h"
#include "../core/compute/computation_stack.h"
#include "../core/input/input_manager.h"
#include "../core/services/service_manager.h"
#include "../core/state/state_manager.h"
#include "../core/audio/audio_manager.h"
#include "../core/settings/settings_manager.h"
#include "../core/settings/default_settings.h"
#include "../core/resources/resource_paths.h"
#include "../utils/game_structs.h"
#include "../utils/log.h"
#include <SDL2/SDL.h>
#include <string.h>

// Initialize core game services and register them with the service manager
int initialize_core_services(GameHandle *gh) {
    // Get window dimensions
    int win_w, win_h;
    SDL_GetWindowSize(gh->win, &win_w, &win_h);

    // Create settings manager first (before other systems)
    SettingsManager *settings = sm_settings_create();
    if (!settings) {
        LOG_ERROR("Failed to create settings manager\n");
        return 0;
    }
    initialize_default_settings(settings);

    // Create core managers
    StateManager *sm = sm_create(gh->ren, win_w, win_h, get_font_path());
    InputManager *im = input_create();
    AudioManager *am = am_create(10); // 10 is the max number of audios

    if (!sm || !im || !am) {
        LOG_ERROR("Failed to create game subsystems\n");
        return 0;
    }

    // Register services
    svc_register(gh->services, INPUT_SERVICE, im);
    svc_register(gh->services, STATE_MANAGER_SERVICE, sm);
    svc_register(gh->services, AUDIO_SERVICE, am);
    svc_register(gh->services, SETTINGS_MANAGER_SERVICE, settings);

    // Set the audio manager for the state manager's menu
    sm_set_audio_manager(sm, am);
    
    // Initialize menu background music
    sm_initialize_menu_music(sm, am, settings);

    return 1;
}

// Runs one iteration of the game loop
void game_loop(GameHandle *gh) {

    InputManager *im = svc_get(gh->services, INPUT_SERVICE);
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);

    SDL_Event e;
    while (SDL_PollEvent(&e))
        // Replace im with getting gh service
        input_handle_event(im, &e);

    /* global hot-keys */
    if (input_pressed(im, ACTION_QUIT))
        gh->running = 0;

    // Iterate through the computation stack and execute each layer
    comp_stack_execute(gh->stack, gh);
}