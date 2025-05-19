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

/* Initialize logging system */
void initialize_logging(void) {
    char log_path[512];
    char *base_path = SDL_GetBasePath();
    snprintf(log_path, sizeof(log_path), "%slogs/conquest.log",
             base_path ? base_path : "");
    SDL_free(base_path);
    log_init(log_path);
}


/* ---------- computation layers moved to core/compute/computation_layers.c ---- */

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Initialize logging
    initialize_logging();

    // Game loop initialization, it reutrns a GameHandle
    GameHandle *gh = game_init();

    // Initialize core services (state, input, audio, settings managers)
    if (!initialize_core_services(gh)) {
        LOG_ERROR("Failed to initialize core services\n");
        game_shutdown(gh);
        return 1;
    }

    // Register computation layers
    register_standard_layers(gh);

    // Get the state manager to check for GS_QUIT state
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);
    while (sm && sm->state != GS_QUIT) {
        game_loop(gh);
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
            LOG_INFO("Saving settings to: %ssettings.ini", base_path ? base_path : "");
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

    /* Free the game handle */
    free(gh);

    /* Close the log system */
    log_shutdown();
}
