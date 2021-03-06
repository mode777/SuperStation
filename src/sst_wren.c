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
#include "input_wren.h"
#include "io_wren.h"
#include "resource.h"

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
  WrenInterpretResult result;
} sst_WrenInternalModule;

static sst_WrenInternalModule InternalModules[] = {
  { .name = "__host__",   .source = NULL },
  { .name = "gfx",        .source = NULL },
  { .name = "json",       .source = NULL },
  { .name = "input",      .source = NULL },
  { .name = "io",         .source = NULL }
};

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

sst_ErrorCode sst_wren_load_resource(WrenVM* vm, const char* path, unsigned char** out_data, size_t* out_size){
  sst_State* state = sst_wren_get_state(vm);
  if(startsWith("res://", path)){
    sst_Resource resId = atoi(path + strlen("res://"));
    const char* data = sst_resource_get(resId);
    size_t len = strlen(data) + 1; 
    *out_data = calloc(sizeof(char), len);
    if(out_size != NULL) *out_size = len;
    memcpy(*out_data, data, len);
  }
  else if(state->game.isZip){
    printf("[DEBUG] Try load resource %s from %s\n", path, state->game.root);
    SST_TRY_CALL(sst_zip_readfile(state->game.root, path, out_data, out_size));
  } else {
    const char* localPath = sst_io_joinPath(2, state->game.root, path);
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

  result.source = (const char*)content;
  return result;
}

sst_ErrorCode sst_wren_dispose_vm(WrenVM* vm){
  if(vm == NULL) {
    return sst_NoError;
    //SST_RETURN();
  }

  sst_WrenData* ud = wrenGetUserData(vm);

  wrenReleaseHandle(vm, ud->callHandle);
  wrenReleaseHandle(vm, ud->mainHandle);
  wrenReleaseHandle(vm, ud->replHandle);
  wrenReleaseHandle(vm, ud->updateHandle);
  
  if(ud->foreignClasses != NULL) stbds_hmfree(ud->foreignClasses);
  if(ud->foreignMethods != NULL) stbds_hmfree(ud->foreignMethods);

  free((void*)ud); 
  wrenFreeVM(vm);
 
  SST_RETURN();
}

static sst_ErrorCode load_internal_modules(WrenVM* vm){
  InternalModules[0].source = sst_resource_get(sst_Res_Script_Bootstrap);
  InternalModules[1].source = sst_resource_get(sst_Res_Script_Gfx);
  InternalModules[2].source = sst_resource_get(sst_Res_Script_Json);
  InternalModules[3].source = sst_resource_get(sst_Res_Script_Input);
  InternalModules[4].source = sst_resource_get(sst_Res_Script_Io);
  
  size_t length = sizeof(InternalModules)/sizeof(sst_WrenInternalModule);
  for (size_t i = 0; i < length; i++)
  {    
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
  sst_input_wren_register(vm);
  sst_io_wren_register(vm);

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