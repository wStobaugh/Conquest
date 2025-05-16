#include "state_manager.h"
#include "../input/input_manager.h"
#include "../audio/audio_manager.h"
#include "../settings/settings_manager.h"
#include "../resources/resource_paths.h"
#include "../resources/resource_manager.h"
#include "../../utils/log.h"
#include <string.h>
#include "../event/event_bus.h"
#include "../event/event_signals.h"
#include "../services/service_manager.h"

// At the top of the file, add a static global instance
static StateManager* g_sm_instance = NULL;

// Add this function to implement sm_get_instance
StateManager* sm_get_instance(void) {
    return g_sm_instance;
}

// Initialize menu background music
void sm_initialize_menu_music(StateManager *sm, AudioManager *am, SettingsManager *settings) {
    if (!am || !settings) {
        LOG_ERROR("Cannot initialize menu music: audio manager or settings manager is NULL\n");
        return;
    }

    // Apply settings to the music volume
    float music_volume = sm_get_float(settings, "music_volume");
    float master_volume = sm_get_float(settings, "master_volume");
    int sdl_volume = (int)(music_volume * master_volume * 128.0f);
    
    // Create and play background music
    Audio *bg = audio_create(get_music_path("Music_1.mp3"), MUSIC, sdl_volume, 1);
    if (bg) {
        am_register(am, bg);
        am_play(am, bg);
    } else {
        LOG_ERROR("Failed to create menu background music\n");
    }
}

// Event handler for menu signals
static void sm_handle_menu_signals(const Event *e) {
    if (e->type != EVENT_TYPE_SIGNAL || !e->data)
        return;
        
    MenuSignal signal = *(MenuSignal*)e->data;
    
    // Free the allocated memory
    free(e->data);
    
    // Get the state manager instance
    StateManager *sm = g_sm_instance;
    if (!sm) return;
    
    switch (signal) {
        case MENU_SIGNAL_GOTO_CONTINUE:
            sm->state = GS_PLAY; // Load saved game
            break;
        case MENU_SIGNAL_GOTO_NEW_GAME:
            sm->state = GS_PLAY; // Create new game
            break;
        case MENU_SIGNAL_GOTO_OPTIONS:
            // Show options menu (handled by menu system)
            break;
        case MENU_SIGNAL_GOTO_QUIT:
            sm->state = GS_QUIT;
            break;
        case MENU_SIGNAL_GOTO_MAIN:
            // Return to main menu (handled by menu system)
            break;
        default:
            break;
    }
}

// Update sm_create to accept resource manager parameter
StateManager *sm_create(SDL_Renderer *ren, int w, int h, ResourceManager *resource_manager) {
    StateManager *sm = calloc(1, sizeof *sm);
    sm->state = GS_MENU;
    sm->menu = menu_create(ren, w, h, NULL, resource_manager);
    
    // Store the instance globally
    g_sm_instance = sm;
    
    // If services are already attached, get the event bus
    if (sm->services) {
        EventBus *bus = svc_get(sm->services, EVENT_BUS_SERVICE);
        if (bus) {
            // Set the event bus for the menu
            menu_set_event_bus(sm->menu, bus);
            
            // Subscribe to menu signal events
            bus_subscribe(bus, "menu_signals", sm_handle_menu_signals);
        }
    }
    
    return sm;
}

// Set the audio manager for the state manager's menu
void sm_set_audio_manager(StateManager *sm, AudioManager *am) {
    if (!sm || !sm->menu) return;
    
    // Update the menu's audio manager
    sm->menu->audio_manager = am;
}

// This function renders the current state of the state manager.
void sm_render(StateManager *sm, SDL_Renderer *ren) {
    if (sm->state == GS_MENU)
        menu_render(sm->menu, ren);
    else if (sm->state == GS_PLAY) {
        SDL_SetRenderDrawColor(ren, 0, 20, 40, 255);
        SDL_RenderClear(ren);
    }
}

// This functions handles input events depending on the current state.
void sm_handle_input(StateManager *sm, const InputManager *im) {
    if (sm->state == GS_MENU)
        menu_handle_input(sm->menu, im);

    /* later: add gameplay input here */

    // Pressing ESCAPE in the game state should return to the menu
    if (sm->state == GS_PLAY && input_pressed(im, ACTION_CANCEL))
        sm->state = GS_MENU;
}

// Update sm_destroy to clear the instance
void sm_destroy(StateManager *sm) {
    if (!sm)
        return;
    if (g_sm_instance == sm)
        g_sm_instance = NULL;
    menu_destroy(sm->menu);
    free(sm);
}

// Implement setting services
void sm_set_services(StateManager *sm, ServiceManager *services) {
    if (!sm) return;
    sm->services = services;
    
    // Try to get event bus and set up subscriptions
    EventBus *bus = svc_get(services, EVENT_BUS_SERVICE);
    if (bus && sm->menu) {
        menu_set_event_bus(sm->menu, bus);
        bus_subscribe(bus, "menu_signals", sm_handle_menu_signals);
    }
}