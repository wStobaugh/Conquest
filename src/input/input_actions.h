#ifndef CONQUEST_INPUT_ACTIONS_H
#define CONQUEST_INPUT_ACTIONS_H

/* Add to this enum as your game grows */
typedef enum {
    ACTION_NONE = 0,

    /* navigation / movement */
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,

    /* UI / gameplay */
    ACTION_CONFIRM,     // “A”, “Enter”, left-click
    ACTION_CANCEL,      // “B”, “Esc”, right-click
    ACTION_QUIT,        // Alt-F4, window close, etc.

    ACTION_COUNT        // keep last
} InputAction;

#endif /* CONQUEST_INPUT_ACTIONS_H */
