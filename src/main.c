
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#define CONQUEST_LOG_IMPLEMENTATION /* <- one place only */
#include "core/audio/audio_manager.h"
#include "core/compute/computation_stack.h"
#include "core/cursor/cursor.h"
#include "core/input/input_manager.h"
#include "core/services/service_manager.h"
#include "core/state/state_manager.h"
#include "game_loop/game_loop.h"
#include "game_loop/initialization.h"
#include "utils/game_structs.h"
#include "utils/log.h"

// implicit declaration of function 'game_shutdown'
void game_shutdown(GameHandle *gh);

// TODO: move this to a separate file
static const char *font_path() {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/fonts/OpenSans-Regular.ttf",
             base ? base : "");
    SDL_free(base);
    return buf;
}

static const char *music_path(const char *file) {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/audio/music/%s", base ? base : "",
             file);
    SDL_free(base);
    return buf;
}

/* ---------- computation layers ------------------------------------ */
// TODO: We probably want to move this to a separate file
static void layer_state_input(GameHandle *gh) {
    InputManager *im = svc_get(gh->services, INPUT_SERVICE);
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);
    sm_handle_input(sm, im);
}

static void layer_state_update(GameHandle *gh) {
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);
    sm_update(sm);
}

static void layer_state_render(GameHandle *gh) {
    StateManager *sm = svc_get(gh->services, STATE_MANAGER_SERVICE);
    sm_render(sm, gh->ren);
}

static void layer_present(GameHandle *gh) {
    SDL_RenderPresent(gh->ren);
    InputManager *im = svc_get(gh->services, INPUT_SERVICE);
    input_update(im);
}

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

    // Pass win_w/win_h to your menu/state manager, etc.
    StateManager *sm = sm_create(gh->ren, win_w, win_h, font_path());
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

    // Register computation layers
    push_layer(gh, "sm_input", layer_state_input, 300);
    push_layer(gh, "sm_update", layer_state_update, 200);
    push_layer(gh, "sm_render", layer_state_render, 100);
    push_layer(gh, "present", layer_present, 0);

    /* ---- STARTUP BACKGROUND MUSIC ------------------------------------ */
    // Loop = 1  → play forever.  Volume (0-128)
    // TODO: Music level should be controlled by settings
    Audio *bg = audio_create(music_path("Music_1.mp3"), MUSIC, 8, 1);
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
