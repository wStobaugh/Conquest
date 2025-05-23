#include "quit_state.h"
#include "../../state_manager.h"
#include "../../../services/service_manager.h"
#include "../../../render/render_service.h"

void quit_state_enter(void *user_data) {
    StateManager *sm = (StateManager *)user_data;
    RenderService *R = svc_get(sm->services, RENDER_SERVICE);
    renderer_remove_all_layers(R);
}

// No need to update or exit a quit state