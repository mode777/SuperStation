#include "input_wren.h"
#include "input.h"

static void button_isDown_1(WrenVM* vm){
    sst_InputState* state = &sst_wren_get_state(vm)->input;
    unsigned int button = wrenGetSlotDouble(vm, 1);
    if(button >= sst_ButtonMax){
        wrenError(vm, "Invalid button");
        return;
    }
    bool isDown = sst_input_isDown(state, button);
    wrenSetSlotBool(vm, 0, isDown);
}

static void button_isPressed_1(WrenVM* vm){
    sst_InputState* state = &sst_wren_get_state(vm)->input;
    unsigned int button = wrenGetSlotDouble(vm, 1);
    if(button >= sst_ButtonMax){
        wrenError(vm, "Invalid button");
        return;
    }
    bool isDown = sst_input_isPressed(state, button);
    wrenSetSlotBool(vm, 0, isDown);
}

sst_ErrorCode sst_input_wren_register(WrenVM* vm){
  sst_wren_register_method(vm, "input.Button.isDown(_)", button_isDown_1);
  sst_wren_register_method(vm, "input.Button.isPressed(_)", button_isPressed_1);
  return sst_NoError;
}