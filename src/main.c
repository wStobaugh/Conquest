#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#define CONQUEST_LOG_IMPLEMENTATION /* <- one place only */
#include "core/audio/audio_manager.h"
#include "core/compute/computation_stack.h"
#include "core/compute/computation_layers.h"
#include "core/cursor/cursor.h"
#include "core/input/input_manager.h"
#include "core/resources/resource_paths.h"
#include "core/services/service_manager.h"
#include "core/settings/settings_manager.h"
#include "core/settings/default_settings.h"
#include "core/state/state_manager.h"
#include "game_loop/game_loop.h"
#include "game_loop/initialization.h"
#include "utils/game_structs.h"
#include "utils/log.h"

// implicit declaration of function 'game_shutdown'
void game_shutdown(GameHandle *gh);



/* ---------- computation layers moved to core/compute/computation_layers.c ---- */

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Initialize logging
    char log_path[512];
    char *base_path = SDL_GetBasePath();
    snprintf(log_path, sizeof(log_path), "%slogs/conquest.log",
             base_path ? base_path : "");
    SDL_free(base_path);
    log_init(log_path);

    // Game loop initialization, it reutrns a GameHandle
    GameHandle *gh = game_init();

    // Get window dimensions
    int win_w, win_h;
    SDL_GetWindowSize(gh->win, &win_w, &win_h);

    // Create settings manager first (before other systems)
    SettingsManager *settings = sm_settings_create();
    if (!settings) {
        LOG_ERROR("Failed to create settings manager\n");
        game_shutdown(gh);
        return 1;
    }
    initialize_default_settings(settings);

    // Pass win_w/win_h to your menu/state manager, etc.
    StateManager *sm = sm_create(gh->ren, win_w, win_h, get_font_path());
    InputManager *im = input_create();
    AudioManager *am = am_create(10); // 10 is the max number of audios

    if (!sm || !im || !am) {
        LOG_ERROR("Failed to create game subsystems\n");
        game_shutdown(gh);
        return 1;
    }

    // Register services
    svc_register(gh->services, INPUT_SERVICE, im);
    svc_register(gh->services, STATE_MANAGER_SERVICE, sm);
    svc_register(gh->services, AUDIO_SERVICE, am);
    svc_register(gh->services, SETTINGS_MANAGER_SERVICE, settings);

    // Register computation layers
    register_standard_layers(gh);

    /* ---- STARTUP BACKGROUND MUSIC ------------------------------------ */
    // Loop = 1  → play forever.  Volume (0-128)
    // Apply settings to the music volume
    float music_volume = sm_get_float(settings, "music_volume");
    float master_volume = sm_get_float(settings, "master_volume");
    int sdl_volume = (int)(music_volume * master_volume * 128.0f);
    
    Audio *bg = audio_create(get_music_path("Music_1.mp3"), MUSIC, sdl_volume, 1);
    am_register(am, bg);
    am_play(am, bg);
    /* ------------------------------------------------------------------- */

    // Target FPS: 60
    Uint32 target_ms = 1000 / 60;
    while (gh->running) {
        Uint32 frame_start = SDL_GetTicks();

        game_loop(gh);

        /* fps cap … */
        Uint32 dt = SDL_GetTicks() - frame_start;
        if (dt < target_ms)
            SDL_Delay(target_ms - dt);
    }

    /* Shutdown all game systems */
    game_shutdown(gh);
    return 0;
}

/* Clean up all resources and shut down the game */
void game_shutdown(GameHandle *gh) {
    if (!gh)
        return;

    /* Clean up subsystems */
    cursor_cleanup();

    /* Clean up services from the service manager */
    if (gh->services) {
        /* Get and clean up input manager */
        InputManager *im = svc_get(gh->services, INPUT_SERVICE);
        if (im)
            input_destroy(im);

        /* Get and clean up state manager */
        StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);
        if (sm)
            sm_destroy(sm);

        /* Get and clean up audio manager */
        AudioManager *am = svc_get(gh->services, AUDIO_SERVICE);
        if (am)
            am_destroy(am);
            
        /* Get and clean up settings manager */
        SettingsManager *settings = svc_get(gh->services, SETTINGS_MANAGER_SERVICE);
        if (settings) {
            // Save settings before shutting down
            char* base_path = SDL_GetBasePath();
            char settings_path[512];
            snprintf(settings_path, sizeof(settings_path), "%ssettings.ini", base_path ? base_path : "");
            SDL_free(base_path);
            
            sm_save_settings(settings, settings_path);
            sm_settings_destroy(settings);
        }

        /* Destroy the service manager itself */
        svc_destroy(gh->services);
    }

    /* Clean up SDL resources */
    if (gh->ren)
        SDL_DestroyRenderer(gh->ren);
    if (gh->win)
        SDL_DestroyWindow(gh->win);

    /* Shutdown SDL subsystems */
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    /* Clean up the computation stack */
    comp_stack_destroy(gh->stack);
    free(gh->stack);

    /* Free the game handle */
    free(gh);

    /* Close the log system */
    log_shutdown();
}
