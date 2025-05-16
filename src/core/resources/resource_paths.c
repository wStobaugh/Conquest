#include "resource_paths.h"
#include <SDL2/SDL.h>
#include <stdio.h>

const char *get_font_path(const char *file) {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/fonts/%s", base ? base : "",
             file);
    SDL_free(base);
    return buf;
}

const char *get_music_path(const char *file) {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/audio/music/%s", base ? base : "",
             file);
    SDL_free(base);
    return buf;
}

const char *get_image_path(const char *file) {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/images/%s", base ? base : "",
             file);
    SDL_free(base);
    return buf;
}

const char *get_sfx_path(const char *file) {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/audio/sfx/%s", base ? base : "",
             file);
    SDL_free(base);
    return buf;
} 