// audio_manager.c
#include "audio_manager.h"
#include "../../utils/log.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <string.h>

/* ───────────────── HELPER: one-time SDL_mixer init ─────────────── */
static int mixer_init_once(void) {
    static int initialised = 0;
    if (!initialised) {
        if (Mix_OpenAudio(44100,                 /* 44.1 kHz */
                          MIX_DEFAULT_FORMAT, 2, /* stereo  */
                          2048) < 0) {
            LOG_ERROR("Mix_OpenAudio: %s\n", Mix_GetError());
            return -1;
        }
        Mix_AllocateChannels(32); /* enough for normal SFX use-cases */
        initialised = 1;
    }
    return 0;
}

/* ───────────────── LOW-LEVEL AUDIO OBJECT HELPERS ─────────────── */
Audio *audio_create(const char *path, AudioType type, int volume, int loop) {
    if (!path)
        return NULL;
    Audio *a = calloc(1, sizeof *a);
    a->path = strdup(path);
    a->type = type;
    a->volume = volume;
    a->loop = loop;
    return a;
}

void audio_destroy(Audio *a) {
    if (!a)
        return;
    switch (a->type) {
    case MUSIC:
        if (a->handle.music)
            Mix_FreeMusic(a->handle.music);
        break;
    case SFX:
    case VOICE:
        if (a->handle.chunk)
            Mix_FreeChunk(a->handle.chunk);
        break;
    default:
        break;
    }
    free(a->path);
    free(a);
}

/* ───────────────── AudioManager core ───────────────────────────── */
AudioManager *am_create(int max_count) {
    if (mixer_init_once() != 0)
        return NULL;

    AudioManager *m = calloc(1, sizeof *m);
    m->audios = calloc(max_count, sizeof(Audio *));
    m->max_count = max_count;
    return m;
}

void am_destroy(AudioManager *m) {
    if (!m)
        return;
    for (int i = 0; i < m->count; ++i)
        audio_destroy(m->audios[i]);
    free(m->audios);
    free(m);

    /* If this was the last manager you could Mix_CloseAudio() here.
       (Left out for simplicity; most games keep one manager for the
       whole app lifetime.) */
}

int am_register(AudioManager *m, Audio *audio) {
    if (m->count >= m->max_count)
        return -1;
    return (m->audios[m->count++] = audio, m->count - 1);
}

/* ───────────────── raw play/stop ───────────────── */
void play_audio_raw(Audio *a) {
    if (!a || a->is_playing)
        return;

    switch (a->type) {
    case MUSIC: {
        if (!a->handle.music) {
            a->handle.music = Mix_LoadMUS(a->path);
            if (!a->handle.music) {
                LOG_ERROR("Mix_LoadMUS(%s): %s\n", a->path, Mix_GetError());
                return;
            }
        }
        Mix_VolumeMusic(a->volume);
        if (Mix_PlayMusic(a->handle.music, a->loop ? -1 : 1) == -1) {
            LOG_ERROR("Mix_PlayMusic: %s\n", Mix_GetError());
            return;
        }
        a->is_playing = 1;
    } break;

    case SFX:
    case VOICE: {
        if (!a->handle.chunk) {
            a->handle.chunk = Mix_LoadWAV(a->path);
            if (!a->handle.chunk) {
                LOG_ERROR("Mix_LoadWAV(%s): %s\n", a->path, Mix_GetError());
                return;
            }
        }
        Mix_VolumeChunk(a->handle.chunk, a->volume);
        a->channel = Mix_PlayChannel(-1, a->handle.chunk, a->loop ? -1 : 0);
        if (a->channel == -1) {
            LOG_ERROR("Mix_PlayChannel: %s\n", Mix_GetError());
            return;
        }
        a->is_playing = 1;
    } break;

    default:
        LOG_ERROR("Unknown AudioType %d\n", a->type);
        break;
    }
}

void stop_audio_raw(Audio *a) {
    if (!a || !a->is_playing)
        return;

    switch (a->type) {
    case MUSIC:
        Mix_HaltMusic();
        break;
    case SFX:
    case VOICE:
        /* channel may be reused by mixer if sound finished; halt is safe */
        Mix_HaltChannel(a->channel);
        break;
    default:
        break;
    }
    a->is_playing = 0;
}

/* ───────────────── manager wrappers (unchanged) ───────────────── */
void am_play(AudioManager *m, Audio *a) {
    if (a->type == MUSIC && m->current_music && m->current_music->is_playing) {
        stop_audio_raw(m->current_music);
        m->current_music->is_playing = 0;
    }
    if (a->type == MUSIC)
        m->current_music = a;
    play_audio_raw(a);
}

void am_stop(AudioManager *m, Audio *a) {
    stop_audio_raw(a);
    if (a == m->current_music)
        m->current_music = NULL;
}
