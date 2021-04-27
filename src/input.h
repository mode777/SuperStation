#ifndef SST_INPUT_H
#define SST_INPUT_H

#include <SDL.h>

typedef enum {
  sst_Button1 = 0,
  sst_Button2 = 1,
  sst_Button3 = 2,
  sst_Button4 = 3,
  sst_Button5 = 4,
  sst_Button6 = 5,
  sst_ButtonStart = 6,
  sst_ButtonSelect = 7,
  sst_ButtonUp = 8,
  sst_ButtonDown = 9,
  sst_ButtonLeft = 10,
  sst_ButtonRight = 11,
  sst_ButtonMax = 12,
} sst_Button;

typedef struct {
    SDL_Scancode keyboardConfig[sst_ButtonMax];
    int pressed[sst_ButtonMax];
    int delay;
    int repeat;
} sst_InputState;

void sst_input_init(sst_InputState* state);
void sst_input_update(sst_InputState* state);
bool sst_input_isDown(sst_InputState* state, sst_Button button);
bool sst_input_isPressed(sst_InputState* state, sst_Button button);

#endif