#include "initialization.h"
#include "../core/compute/computation_stack.h"
#include "../core/cursor/cursor.h"
#include "../core/services/service_manager.h"
#include "../core/event/event_bus.h"
#include "../utils/log.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <unistd.h>

static void SDL_CheckErrors() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        SDL_Quit();
        return;
    }
    
    if (TTF_Init() != 0) {
        SDL_Log("TTF: %s", TTF_GetError());
        SDL_Quit();
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("IMG_Init failed: %s", IMG_GetError());
        SDL_Quit();
        return;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                TTF_GetError());
        SDL_Quit();
        return;
    }
}

// entry function
GameHandle *game_init(void) {
    // Initialize SDL & check for errors
    SDL_CheckErrors();

    // Set the working directory to the base path of game
    char *base = SDL_GetBasePath();
    chdir(base);
    SDL_free(base);

    // 1) Create a borderless fullscreen-desktop window (size args are ignored)
    SDL_Window *win = SDL_CreateWindow(
        "Conquest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);

    // 2) Create the renderer
    SDL_Renderer *ren = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* allocate and populate the handle */
    GameHandle *gh = malloc(sizeof *gh);
    if (!gh) {
        SDL_Log("Out-of-memory in game_init");
        return NULL;
    }

    gh->win = win;
    gh->ren = ren;
    gh->services = svc_create();
    gh->stack = malloc(sizeof(ComputationStack));
    comp_stack_init(gh->stack);
    initialize_event_bus(gh);

    return gh;
}

// Initialize the EventBus and register it as a service
int initialize_event_bus(GameHandle *gh) {
    if (!gh || !gh->services) 
        return 0;
        
    // Create and initialize the event bus
    EventBus *bus = malloc(sizeof(EventBus));
    if (!bus) {
        LOG_ERROR("Failed to allocate memory for EventBus\n");
        return 0;
    }
    
    bus_init(bus);
    
    // Register the event bus as a service
    if (svc_register(gh->services, EVENT_BUS_SERVICE, bus) != 0) {
        LOG_ERROR("Failed to register EventBus service\n");
        bus_destroy(bus);
        free(bus);
        return 0;
    }
    
    LOG_INFO("EventBus initialized and registered as a service\n");
    return 1;
}
