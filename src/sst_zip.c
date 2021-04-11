#include <stdbool.h>
#include <stdio.h>

#include <zip.h>

#include "sst_zip.h"

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

bool sst_zip_isArchive(const char* zipPath){
  struct zip_t *zip = zip_open(zipPath, 0, 'r');
  if(zip == NULL) return false;
  zip_close(zip);
  return true;
}

sst_ErrorCode sst_zip_readfile(const char* zipPath, const char* path, const char** out_str, size_t* out_size){
  #ifdef EMSCRIPTEN
  do_fetch(zipPath);
  #endif 
  
  void *buf = NULL;
  size_t bufsize;

  struct zip_t *zip = zip_open(zipPath, 0, 'r');
  if(zip == NULL) {
    SST_RETURN_ERROR("Zip-File not found");
  }

  if(zip_entry_open(zip, path) < 0) {
    zip_close(zip);
    SST_RETURN_ERROR("ZIP entry not found");
  }

  if(zip_entry_read(zip, &buf, &bufsize) < 0){
    zip_entry_close(zip);
    zip_close(zip);
    SST_RETURN_ERROR("Error reading ZIP entry");
  }

  zip_entry_close(zip);
  zip_close(zip);

  buf = realloc(buf, bufsize+1);
  ((char*)buf)[bufsize] = 0;

  SST_RETURN(*out_str = (const char*)buf;if(out_size!=NULL){*out_size = bufsize;});
}