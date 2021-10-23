#pragma once

#include "lua_sol_types.h"

void lua_light__register(sol::state_view & lua, sol::usertype<tm_entity_t> & entity_type);
