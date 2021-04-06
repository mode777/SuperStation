#include <wren.h>
#include <string.h>
#include <stdio.h>

// CAUTION: Do this only once
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include "sst_common.h"
#include "error.h"
#include "io.h"
#include "sst_wren.h"

typedef struct {
  char * key;
  WrenForeignMethodFn value;
} sst_WrenBinding;

typedef struct {
  char* key;
  WrenForeignClassMethods value;
} sst_WrenClassBinding;

typedef struct {
  sst_State* state;
  WrenHandle* updateHandle;
  WrenHandle* mainHandle;
  WrenHandle* replHandle;
  WrenHandle* callHandle;
  sst_WrenBinding* foreignMethods;
  sst_WrenClassBinding* foreignClasses;
  bool isPaused;
} sst_WrenData;

static char* get_method_name(const char* module, 
  const char* className, 
  bool isStatic, 
  const char* signature)
{
  int length = strlen(module) + strlen(className) + strlen(signature) + 3;
  char *str = (char*)calloc(length, sizeof(char));
  sprintf(str, "%s.%s.%s", module, className, signature);
  return str;
}

static char* get_class_name(const char* module, 
  const char* className)
{
  int length = strlen(module) + strlen(className) + 2;
  char *str = (char*)calloc(length, sizeof(char));
  sprintf(str, "%s.%s", module, className);
  return str;
}

static WrenForeignMethodFn bind_method_fn( 
  WrenVM* vm, 
  const char* module, 
  const char* className, 
  bool isStatic, 
  const char* signature) 
{
  sst_WrenData* userData = wrenGetUserData(vm);

  char* fullName = get_method_name(module, className, isStatic, signature);
  WrenForeignMethodFn func = shget(userData->foreignMethods, fullName);
  free(fullName);
  return func;
}

WrenForeignClassMethods bind_class_fn( 
  WrenVM* vm, 
  const char* module, 
  const char* className)
{
  sst_WrenData* userData = wrenGetUserData(vm);

  char* fullName = get_class_name(module, className);
  
  int index = shgeti(userData->foreignClasses, fullName);
  free(fullName);
  if(index == -1){
    WrenForeignClassMethods wfcm ={0};
    return wfcm;  
  }
  else {
    return userData->foreignClasses[index].value;
  }
}

static void error_fn(WrenVM *vm, WrenErrorType type, const char *module, int line, const char *message)
{  
  printf("Wren-Error in module '%s' line %i: %s\n", module, line, message);
}

static void write_fn(WrenVM *vm, const char *text)
{
  printf("%s", text);
}

static const char* load_script(const char* prefix, const char* module){
  char* path = calloc(1, strlen(prefix) + strlen(module) + 1 + 5 + 1);
  strcpy(path, prefix);
  strcat(path, module);
  strcat(path, ".wren");

  size_t size;
  const char* content = NULL;
  printf("[DEBUG] Try load script %s\n", path);
  sst_ErrorCode error = sst_io_readfile((const char*)path, &content, &size);

  free((void*)path);
  return content;
}

static WrenLoadModuleResult load_module_fn(WrenVM* vm, const char* name){
  sst_WrenData* userData = wrenGetUserData(vm);
  
  WrenLoadModuleResult result = {0};

  if(strcmp(name, "random") == 0 || strcmp(name, "meta") == 0) return result;

  const char* content = load_script(userData->state->root, name);
  // TODO: remove this line for production build
  sst_error_clear();
  if(content == NULL) content = load_script("wren/", name);
  if(content == NULL) return result;

  result.source = content;
  return result;
}

sst_ErrorCode sst_wren_init(sst_State* state, WrenVM** out_vm){
  WrenConfiguration config;
  wrenInitConfiguration(&config);

  sst_WrenData* userData = calloc(1, sizeof(sst_WrenData));
  
  userData->state = state;

  config.errorFn = error_fn;
  config.writeFn = write_fn;
  config.loadModuleFn = load_module_fn;
  config.bindForeignMethodFn = bind_method_fn;
  config.bindForeignClassFn = bind_class_fn;
  config.userData = userData;

  WrenVM* vm = wrenNewVM(&config);

  const char* bootstrapSource;
  SST_TRY_CALL(sst_io_readfile("wren/bootstrap.wren", &bootstrapSource, NULL));

  WrenInterpretResult fiberResult = wrenInterpret(vm, "__host__", bootstrapSource);
  if(fiberResult != WREN_RESULT_SUCCESS){
    wrenFreeVM(vm);
    return sst_Error;
  }
  wrenEnsureSlots(vm, 1);
  wrenGetVariable(vm, "__host__", "update__", 0);
  userData->updateHandle = wrenGetSlotHandle(vm, 0);
  wrenGetVariable(vm, "__host__", "main__", 0);
  userData->mainHandle = wrenGetSlotHandle(vm, 0);
  wrenGetVariable(vm, "__host__", "repl__", 0);
  userData->replHandle = wrenGetSlotHandle(vm, 0);

  userData->callHandle = wrenMakeCallHandle(vm, "call(_)");

  SST_RETURN(*out_vm = vm);
}

sst_ErrorCode sst_wren_update(WrenVM* vm, bool* out_running){
  sst_WrenData* userData = wrenGetUserData(vm);
  if(userData->isPaused) return sst_NoError;
  wrenEnsureSlots(vm, 2);
  wrenSetSlotHandle(vm, 0, userData->updateHandle);
  wrenSetSlotHandle(vm, 1, userData->mainHandle);
  WrenInterpretResult fiberResult = wrenCall(vm, userData->callHandle);
  if(fiberResult != WREN_RESULT_SUCCESS){
    wrenFreeVM(vm);
    return sst_Error;
  }
  *out_running = wrenGetSlotBool(vm, 0);
  return sst_NoError;
}

sst_ErrorCode sst_wren_repl(WrenVM* vm, const char* code){
  sst_WrenData* userData = wrenGetUserData(vm);
  wrenEnsureSlots(vm, 2);
  wrenSetSlotHandle(vm, 0, userData->replHandle);
  wrenSetSlotString(vm, 1, code);
  WrenInterpretResult fiberResult = wrenCall(vm, userData->callHandle);
  if(fiberResult != WREN_RESULT_SUCCESS){
    return sst_Error;
  }
  return sst_NoError;
}



sst_ErrorCode sst_wren_register_method(WrenVM* vm, const char* name, WrenForeignMethodFn func){
  sst_WrenData* userData = wrenGetUserData(vm);
  shput(userData->foreignMethods, name, func);
  return sst_NoError;
}

sst_ErrorCode sst_wren_register_class(WrenVM* vm, const char* name, WrenForeignMethodFn allocator, WrenFinalizerFn finalizer){
  sst_WrenData* userData = wrenGetUserData(vm);
  WrenForeignClassMethods methods = {
    allocator = allocator,
    finalizer = finalizer
  };
  shput(userData->foreignClasses, name, methods);
  return sst_NoError;
}

sst_State* sst_wren_get_state(WrenVM* vm){
  sst_WrenData* userData = wrenGetUserData(vm);
  return userData->state;
}

void sst_wren_set_paused(WrenVM* vm, bool isPaused){
  sst_WrenData* userData = wrenGetUserData(vm);
  userData->isPaused = isPaused;
}