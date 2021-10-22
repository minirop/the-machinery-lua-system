#pragma once

#include <tuple>
using lua_vec3 = std::tuple<float, float, float>;

void lua_transform__set_position(tm_entity_t* entity, float x, float y, float z);
void lua_transform__set_rotation(tm_entity_t* entity, float x, float y, float z);
void lua_transform__set_scale(tm_entity_t* entity, float x, float y, float z);

lua_vec3 lua_transform__get_position(tm_entity_t* entity);
lua_vec3 lua_transform__get_rotation(tm_entity_t* entity);
lua_vec3 lua_transform__get_scale(tm_entity_t* entity);

void lua_transform__set_parent(tm_entity_t* entity, tm_entity_t* parent);
