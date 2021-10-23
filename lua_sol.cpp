#include "lua_sol.h"

#include <sol/sol.hpp>

#include "lua_transform.h"
#include "lua_light.h"

void lua_sol__register(lua_State* L)
{
	sol::state_view lua(L);

	auto entity_type = lua.new_usertype<tm_entity_t>("tm_entity_t", sol::no_constructor);
	entity_type["set_position"] = lua_transform__set_position;
	entity_type["set_rotation"] = lua_transform__set_rotation;
	entity_type["set_scale"]    = lua_transform__set_scale;
	entity_type["get_position"] = lua_transform__get_position;
	entity_type["get_rotation"] = lua_transform__get_rotation;
	entity_type["get_scale"]    = lua_transform__get_scale;
	entity_type["set_parent"]   = lua_transform__set_parent;

	entity_type["set_light_color"] = lua_light__set_color;
}

void lua_sol__set_entity(lua_State* L, tm_entity_t entity)
{
	sol::state_view lua(L);
	lua["entity"] = entity;
}
