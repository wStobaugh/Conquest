#include "resource_cache.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#define RESOURCE_CACHE_DEFAULT_CAPACITY 1024



// Create hashmap for types of resources
HashMap* hashmap_create(int capacity) {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    map->size = 0;
    map->capacity = capacity > 0 ? capacity : 16; // Default capacity if invalid
    map->entries = (HashMapEntry*)calloc(map->capacity, sizeof(HashMapEntry));
    return map;
}

// resource_cache_create
ResourceCache* resource_cache_create() {
    ResourceCache* cache = (ResourceCache*)malloc(sizeof(ResourceCache));
    cache->textures = hashmap_create(RESOURCE_CACHE_DEFAULT_CAPACITY);
    cache->fonts = hashmap_create(RESOURCE_CACHE_DEFAULT_CAPACITY);
    cache->sounds = hashmap_create(RESOURCE_CACHE_DEFAULT_CAPACITY);
    return cache;
}

// Hash function for strings
unsigned int hashmap_hash(const char* key, int capacity) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + (*key++);
    }
    return hash % capacity;
}

// Insert or update a key-value pair
void hashmap_put(HashMap* map, const char* key, void* value) {
    if (!map || !key) return;
    
    unsigned int index = hashmap_hash(key, map->capacity);
    HashMapEntry* entry = &map->entries[index];
    
    // If this slot is empty
    if (entry->key == NULL) {
        entry->key = strdup(key);
        entry->value = value;
        entry->next = NULL;
        map->size++;
        return;
    }
    
    // Check if key exists and update
    HashMapEntry* current = entry;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        if (!current->next) break;
        current = current->next;
    }
    
    // Add new entry to the chain
    HashMapEntry* new_entry = (HashMapEntry*)malloc(sizeof(HashMapEntry));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = NULL;
    current->next = new_entry;
    map->size++;
}

// Get a value by key
void* hashmap_get(HashMap* map, const char* key) {
    if (!map || !key) return NULL;
    
    unsigned int index = hashmap_hash(key, map->capacity);
    HashMapEntry* entry = &map->entries[index];
    
    if (entry->key == NULL) return NULL;
    
    HashMapEntry* current = entry;
    while (current) {
        if (current->key && strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    return NULL;
}

// Remove a key-value pair
void hashmap_remove(HashMap* map, const char* key) {
    if (!map || !key) return;
    
    unsigned int index = hashmap_hash(key, map->capacity);
    HashMapEntry* entry = &map->entries[index];
    
    if (entry->key == NULL) return;
    
    // If it's the first entry
    if (strcmp(entry->key, key) == 0) {
        if (entry->next) {
            HashMapEntry* next = entry->next;
            free(entry->key);
            entry->key = strdup(next->key);
            entry->value = next->value;
            entry->next = next->next;
            free(next->key);
            free(next);
        } else {
            free(entry->key);
            entry->key = NULL;
            entry->value = NULL;
        }
        map->size--;
        return;
    }
    
    // If it's in the chain
    HashMapEntry* prev = entry;
    HashMapEntry* current = entry->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            prev->next = current->next;
            free(current->key);
            free(current);
            map->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Free all memory used by the hashmap
void hashmap_destroy(HashMap* map) {
    if (!map) return;
    
    for (int i = 0; i < map->capacity; i++) {
        HashMapEntry* entry = &map->entries[i];
        if (entry->key) {
            free(entry->key);
            
            // Free linked entries
            HashMapEntry* current = entry->next;
            while (current) {
                HashMapEntry* next = current->next;
                free(current->key);
                free(current);
                current = next;
            }
        }
    }
    
    free(map->entries);
    free(map);
}

// Destroy the resource cache and free all resources
void resource_cache_destroy(ResourceCache* cache) {
    if (!cache) return;
    
    // Free all textures
    for (int i = 0; i < cache->textures->capacity; i++) {
        HashMapEntry* entry = &cache->textures->entries[i];
        if (entry->key) {
            // Free the SDL texture
            SDL_Texture* texture = (SDL_Texture*)entry->value;
            if (texture) {
                SDL_DestroyTexture(texture);
            }
            
            // Check for chained entries
            HashMapEntry* current = entry->next;
            while (current) {
                texture = (SDL_Texture*)current->value;
                if (texture) {
                    SDL_DestroyTexture(texture);
                }
                current = current->next;
            }
        }
    }
    
    // Free all fonts
    for (int i = 0; i < cache->fonts->capacity; i++) {
        HashMapEntry* entry = &cache->fonts->entries[i];
        if (entry->key) {
            // Free the TTF font
            TTF_Font* font = (TTF_Font*)entry->value;
            if (font) {
                TTF_CloseFont(font);
            }
            
            // Check for chained entries
            HashMapEntry* current = entry->next;
            while (current) {
                font = (TTF_Font*)current->value;
                if (font) {
                    TTF_CloseFont(font);
                }
                current = current->next;
            }
        }
    }
    
    // Free all sounds
    // Note: Specific cleanup depends on the sound implementation (SDL_mixer, etc.)
    // This is a placeholder for future implementation
    
    // Destroy the hashmaps
    hashmap_destroy(cache->textures);
    hashmap_destroy(cache->fonts);
    hashmap_destroy(cache->sounds);
    
    // Free the cache itself
    free(cache);
}