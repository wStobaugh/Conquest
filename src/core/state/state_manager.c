#include "state_manager.h"
#include "../input/input_manager.h"
#include "../audio/audio_manager.h"
#include "../settings/settings_manager.h"
#include "../resources/resource_paths.h"
#include "../../utils/log.h"
#include <string.h>

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

// This is how we initialize a new game state manager.
StateManager *sm_create(SDL_Renderer *ren, int w, int h,
                        const char *font_path) {
    StateManager *sm = calloc(1, sizeof *sm);
    sm->state = GS_MENU;
    
    // For now, we'll have to create menu without audio, audio will be added later
    sm->menu = menu_create(ren, w, h, font_path, NULL);
    return sm;
}

// Set the audio manager for the state manager's menu
void sm_set_audio_manager(StateManager *sm, AudioManager *am) {
    if (!sm || !sm->menu) return;
    
    // Update the menu's audio manager
    sm->menu->audio_manager = am;
}

// This function updates the state manager.
void sm_update(StateManager *sm) {
    if (sm->state == GS_MENU) {
        const char *sig = menu_pop_signal(sm->menu);
        if (!sig)
            return;
        if (strcmp(sig, "goto_newgame") == 0 ||
            strcmp(sig, "goto_continue") == 0) {
            sm->state = GS_PLAY; /* TODO: load or create world */
        }
    }
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

// This function destroys the state manager and frees its resources.
void sm_destroy(StateManager *sm) {
    if (!sm)
        return;
    menu_destroy(sm->menu);
    free(sm);
}