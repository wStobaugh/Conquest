#include "computation_layers.h"
#include "computation_stack.h"
#include "../input/input_manager.h"
#include "../services/service_manager.h"
#include "../state/state_manager.h"
#include "../clock/clock_service.h"
#include "../render/render_service.h"
#include <SDL2/SDL.h>

void layer_state_input(GameHandle *gh) {
    InputManager *im = svc_get(gh->services, INPUT_SERVICE);
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);
    sm_handle_input(sm, im);
}
void layer_present(GameHandle *gh) {
    RenderService *renderer = svc_get(gh->services, RENDER_SERVICE);
    renderer_present(renderer);
    InputManager *im = svc_get(gh->services, INPUT_SERVICE);
    input_update(im);
}

void layer_clock_update(GameHandle *gh) {
    ClockService *clock = svc_get(gh->services, CLOCK_SERVICE);
    clock_service_update(clock);
}

void layer_state_render(GameHandle *gh)
{
    /* begin the frame before individual render layers draw */
    RenderService *R = svc_get(gh->services, RENDER_SERVICE);
    if (R) renderer_begin_frame(R);
}

void register_standard_layers(GameHandle *gh)
{
    /* highest priority first */
    push_layer(gh, "clock",   layer_clock_update,  LAYER_PRIORITY_CLOCK);
    push_layer(gh, "input",   layer_state_input,   LAYER_PRIORITY_INPUT);
    push_layer(gh, "render",  layer_state_render,  LAYER_PRIORITY_RENDER);
    push_layer(gh, "present", layer_present,       LAYER_PRIORITY_PRESENT);
}