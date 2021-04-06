#include <string.h>
#include <stdio.h>

#include "error.h"

static char error_str[16*1024];

const char* sst_error_get(){
  return (const char*)error_str;
}

void sst_error_add(const char* error){
  strcat(error_str, "\n");
  strcat(error_str, error);
}

void sst_error_clear(){
  error_str[0] = 0;
}



