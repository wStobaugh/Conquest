#include "input_manager.h"
#include <stdlib.h>
#include <string.h> /* memset */

struct InputManager {
    /* 3 parallel bit-fields: 1 bit per action                          */
    uint32_t down, pressed, released;
    int mouse_x, mouse_y;
};

/* ---------- helpers ---------------------------------------------------- */

static InputAction key_to_action(SDL_Scancode sc) {
    switch (sc) {
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP:
        return ACTION_MOVE_UP;
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN:
        return ACTION_MOVE_DOWN;
    case SDL_SCANCODE_A:
    case SDL_SCANCODE_LEFT:
        return ACTION_MOVE_LEFT;
    case SDL_SCANCODE_D:
    case SDL_SCANCODE_RIGHT:
        return ACTION_MOVE_RIGHT;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_SPACE:
        return ACTION_CONFIRM;
    case SDL_SCANCODE_ESCAPE:
        return ACTION_CANCEL;
    default:
        return ACTION_NONE;
    }
}

static void set_bit(uint32_t *mask, InputAction a) { *mask |= (1u << a); }
static void clr_bit(uint32_t *mask, InputAction a) { *mask &= ~(1u << a); }
static int get_bit(uint32_t mask, InputAction a) { return (mask >> a) & 1u; }

/* ---------- public API ------------------------------------------------- */

InputManager *input_create(void) { return calloc(1, sizeof(InputManager)); }

void input_destroy(InputManager *im) { free(im); }

/* feed every SDL event here */
void input_handle_event(InputManager *im, const SDL_Event *e) {

    /* reset “just” flags when next frame starts (see input_update)      */
    if (e->type == SDL_KEYDOWN && !e->key.repeat) {
        InputAction a = key_to_action(e->key.keysym.scancode);
        if (a != ACTION_NONE && !get_bit(im->down, a)) {
            set_bit(&im->down, a);
            set_bit(&im->pressed, a);
        }
    }
    if (e->type == SDL_KEYUP) {
        InputAction a = key_to_action(e->key.keysym.scancode);
        if (a != ACTION_NONE) {
            clr_bit(&im->down, a);
            set_bit(&im->released, a);
        }
    }

    /* mouse buttons → actions */
    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        set_bit(&im->pressed, ACTION_CONFIRM);
        set_bit(&im->down, ACTION_CONFIRM);
    }
    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT) {
        clr_bit(&im->down, ACTION_CONFIRM);
        set_bit(&im->released, ACTION_CONFIRM);
    }

    /* cursor */
    if (e->type == SDL_MOUSEMOTION) {
        im->mouse_x = e->motion.x;
        im->mouse_y = e->motion.y;
    }

    if (e->type == SDL_QUIT)
        set_bit(&im->pressed, ACTION_QUIT);
}

void input_update(InputManager *im) {
    /* clear transient states after user code has had a chance to read them */
    im->pressed = 0;
    im->released = 0;
}

/* queries */
int input_pressed(const InputManager *im, InputAction a) {
    return get_bit(im->pressed, a);
}
int input_released(const InputManager *im, InputAction a) {
    return get_bit(im->released, a);
}
int input_held(const InputManager *im, InputAction a) {
    return get_bit(im->down, a);
}

void input_mouse_pos(const InputManager *im, int *x, int *y) {
    if (x)
        *x = im->mouse_x;
    if (y)
        *y = im->mouse_y;
}
