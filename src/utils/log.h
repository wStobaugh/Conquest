/*============================================================================
 *  Conquest Logging — header‑only, portable, colorful, file‑aware logger
 *  ---------------------------------------------------------------------
 *  Drop‑in usage:
 *      #include "utils/log.h"
 *      LOG_INFO("Hello %d", 42);
 *
 *  Compile‑time flags (define before including this file):
 *      NDEBUG                         → disables TRACE / DEBUG logs
 *      CONQUEST_LOG_NO_COLOR          → disable ANSI color output everywhere
 *      CONQUEST_LOG_NO_WIN32_ANSI     → skip Win32 console‑mode tweaking
 *      CONQUEST_LOG_FILE="conquest.log" → default log file at build‑time
 *      CONQUEST_LOG_THREAD_SAFE       → uses SDL_mutex for locking
 *
 *  Runtime API:
 *      log_init(path)        → send output to |path| (NULL ⇒ stdout)
 *      log_set_level(lvl)    → change minimum level dynamically
 *      log_shutdown()        → flush / close file
 *
 *  Implementation notes
 *  --------------------
 *  • Fully OS‑agnostic *except* for an optional Win32 console tweak that
 *    enables ANSI escape processing on Windows 10+.  Surrounding #ifdefs
 *    keep all non‑portable code out of other platforms, and you can disable
 *    it entirely with CONQUEST_LOG_NO_WIN32_ANSI.
 *  • Everything else relies only on the C standard library (plus SDL if you
 *    opt‑in to CONQUEST_LOG_THREAD_SAFE).
 *===========================================================================*/
#ifndef CONQUEST_LOG_H
#define CONQUEST_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(CONQUEST_LOG_THREAD_SAFE)
#  include <SDL2/SDL_mutex.h>
#endif

#if defined(_WIN32) && !defined(CONQUEST_LOG_NO_WIN32_ANSI)
#  include <windows.h>   /* for HANDLE / ConsoleMode APIs */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*───────────────────────────────────────────────────────────────────────────*/
/*  Public interface                                                         */
/*───────────────────────────────────────────────────────────────────────────*/

typedef enum {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;

void log_init     (const char *file_path); /* NULL ⇒ stdout               */
void log_set_level(LogLevel lvl);          /* change filter after init    */
void log_shutdown (void);                  /* flush + close file          */

void log_write_internal(LogLevel lvl, const char *file, int line,
                        const char *fmt, ...) __attribute__((format(printf,4,5)));

/*──────────────────────────── Variadic convenience macros ─────────────────*/

#if defined(NDEBUG)
#  define LOG_TRACE(...) (void)0
#  define LOG_DEBUG(...) (void)0
#else
#  define LOG_TRACE(...) log_write_internal(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#  define LOG_DEBUG(...) log_write_internal(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#endif

#define LOG_INFO(...)  log_write_internal(LOG_LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_write_internal(LOG_LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_write_internal(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_write_internal(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/*──────────────────────────────── Internal implementation ─────────────────*/
#ifdef CONQUEST_LOG_IMPLEMENTATION

static struct {
    FILE     *fp;
    int       use_color;
#if defined(CONQUEST_LOG_THREAD_SAFE)
    SDL_mutex *mtx;
#endif
    LogLevel  level;
} log_state = {
    .fp        = NULL,
    .use_color = 1,
#if defined(CONQUEST_LOG_THREAD_SAFE)
    .mtx       = NULL,
#endif
#ifdef NDEBUG
    .level     = LOG_LEVEL_INFO,
#else
    .level     = LOG_LEVEL_TRACE,
#endif
};

static const char *level_names[]  = {"TRACE","DEBUG","INFO","WARN","ERROR","FATAL"};
static const char *level_colors[] = {"\x1b[90m","\x1b[36m","\x1b[32m","\x1b[33m","\x1b[31m","\x1b[35m"};

static inline void _log_lock(void) {
#if defined(CONQUEST_LOG_THREAD_SAFE)
    if (log_state.mtx) SDL_LockMutex(log_state.mtx);
#endif
}

static inline void _log_unlock(void) {
#if defined(CONQUEST_LOG_THREAD_SAFE)
    if (log_state.mtx) SDL_UnlockMutex(log_state.mtx);
#endif
}

void log_init(const char *file_path) {
    _log_lock();
    if (log_state.fp) { _log_unlock(); return; } /* already initialised */

    if (file_path && *file_path) {
        log_state.fp = fopen(file_path, "w");
        if (!log_state.fp) log_state.fp = stdout; /* fallback */
    } else {
#ifdef CONQUEST_LOG_FILE
        log_state.fp = fopen(CONQUEST_LOG_FILE, "w");
        if (!log_state.fp) log_state.fp = stdout;
#else
        log_state.fp = stdout;
#endif
    }

#if defined(_WIN32) && !defined(CONQUEST_LOG_NO_COLOR) && !defined(CONQUEST_LOG_NO_WIN32_ANSI)
    /* Enable ANSI escape processing on Windows 10+ consoles. Optional. */
    if (log_state.fp == stdout) {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h != INVALID_HANDLE_VALUE) {
            DWORD mode = 0;
            if (GetConsoleMode(h, &mode)) {
                mode |= 0x0004; /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
                SetConsoleMode(h, mode);
            }
        }
    }
#endif

#if defined(CONQUEST_LOG_THREAD_SAFE)
    log_state.mtx = SDL_CreateMutex();
#endif

#ifndef CONQUEST_LOG_NO_COLOR
    log_state.use_color = (log_state.fp == stdout);
#else
    log_state.use_color = 0;
#endif
    _log_unlock();
}

void log_set_level(LogLevel lvl) { log_state.level = lvl; }

void log_shutdown(void) {
    _log_lock();
    if (log_state.fp && log_state.fp != stdout) fclose(log_state.fp);
#if defined(CONQUEST_LOG_THREAD_SAFE)
    if (log_state.mtx) SDL_DestroyMutex(log_state.mtx);
#endif
    memset(&log_state, 0, sizeof log_state);
    _log_unlock();
}

void log_write_internal(LogLevel lvl, const char *file, int line,
                        const char *fmt, ...) {
    if (lvl < log_state.level) return; /* filtered */
    if (!log_state.fp) log_init(NULL);

    _log_lock();

    /* timestamp */
    time_t now = time(NULL);
    struct tm tm_info;
#if defined(_WIN32)
    localtime_s(&tm_info, &now);
#else
    localtime_r(&now, &tm_info);
#endif
    char time_buf[20];
    strftime(time_buf, sizeof time_buf, "%H:%M:%S", &tm_info);

    /* prefix */
    if (log_state.use_color)
        fprintf(log_state.fp, "%s%s %-5s\x1b[0m \x1b[90m%-20s:%d\x1b[0m ",
                level_colors[lvl], time_buf, level_names[lvl], file, line);
    else
        fprintf(log_state.fp, "%s %-5s %-20s:%d ",
                time_buf, level_names[lvl], file, line);

    /* message */
    va_list ap;
    va_start(ap, fmt);
    vfprintf(log_state.fp, fmt, ap);
    va_end(ap);
    fputc('\n', log_state.fp);
    fflush(log_state.fp);

    _log_unlock();

    if (lvl == LOG_LEVEL_FATAL) {
        log_shutdown();
        abort();
    }
}

#endif /* CONQUEST_LOG_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* CONQUEST_LOG_H */
