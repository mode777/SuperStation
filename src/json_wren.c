#include <jsmn.h>
#include <jsmn_string.h>

#include "json_wren.h"
#include "sst_common.h"
#include "sst_wren.h"
#include "io.h"
#include "error.h"

typedef struct {
  const char* content;
  jsmntok_t* tokens;
  size_t num_tokens;
  int token;
  WrenVM* vm;
  int slot;
} sst_jsonContext;

static void unpack_value(sst_jsonContext* ctx);

static void unpack_array(sst_jsonContext* ctx){
  int size = ctx->tokens[ctx->token].size;
  wrenSetSlotNewList(ctx->vm, ctx->slot);
  wrenEnsureSlots(ctx->vm, ctx->slot+2);
  for (int i = 0; i < size; i++)
  {
    ctx->token++;
    ctx->slot++;
    unpack_value(ctx);
    wrenInsertInList(ctx->vm, ctx->slot-1, -1, ctx->slot);
    ctx->slot--;
  }
}

static void unpack_map(sst_jsonContext* ctx){
  int size = ctx->tokens[ctx->token].size;
  wrenSetSlotNewMap(ctx->vm, ctx->slot);
  wrenEnsureSlots(ctx->vm, ctx->slot+3);
  for (int i = 0; i < size; i++)
  {
    ctx->token++;
    ctx->slot++;
    unpack_value(ctx);
    ctx->token++;
    ctx->slot++;
    unpack_value(ctx);
    wrenSetMapValue(ctx->vm, ctx->slot-2, ctx->slot-1, ctx->slot);
    ctx->slot-=2;
  }
}

static void unpack_value(sst_jsonContext *ctx){
  jsmntok_t t = ctx->tokens[ctx->token];

  switch (t.type)
  {
  case JSMN_UNDEFINED:
    wrenSetSlotNull(ctx->vm, ctx->slot);
    break;
  case JSMN_OBJECT:
    unpack_map(ctx);
    break;
  case JSMN_ARRAY:
    unpack_array(ctx);
    break;
  case JSMN_STRING:
    wrenSetSlotString(ctx->vm, ctx->slot, jsmn_string((char*)ctx->content, &t));
    break;
  case JSMN_PRIMITIVE:
  {
    switch (ctx->content[t.start])
    {
    case 't':
      wrenSetSlotBool(ctx->vm, ctx->slot, true);
      break;
    case 'f':
      wrenSetSlotBool(ctx->vm, ctx->slot, false);
      break;
    case 'n':
      wrenSetSlotNull(ctx->vm, ctx->slot);
      break;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      wrenSetSlotDouble(ctx->vm, ctx->slot, strtod(&ctx->content[t.start], NULL));
      break;
    default:
      wrenSetSlotNull(ctx->vm, ctx->slot);
      break;
    }
  }
  }
}

static void json_load_1(WrenVM* vm){
  sst_State* state = sst_wren_get_state(vm);

  const char* filename = wrenGetSlotString(vm, 1);

  const char* content;
  sst_ErrorCode err = sst_wren_load_resource(vm, filename, (unsigned char**)&content, NULL);

  if(err != sst_NoError){
    wrenError(vm, sst_error_get());
    sst_error_clear();
  } 

  jsmn_parser parser;
  jsmn_init(&parser);

  size_t num_tokens = 512;
  jsmntok_t* tokens = calloc(num_tokens, sizeof(jsmntok_t));
  int tokensParsed;
  size_t contentLength = strlen(content);

  while((tokensParsed = jsmn_parse(&parser, content, contentLength, tokens, num_tokens)) == JSMN_ERROR_NOMEM){
    num_tokens *= 2;
    tokens = realloc(tokens, num_tokens * sizeof(jsmntok_t));
  }

  if(tokensParsed == JSMN_ERROR_INVAL || tokensParsed == JSMN_ERROR_PART){
    free((void*)content);
    free((void*)tokens);
    wrenError(vm, "Invalid JSON input");
    return;
  }

  sst_jsonContext ctx = {
    .content = content,
    .num_tokens = tokensParsed,
    .slot = 0,
    .token = 0,
    .tokens = tokens,
    .vm = vm
  };

  unpack_value(&ctx);

  free((void*)content);
  free((void*)tokens);
} 

sst_ErrorCode sst_json_wren_register(WrenVM* vm){
  sst_wren_register_method(vm, "json.JSON.load(_)", json_load_1);
  return sst_NoError;
}