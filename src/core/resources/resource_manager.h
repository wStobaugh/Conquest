#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "resource_cache.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// Forward declarations
typedef struct ResourceCache ResourceCache;

// Resource manager structure
typedef struct ResourceManager {
    ResourceCache* cache;
} ResourceManager;

ResourceManager* resource_manager_create();
void resource_manager_destroy(ResourceManager* manager);
SDL_Texture* load_texture(ResourceManager* manager, const char* sub_path, SDL_Renderer* renderer);
TTF_Font* load_font(ResourceManager* manager, const char* file_name, int size);
Mix_Chunk* load_chunk(ResourceManager* manager, const char* sub_path);
Mix_Music* load_music(ResourceManager* manager, const char* sub_path);
SDL_Surface* load_surface(ResourceManager* manager, const char* sub_path);

#endif