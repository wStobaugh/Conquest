#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

// Hashmap entry structure
typedef struct HashMapEntry {
    char* key;
    void* value;
    struct HashMapEntry* next;
} HashMapEntry;

// Define hashmap structure
typedef struct HashMap {
    int size;
    int capacity;
    HashMapEntry* entries;
} HashMap;

// ResourceCache
typedef struct ResourceCache {
    HashMap* textures;
    HashMap* fonts;
    HashMap* sounds;
} ResourceCache;

// Hashmap functions
HashMap* hashmap_create(int capacity);
unsigned int hashmap_hash(const char* key, int capacity);
void hashmap_put(HashMap* map, const char* key, void* value);
void* hashmap_get(HashMap* map, const char* key);
void hashmap_remove(HashMap* map, const char* key);
void hashmap_destroy(HashMap* map);

// ResourceCache functions
ResourceCache* resource_cache_create();
void resource_cache_destroy(ResourceCache* cache);

#endif
