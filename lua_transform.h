#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void lua_transform__set_position(tm_entity_t* entity, float x, float y, float z);
void lua_transform__set_rotation(tm_entity_t* entity, float x, float y, float z);

#ifdef __cplusplus
}
#endif
