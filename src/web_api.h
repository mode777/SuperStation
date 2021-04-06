#ifndef WEB_API_H
#define WEB_API_H

#include <stdbool.h>

#include "sst_common.h"
#include "error.h"

sst_ErrorCode sst_web_init(sst_State* state);

void sst_web_vm_set_paused(bool isPaused);

#endif