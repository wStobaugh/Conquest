#ifndef EVENT_SIGNALS_H
#define EVENT_SIGNALS_H

// Menu signal enum to replace string-based signals
typedef enum MenuSignal {
    MENU_SIGNAL_NONE,
    MENU_SIGNAL_GOTO_CONTINUE,
    MENU_SIGNAL_GOTO_NEW_GAME,
    MENU_SIGNAL_GOTO_OPTIONS,
    MENU_SIGNAL_GOTO_QUIT,
    MENU_SIGNAL_GOTO_MAIN,
    // Add other menu signals as needed
} MenuSignal;

#endif // EVENT_SIGNALS_H 