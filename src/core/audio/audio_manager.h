// audio_manager.h
#pragma once
#include <SDL2/SDL_mixer.h>

typedef enum { MUSIC, SFX, VOICE, AMBIENT } AudioType;

typedef struct Audio {
    char           *path;
    AudioType       type;
    int             volume;
    int             loop;
    int             is_playing;
    union {
        Mix_Music     *music;
        Mix_Chunk     *chunk;
        SDL_AudioDeviceID dev;
    } handle;
    int             channel;  // for SFX channels
} Audio;

/* ------------- low-level helpers you can call from game code -------------- */
Audio *audio_create (const char *path, AudioType type,
                     int volume, int loop);
void   audio_destroy(Audio *a);


typedef struct AudioManager {
    Audio         **audios;
    int             count;
    int             max_count;
    Audio          *current_music;
} AudioManager;

// create/destroy
AudioManager *am_create(int max_count);
void           am_destroy(AudioManager *mgr);

// register/unregister sounds
// returns index (>=0) on success, -1 if full
int            am_register(AudioManager *mgr, Audio *audio);

// high-level play/stop via manager:
void           am_play   (AudioManager *mgr, Audio *audio);
void           am_stop   (AudioManager *mgr, Audio *audio);
