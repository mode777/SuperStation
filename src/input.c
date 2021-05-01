#include <stdbool.h>

#include <SDL.h>

#include "input.h"



void sst_input_init(sst_InputState* state){
    state->keyboardConfig[sst_Button1] = SDL_SCANCODE_A;
    state->keyboardConfig[sst_Button2] = SDL_SCANCODE_S;
    state->keyboardConfig[sst_Button3] = SDL_SCANCODE_D;
    state->keyboardConfig[sst_Button4] = SDL_SCANCODE_Y;
    state->keyboardConfig[sst_Button5] = SDL_SCANCODE_X;
    state->keyboardConfig[sst_Button6] = SDL_SCANCODE_C;
    state->keyboardConfig[sst_ButtonStart] = SDL_SCANCODE_RETURN;
    state->keyboardConfig[sst_ButtonSelect] = SDL_SCANCODE_SPACE;
    state->keyboardConfig[sst_ButtonUp] = SDL_SCANCODE_UP;
    state->keyboardConfig[sst_ButtonDown] = SDL_SCANCODE_DOWN;
    state->keyboardConfig[sst_ButtonLeft] = SDL_SCANCODE_LEFT;
    state->keyboardConfig[sst_ButtonRight] = SDL_SCANCODE_RIGHT;

    for (size_t i = 0; i < sst_ButtonMax; i++)
    {
        state->pressed[i] = 0;
    }

    state->delay = 16;
    state->repeat = 3;
}

void sst_input_update(sst_InputState* state){
    const Uint8* sdlState = SDL_GetKeyboardState(NULL);
    for (size_t i = 0; i < sst_ButtonMax; i++)
    {
        Uint8 pressed = sdlState[state->keyboardConfig[i]];
        if(pressed){
            state->pressed[i]++;
        } else {
            state->pressed[i] = 0;
        }
    }
}

bool sst_input_isDown(sst_InputState* state, sst_Button button){
    return state->pressed[button] > 0;
}

bool sst_input_isPressed(sst_InputState* state, sst_Button button){
    int pressed = state->pressed[button];
    if(pressed == 1) return true;

    int normalized = pressed - state->delay;
    if(normalized >= 0){
        return (normalized % state->repeat) == 0;
    } else {
        return false;
    }
}