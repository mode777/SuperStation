#include <SDL.h>

#include "string.h"
#include "io.h"

//#define EMSCRIPTEN

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>

EM_JS(void, do_fetch, (const char* path), {
  Asyncify.handleSleep(wakeUp => {
    var pathStr = UTF8ToString(path);
    sst.fetch(pathStr, function(){
        wakeUp();
    })
  });
});

#endif

sst_ErrorCode sst_io_readfile(const char* path, const char** out_str, size_t* out_size) {
  #ifdef EMSCRIPTEN
  do_fetch(path);
  #endif  
  
  size_t size;
  const char* data = (const char*)SDL_LoadFile(path, &size);
  if(data == NULL){
    //SDL_GetErrorMsg()
    sst_error_add("File not found:");
    sst_error_add(path);
    return sst_Error;
  }
  SST_RETURN(*out_str = data; if(out_size!=NULL){*out_size = size;});
}

// #else

// #include <stdio.h>
// #include <emscripten/fetch.h>

// sst_ErrorCode sst_io_readfile(const char* path, const char** out_str, size_t* out_size) {
//   char* wpath = calloc(strlen(path) + 2,sizeof(char));
//   strcat(wpath, "/");
//   strcat(wpath, path);

//   emscripten_fetch_attr_t attr;
//   emscripten_fetch_attr_init(&attr);
//   strcpy(attr.requestMethod, "GET");
//   attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
//   emscripten_fetch_t *fetch = emscripten_fetch(&attr, wpath); // Blocks here until the operation is complete.

//   sst_ErrorCode error;

//   printf("Status %i\n", fetch->status);
  
//   if (fetch->status == 200) {
//     const char* str = calloc(fetch->numBytes, sizeof(char));
//     memcpy((char*)str, fetch->data, fetch->numBytes);
//     *out_str = str;
//     *out_size = fetch->numBytes;
//     error = sst_NoError;
//   } else {
//     sst_error_add("File not found:");
//     sst_error_add(path);
//     error = sst_Error;
//   }

//   free((void*)wpath);
//   emscripten_fetch_close(fetch);
//   return error;
// }

// #endif