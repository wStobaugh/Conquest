#include "menu_state.h"
#include "../../state_manager.h"
#include "../../../services/service_manager.h"
#include "../../../render/render_service.h"

static void menu_render_layer(SDL_Renderer *ren, void *ud)
{
    menu_render((Menu *)ud, ren); 
}


void menu_state_enter(void *user_data) {
    StateManager *sm = (StateManager *)user_data;
    RenderService *R = svc_get(sm->services, RENDER_SERVICE);
    renderer_remove_all_layers(R);
    renderer_add_layer(R, menu_render_layer, sm->menu, "menu");
}

void menu_state_update(void *user_data) {
    return;
}

void menu_state_exit(void *user_data) {
    return;
}