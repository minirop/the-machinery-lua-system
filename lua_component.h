#pragma once

#include <foundation/api_types.h>

#define TM_TT_TYPE__LUA_COMPONENT "tm_lua_component"
#define TM_TT_TYPE_HASH__LUA_COMPONENT TM_STATIC_HASH("tm_lua_component", 0x8d4d19e6dd2102dULL)

#define TM_TT_TYPE__LUA_COMPONENT_ENGINE "tm_lua_component_engine"
#define TM_TT_TYPE_HASH__LUA_COMPONENT_ENGINE TM_STATIC_HASH("tm_lua_component_engine", 0x9ac47d2d4bc7539fULL)

struct tm_the_truth_o;
struct tm_entity_context_o;

void component__truth__create_types(struct tm_the_truth_o* tt);
void component__create(struct tm_entity_context_o* ctx);
void component__register_engine(struct tm_entity_context_o* ctx);
