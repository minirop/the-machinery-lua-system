#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>

#include <plugins/entity/entity.h>

void lua_sol__register(lua_State* L);
void lua_sol__set_entity(lua_State* L, tm_entity_t entity);
tm_entity_t lua_sol__get_entity(lua_State* L);

#ifdef __cplusplus
}
#endif
