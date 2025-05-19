#include "computation_layers.h"
#include "computation_stack.h"
#include "../input/input_manager.h"
#include "../services/service_manager.h"
#include "../state/state_manager.h"
#include "../clock/clock_service.h"
#include <SDL2/SDL.h>

void layer_state_input(GameHandle *gh) {
    InputManager *im = svc_get(gh->services, INPUT_SERVICE);
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);
    sm_handle_input(sm, im);
}

void layer_state_render(GameHandle *gh) {
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);
    sm_render(sm, gh->ren);
}

void layer_present(GameHandle *gh) {
    SDL_RenderPresent(gh->ren);
    InputManager *im = svc_get(gh->services, INPUT_SERVICE);
    input_update(im);
}

void layer_clock_update(GameHandle *gh) {
    ClockService *clock = svc_get(gh->services, CLOCK_SERVICE);
    clock_service_update(clock);
}

void register_standard_layers(GameHandle *gh) {
    push_layer(gh, "clock_update", layer_clock_update, LAYER_PRIORITY_CLOCK);
    push_layer(gh, "sm_input", layer_state_input, LAYER_PRIORITY_INPUT);
    push_layer(gh, "sm_render", layer_state_render, LAYER_PRIORITY_RENDER);
    push_layer(gh, "present", layer_present, LAYER_PRIORITY_PRESENT);
} 