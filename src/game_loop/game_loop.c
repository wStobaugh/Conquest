/*
This file is part of the game engine project.


*/
#include "game_loop.h"
#include "../core/compute/computation_stack.h"
#include "../core/input/input_manager.h"
#include "../core/services/service_manager.h"
#include "../core/state/state_manager.h"
#include "../core/state/state_functions/state_functions.h"
#include "../core/audio/audio_manager.h"
#include "../core/settings/settings_manager.h"
#include "../core/settings/default_settings.h"
#include "../core/resources/resource_paths.h"
#include "../utils/game_structs.h"
#include "../utils/log.h"
#include <SDL2/SDL.h>
#include <string.h>
#include "../core/resources/resource_manager.h"
#include "../core/event/event_bus.h"
#include "../core/event/event_signals.h"
#include "../core/cursor/cursor.h"
#include "../core/clock/clock_service.h"
#include "../core/render/render_service.h"

// Initialize core game services and register them with the service manager
int initialize_core_services(GameHandle *gh) {
    

    // 1) Create a borderless fullscreen-desktop window (size args are ignored)
    SDL_Window *win = SDL_CreateWindow(
        "Conquest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);

    // 2) Create the renderer
    SDL_Renderer *ren = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Get window dimensions
    int win_w, win_h;
    SDL_GetWindowSize(win, &win_w, &win_h);

    // Create core managers
    RenderService *renderer = renderer_init(ren, win);
    SettingsManager *settings = sm_settings_create();
    EventBus *bus = malloc(sizeof(EventBus));
    ResourceManager *resource_manager = resource_manager_create();
    StateManager *sm = sm_create(ren, win_w, win_h, resource_manager);
    InputManager *im = input_create();
    AudioManager *am = am_create(10); // 10 is the max number of audios
    ClockService *clock = clock_service_init();
    
    if (!sm || !im || !am || !settings || !bus || !renderer) {
        LOG_ERROR("Failed to create game subsystems\n");
        return 0;
    }

    // Initialize cursor module after core services are ready
    if (cursor_init(resource_manager) != 0) {
        LOG_INFO("cursor_init() failed – using default system cursor");
    }

    // Initialize services
    initialize_default_settings(settings);
    bus_init(bus);
    
    // Register services
    svc_register(gh->services, INPUT_SERVICE, im);
    svc_register(gh->services, STATE_MANAGER_SERVICE, sm);
    svc_register(gh->services, AUDIO_SERVICE, am);
    svc_register(gh->services, SETTINGS_MANAGER_SERVICE, settings);
    svc_register(gh->services, EVENT_BUS_SERVICE, bus);
    svc_register(gh->services, RESOURCE_MANAGER_SERVICE, resource_manager);
    svc_register(gh->services, CLOCK_SERVICE, clock);
    svc_register(gh->services, RENDER_SERVICE, renderer);
    
    // Set the services for the state manager
    sm_set_services(sm, gh->services);

    // Enter the menu state
    sm_enter(sm, GS_MENU);
    
    // Set the audio manager for the state manager's menu
    sm_set_audio_manager(sm, am);
    
    // Initialize menu background music
    sm_initialize_menu_music(sm, am, settings);

    return 1;
}

// Runs one iteration of the game loop
void game_loop(GameHandle *gh) {

    // TODO: Wonder if there is a way to have
    // Access to the services without requesting them every loop?
    InputManager *im = svc_get(gh->services, INPUT_SERVICE);
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);

    SDL_Event e;
    while (SDL_PollEvent(&e))
        // Replace im with getting gh service
        input_handle_event(im, &e);

    /* global hot-keys */
    if (input_pressed(im, ACTION_QUIT))
        sm->current_state = get_state_object(sm->states, GS_QUIT);

    // Iterate through the computation stack and execute each layer
    comp_stack_execute(gh->stack, gh);
}