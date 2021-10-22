#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tm_entity_context_o tm_entity_context_o;
struct tm_lua_global_api {
    tm_entity_context_o * ctx;
    int running;
};

#ifdef __cplusplus
}
#endif
