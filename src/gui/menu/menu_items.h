#ifndef CONQUEST_GUI_MENU_ITEMS_H
#define CONQUEST_GUI_MENU_ITEMS_H

typedef struct Menu Menu;
typedef enum MenuSignal MenuSignal;

void menu_build_main(Menu *m);
void menu_build_placeholder(Menu *m, const char *title);
void menu_build_from_signal(Menu *m, MenuSignal signal);

#endif // CONQUEST_GUI_MENU_ITEMS_H