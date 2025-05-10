
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#define CONQUEST_LOG_IMPLEMENTATION   /* <- one place only */
#include "utils/log.h"
#include "utils/game_structs.h"
#include "core/input/input_manager.h"
#include "core/state/state_manager.h"
#include "core/services/service_manager.h"
#include "core/cursor/cursor.h"
#include "game_loop/game_loop.h"
#include "game_loop/initialization.h"

// implicit declaration of function 'game_shutdown'
void game_shutdown(GameHandle *gh);

static const char *font_path() {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/fonts/OpenSans-Regular.ttf",
             base ? base : "");
    SDL_free(base);
    return buf;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Initialize logging
    char log_path[512];
    char *base_path = SDL_GetBasePath();
    snprintf(log_path, sizeof(log_path), "%slogs/conquest.log", base_path ? base_path : "");
    SDL_free(base_path);
    log_init(log_path);

    // Game loop initialization, it reutrns a GameHandle
    GameHandle *gh = game_init();

    // Get window dimensions
    int win_w, win_h;
    SDL_GetWindowSize(gh->win, &win_w, &win_h);

    // Pass win_w/win_h to your menu/state manager, etc.
    StateManager *sm = sm_create(gh->ren, win_w, win_h, font_path());
    InputManager *im = input_create();

    // Register services
    svc_register(gh->services, SERVICE_INPUT, sm);
    svc_register(gh->services, SERVICE_STATE, im);

    Uint32 target_ms = 1000 / 60;
    int running = 1;
    while (running) {
        // TODO: Run game_loop here
        Uint32 frame_start = SDL_GetTicks();
        SDL_Event e;
        while (SDL_PollEvent(&e))
            input_handle_event(im, &e);

        /* global hot-keys */
        if (input_pressed(im, ACTION_QUIT))
            running = 0;
        /* ---------- let the state react to actions ---------------- */
        sm_handle_input(sm, im);

        sm_update(sm);
        sm_render(sm, gh->ren);
        SDL_RenderPresent(gh->ren);
        input_update(im); /* wipe pressed / released for next frame */

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
    if (!gh) return;
    
    /* Clean up subsystems */
    cursor_cleanup();
    
    /* Clean up services from the service manager */
    if (gh->services) {
        /* Get and clean up input manager */
        InputManager *im = svc_get(gh->services, SERVICE_INPUT);
        if (im) input_destroy(im);
        
        /* Get and clean up state manager */
        StateManager *sm = svc_get(gh->services, SERVICE_STATE);
        if (sm) sm_destroy(sm);
        
        /* Destroy the service manager itself */
        svc_destroy(gh->services);
    }
    
    /* Clean up SDL resources */
    if (gh->ren) SDL_DestroyRenderer(gh->ren);
    if (gh->win) SDL_DestroyWindow(gh->win);
    
    /* Shutdown SDL subsystems */
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    /* Free the game handle */
    free(gh);
    
    /* Close the log system */
    log_shutdown();
}