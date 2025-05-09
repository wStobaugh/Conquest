#include "initialization.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Set mouse image
static const char *mouse_path() {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/images/ui/cursor_normal.png",
             base ? base : "");
    SDL_free(base);
    return buf;
}


static void SDL_CheckErrors() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return 1;
      }
      if (TTF_Init() != 0) {
        SDL_Log("TTF: %s", TTF_GetError());
        SDL_Quit();
        return 1;
      }
      if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("IMG_Init failed: %s", IMG_GetError());
        SDL_Quit();
        return 1;
      }
      if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
}


// entry function 
void game_init() {
    // Initialize SDL & check for errors
    SDL_CheckErrors();
    
    // Set the working directory to the base path of game
    char *base = SDL_GetBasePath();
    _chdir(base);
    SDL_free(base);
}
