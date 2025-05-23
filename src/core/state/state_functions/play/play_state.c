#include "play_state.h"
#include "../../state_manager.h"
#include "../../../services/service_manager.h"
#include "../../../render/render_service.h"

static void play_state_background(SDL_Renderer *ren, void *ud)
{
    (void)ud;
    SDL_SetRenderDrawColor(ren, 0, 20, 40, 255);
    SDL_RenderClear(ren);
}

void play_state_enter(void *user_data) {
    StateManager *sm = (StateManager *)user_data;
    RenderService *R = svc_get(sm->services, RENDER_SERVICE);
    renderer_remove_all_layers(R);
    renderer_add_layer(R, play_state_background, NULL, "play_background");
}

void play_state_update(void *user_data) {
    return;
}

void play_state_exit(void *user_data) {
    return;
}