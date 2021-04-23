#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <wren.h>
// CAUTION: Do this only once
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include "sst_common.h"
#include "error.h"
#include "io.h"
#include "sst_zip.h"
#include "sst_wren.h"
#include "gfx_wren.h"
#include "json_wren.h"

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

typedef struct {
  const char* name;
  const char* source;
  const char* filename;
  WrenInterpretResult result;
} sst_WrenInternalModule;

static sst_WrenInternalModule InternalModules[3] = {
  { .name = "__host__", .source = NULL, .filename = "wren/bootstrap.wren" },
  { .name = "gfx", .source = NULL, .filename = "wren/gfx.wren" },
  { .name = "json", .source = NULL, .filename = "wren/json.wren" }
};

sst_ErrorCode sst_wren_load_resource(WrenVM* vm, const char* path, unsigned char** out_data, size_t* out_size){
  sst_State* state = sst_wren_get_state(vm);

  if(state->isZip){
    printf("[DEBUG] Try load resource %s from %s\n", path, state->root);
    SST_TRY_CALL(sst_zip_readfile(state->root, path, out_data, out_size));
  } else {
    const char* localPath = sst_io_joinPath(2, state->root, path);
    printf("[DEBUG] Try load resource file %s\n", localPath);
    sst_ErrorCode error = sst_io_readfile(localPath, out_data, out_size);
    free((void*)localPath);
    return error;
  }
  SST_RETURN();
}

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

static WrenLoadModuleResult load_module_fn(WrenVM* vm, const char* name){
  sst_WrenData* userData = wrenGetUserData(vm);
  
  WrenLoadModuleResult result = {0};

  if(strcmp(name, "random") == 0 || strcmp(name, "meta") == 0) return result;

  const char* path = sst_io_joinPath(2, name, ".wren");
  unsigned char* content = NULL;

  sst_ErrorCode error = sst_wren_load_resource(vm, path, &content, NULL);
  free((void*)path);

  if(error != sst_NoError){
    puts(sst_error_get());
    sst_error_clear();
  }

  result.source = content;
  return result;
}

sst_ErrorCode sst_wren_dispose_vm(WrenVM* vm){
  if(vm == NULL) SST_RETURN();

  sst_WrenData* ud = wrenGetUserData(vm);

  wrenReleaseHandle(vm, ud->callHandle);
  wrenReleaseHandle(vm, ud->mainHandle);
  wrenReleaseHandle(vm, ud->mainHandle);
  wrenReleaseHandle(vm, ud->replHandle);
  wrenReleaseHandle(vm, ud->updateHandle);
  
  if(ud->foreignClasses != NULL) free((void*)ud->foreignClasses);
  if(ud->foreignMethods != NULL) free((void*)ud->foreignMethods);

  free((void*)ud); 
  wrenFreeVM(vm);
 
  SST_RETURN();
}



static sst_ErrorCode load_internal_modules(WrenVM* vm){
  for (size_t i = 0; i < 3; i++)
  {
    if(InternalModules[i].source == NULL) {
      SST_TRY_CALL(sst_io_readfile(InternalModules[i].filename, (unsigned char**)&InternalModules[i].source, NULL));
    }
    
    WrenInterpretResult fiberResult = wrenInterpret(vm, InternalModules[i].name, InternalModules[i].source);
    if(fiberResult != WREN_RESULT_SUCCESS) { 
      SST_RETURN_ERROR("Script reported errors");
    }
  }
  
  SST_RETURN();
}

sst_ErrorCode sst_wren_new_vm(sst_State* state, WrenVM** out_vm){
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

  sst_gfx_wren_register(vm);
  sst_json_wren_register(vm);

  SST_TRY_CALL(load_internal_modules(vm));

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