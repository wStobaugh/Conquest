#include "resource_manager.h"
#include "resource_cache.h"
#include "resource_paths.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

ResourceManager* resource_manager_create() {
    ResourceManager* manager = (ResourceManager*)malloc(sizeof(ResourceManager));
    manager->cache = resource_cache_create();
    return manager;
}

void resource_manager_destroy(ResourceManager* manager) {
    resource_cache_destroy(manager->cache);
    free(manager);
}


SDL_Texture* load_texture(ResourceManager* manager, const char* sub_path, SDL_Renderer* renderer) {
    // Get the full path for the texture file
    const char* path = get_image_path(sub_path);
    if (!path) {
        SDL_Log("Error: Could not get texture path for %s", sub_path);
        return NULL;
    }
    
    
    // Check if texture is already in cache
    SDL_Texture* texture = hashmap_get(manager->cache->textures, path);
    if (texture != NULL) {
        return texture;
    }
    
    // Load the texture from file
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) {
        SDL_Log("Failed to load image %s: %s", path, IMG_GetError());
        return NULL;
    }
    
    // Create texture from surface
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (texture == NULL) {
        SDL_Log("Failed to create texture from %s: %s", path, SDL_GetError());
        return NULL;
    }
    
    // Store in cache
    hashmap_put(manager->cache->textures, path, texture);
    
    return texture;
}

TTF_Font* load_font(ResourceManager* manager, const char* file_name, int size) {
    // Get the full path for the font file
    const char* path = get_font_path(file_name);
    if (!path) {
        SDL_Log("Error: Could not get font path for %s", file_name);
        return NULL;
    }
    
    // Create a unique key that includes both path and size
    char key[256];
    snprintf(key, sizeof(key), "%s_%d", path, size);
    
    // Check if font is already in cache
    TTF_Font* font = hashmap_get(manager->cache->fonts, key);
    if (font != NULL) {
        return font;
    }
    
    // Load the font from file
    font = TTF_OpenFont(path, size);
    if (font == NULL) {
        SDL_Log("Failed to load font %s at size %d: %s", path, size, TTF_GetError());
        return NULL;
    }
    
    // Store in cache
    hashmap_put(manager->cache->fonts, key, font);
    
    return font;
}

Mix_Chunk* load_chunk(ResourceManager* manager, const char* sub_path) {
    // Get the full path for the sound effect file
    const char* path = get_sfx_path(sub_path);
    if (!path) {
        SDL_Log("Error: Could not get sound effect path for %s", sub_path);
        return NULL;
    }
    
    // Check if chunk is already in cache
    Mix_Chunk* chunk = hashmap_get(manager->cache->sounds, path);
    if (chunk != NULL) {
        return chunk;
    }
    
    // Load the chunk from file
    chunk = Mix_LoadWAV(path);
    if (chunk == NULL) {
        SDL_Log("Failed to load sound effect %s: %s", path, Mix_GetError());
        return NULL;
    }
    
    // Store in cache
    hashmap_put(manager->cache->sounds, path, chunk);
    
    return chunk;
}

Mix_Music* load_music(ResourceManager* manager, const char* sub_path) {
    // Get the full path for the music file
    const char* path = get_music_path(sub_path);
    if (!path) {
        SDL_Log("Error: Could not get music path for %s", sub_path);
        return NULL;
    }
    
    // Check if music is already in cache
    Mix_Music* music = hashmap_get(manager->cache->sounds, path);
    if (music != NULL) {
        return music;
    }
    
    // Load the music from file
    music = Mix_LoadMUS(path);
    if (music == NULL) {
        SDL_Log("Failed to load music %s: %s", path, Mix_GetError());
        return NULL;
    }
    
    // Store in cache
    hashmap_put(manager->cache->sounds, path, music);
    
    return music;
}

SDL_Surface* load_surface(ResourceManager* manager, const char* sub_path) {
    // Get the full path for the image file
    const char* path = get_image_path(sub_path);
    if (!path) {
        SDL_Log("Error: Could not get image path for %s", sub_path);
        return NULL;
    }
    
    // Check if surface is already in cache
    SDL_Surface* surface = hashmap_get(manager->cache->textures, path);
    if (surface != NULL) {
        return surface;
    }
    
    // Load the surface from file
    surface = IMG_Load(path);
    if (surface == NULL) {
        SDL_Log("Failed to load image %s: %s", path, IMG_GetError());
        return NULL;
    }
    
    // Store in cache
    hashmap_put(manager->cache->textures, path, surface);
    
    return surface;
}
