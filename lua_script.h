#pragma once

#include <foundation/api_types.h>

#define TM_TT_TYPE__LUA_SCRIPT "tm_lua_script"
#define TM_TT_TYPE_HASH__LUA_SCRIPT TM_STATIC_HASH("tm_lua_script", 0xabe5258857d03dd7ULL)

enum
{
    TM_TT_PROP__LUA_SCRIPT__TEXT,
};

struct tm_the_truth_o;

void asset__truth__create_types(struct tm_the_truth_o* tt);
