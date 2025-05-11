/*
This file is part of the game engine project.


*/
#include "game_loop.h"
#include "../core/compute/computation_stack.h"
#include "../core/input/input_manager.h"
#include "../core/services/service_manager.h"
#include "../core/state/state_manager.h"
#include "../utils/game_structs.h"
#include <SDL2/SDL.h>
#include <string.h>

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