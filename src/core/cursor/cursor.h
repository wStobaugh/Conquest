#ifndef CONQUEST_CURSOR_H
#define CONQUEST_CURSOR_H
#include <SDL2/SDL.h>

typedef enum {
    CURSOR_NORMAL,
    CURSOR_SELECT
} CursorType;

/* returns 0 on success, -1 on error (check SDL/IMG logs) */
int  cursor_init(void);          /* call once at startup          */
void cursor_set(CursorType t);   /* swap between the two skins    */
void cursor_cleanup(void);       /* call once before SDL_Quit()   */
#endif
