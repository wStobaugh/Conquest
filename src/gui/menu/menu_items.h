#ifndef CONQUEST_GUI_MENU_ITEMS_H
#define CONQUEST_GUI_MENU_ITEMS_H

typedef struct Menu Menu;

void menu_build_main(Menu *m);
void menu_build_placeholder(Menu *m,const char *title);
void menu_build_from_signal(Menu *m, const char *sig);

#endif // CONQUEST_GUI_MENU_ITEMS_H