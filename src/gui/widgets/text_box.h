#ifndef WIDGET_TEXT_BOX
#define WIDGET_TEXT_BOX

typedef struct Textbox {
  SDL_Rect box;
  SDL_Texture *label; /* text rendered into texture */
  SDL_Color base_background_color; /* color of the button */
} Textbox;


#endif // WIDGET_TEXT_BOX