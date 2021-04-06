#ifndef ERROR_H
#define ERROR_H

typedef enum {
  sst_NoError,
  sst_Error
} sst_ErrorCode;

const char* sst_error_get();
void sst_error_add(const char* error);
void sst_error_clear();

#define SST_TRY_CALL(CALL) { sst_ErrorCode e = CALL; if(e != sst_NoError){ return e; } }
#define SST_RETURN(EXP) EXP; return sst_NoError  
#define SST_RETURN_ERROR(ERR) sst_error_add(ERR); return sst_Error  
#define SST_CALL_TERM(CALL) { sst_ErrorCode e = CALL; if(e != sst_NoError){ puts("Application Terminated"); puts(sst_error_get()); exit(EXIT_FAILURE); } }
#define SST_TERM() { puts("Application Terminated"); puts(sst_error_get()); exit(EXIT_FAILURE); }

#endif