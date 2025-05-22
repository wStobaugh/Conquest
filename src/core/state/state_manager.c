/* -------------------------------------------------------------------------
 *  Conquest – State Manager
 * ---------------------------------------------------------------------- */
#include "state_manager.h"
#include "../input/input_manager.h"
#include "../audio/audio_manager.h"
#include "../settings/settings_manager.h"
#include "../resources/resource_paths.h"
#include "../resources/resource_manager.h"
#include "../../utils/log.h"
#include "../event/event_bus.h"
#include "../event/event_signals.h"
#include "../services/service_manager.h"
#include "../render/render_service.h"
#include <string.h>

/* ---------------------------------------------------------------------- */
/*  Global pointer so async listeners can reach the current state manager */
/* ---------------------------------------------------------------------- */
static StateManager *g_sm_instance = NULL;
StateManager *sm_get_instance(void) { return g_sm_instance; }

/* ---------------------------------------------------------------------- */
/*  Menu background music helper (unchanged)                              */
/* ---------------------------------------------------------------------- */
void sm_initialize_menu_music(StateManager *sm,
                              AudioManager *am,
                              SettingsManager *settings)
{
    if (!am || !settings) {
        LOG_ERROR("Cannot initialize menu music – missing managers\n");
        return;
    }

    float music_v  = sm_get_float(settings, "music_volume");
    float master_v = sm_get_float(settings, "master_volume");
    int   vol      = (int)(music_v * master_v * 128.0f);

    Audio *bg = audio_create(get_music_path("Music_1.mp3"), MUSIC, vol, 1);
    if (!bg || am_register(am, bg) == -1) {
        LOG_ERROR("Failed to create/register menu BGM\n");
        return;
    }
    am_play(am, bg);
}

/* ---------------------------------------------------------------------- */
/*  Event-bus listener (unchanged)                                        */
/* ---------------------------------------------------------------------- */
static void sm_handle_menu_signals(const Event *e)
{
    if (e->type != EVENT_TYPE_SIGNAL || !e->data) return;

    MenuSignal sig = *(MenuSignal *)e->data;
    free(e->data);

    StateManager *sm = g_sm_instance;
    if (!sm) return;

    switch (sig) {
        case MENU_SIGNAL_GOTO_CONTINUE:
        case MENU_SIGNAL_GOTO_NEW_GAME:  sm_enter(sm, GS_PLAY); break;
        case MENU_SIGNAL_GOTO_OPTIONS:   /* handled inside menu */       break;
        case MENU_SIGNAL_GOTO_QUIT:      sm_enter(sm, GS_QUIT); break;
        case MENU_SIGNAL_GOTO_MAIN:      sm_enter(sm, GS_MENU); break;
        default: break;
    }
}

/* ---------------------------------------------------------------------- */
/*  Construction / destruction (unchanged)                                */
/* ---------------------------------------------------------------------- */
StateManager *sm_create(SDL_Renderer *ren, int w, int h,
                        ResourceManager *resources)
{
    StateManager *sm = calloc(1, sizeof *sm);
    // TODO: GS_PLAY is temp to make the game run
    sm->state  = GS_PLAY;
    sm->menu   = menu_create(ren, w, h, NULL, resources);
    g_sm_instance = sm;
    return sm;
}

void sm_set_audio_manager(StateManager *sm, AudioManager *am)
{ if (sm && sm->menu) sm->menu->audio_manager = am; }

/* ---------------------------------------------------------------------- */
/*  Render helpers                                                        */
/* ---------------------------------------------------------------------- */
static void play_state_background(SDL_Renderer *ren, void *ud)
{
    (void)ud;
    SDL_SetRenderDrawColor(ren, 0, 20, 40, 255);
    SDL_RenderClear(ren);
}

/* flips (Menu*, SDL_Renderer*) → (SDL_Renderer*, void*) */
static void menu_render_layer(SDL_Renderer *ren, void *ud)
{
    menu_render((Menu *)ud, ren); 
}

/* called once when we *enter* the menu state */
void enter_menu(Menu *menu, RenderService *R)
{
    renderer_remove_all_layers(R);
    renderer_add_layer(R, menu_render_layer, menu, "menu");
}

/* called once when we *enter* the play state */
void enter_play(Menu *menu, RenderService *R)
{
    renderer_remove_all_layers(R);
    renderer_add_layer(R, play_state_background, NULL, "play_background");
}

/* ---------------------------------------------------------------------- */
/*  Public state transition                                               */
/* ---------------------------------------------------------------------- */
void sm_enter(StateManager *sm, enum GameState new_state)
{
    // if the state is the same, don't do anything
    if (!sm || sm->state == new_state) { 
        LOG_ERROR("State_manager: Can't switch to same state"); 
        return; 
    }

    
    sm->state = new_state;

    RenderService *R = svc_get(sm->services, RENDER_SERVICE);
    if (!R) { LOG_ERROR("Render service missing in sm_enter"); return; }
    
    switch (new_state) {
        case GS_MENU:  enter_menu(sm->menu, R); break;
        case GS_PLAY:  enter_play(sm->menu, R); break;
        case GS_PAUSE: /* TODO: enter_pause(...) */ break;
        case GS_QUIT:  renderer_remove_all_layers(R); break;
    }
}

/* ---------------------------------------------------------------------- */
/*  Per-frame helpers (unchanged)                                         */
/* ---------------------------------------------------------------------- */
void sm_handle_input(StateManager *sm, const InputManager *im)
{
    if (sm->state == GS_MENU)
        menu_handle_input(sm->menu, im);

    if (sm->state == GS_PLAY && input_pressed(im, ACTION_CANCEL))
        sm_enter(sm, GS_MENU);
}

void sm_destroy(StateManager *sm)
{
    if (!sm) return;
    if (g_sm_instance == sm) g_sm_instance = NULL;
    menu_destroy(sm->menu);
    free(sm);
}

void sm_set_services(StateManager *sm, ServiceManager *svc)
{
    sm->services = svc;
    EventBus *bus = svc_get(svc, EVENT_BUS_SERVICE);
    if (bus) {
        menu_set_event_bus(sm->menu, bus);
        bus_subscribe(bus, "menu_signals", sm_handle_menu_signals);
    }
}
