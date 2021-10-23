#pragma once

#include <foundation/api_types.h>

#define TM_LUA_TAB_VT_NAME "tm_lua_tab"
#define TM_LUA_TAB_VT_NAME_HASH TM_STATIC_HASH("tm_lua_tab", 0xbc4e3e47fbf1cdc1ULL)

struct tm_api_registry_api;

void editor__register(struct tm_api_registry_api* reg, bool load);
