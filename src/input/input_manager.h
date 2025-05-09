#ifndef CONQUEST_INPUT_MANAGER_H
#define CONQUEST_INPUT_MANAGER_H
#include <SDL2/SDL.h>
#include "input_actions.h"

typedef struct InputManager InputManager;

/* life-cycle */
InputManager *input_create(void);
void          input_destroy(InputManager *im);

/* pump raw SDL events */
void          input_handle_event(InputManager *im, const SDL_Event *e);

/* mark end of frame (called once per frame) */
void          input_update(InputManager *im);

/* queries — true for *this* frame only                            */
int  input_pressed (const InputManager *im, InputAction a);  // just went down
int  input_released(const InputManager *im, InputAction a);  // just went up
int  input_held    (const InputManager *im, InputAction a);  // is down
void input_mouse_pos(const InputManager *im, int *x, int *y);

#endif /* CONQUEST_INPUT_MANAGER_H */
