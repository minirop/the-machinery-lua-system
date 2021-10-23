#ifdef __cplusplus
extern "C" {
#endif

extern struct tm_transform_component_api* tm_transform_component_api;
extern struct tm_entity_api* tm_entity_api;
extern struct tm_lua_global_api tm_lua_global_api;

#include <plugins/entity/transform_component.h>
#include <plugins/entity/entity.h>
#include <foundation/math.inl>

#include "lua_api.h"

#ifdef __cplusplus
}
#endif

#include "lua_transform.h"

static void lua_transform__set_position(tm_entity_t* entity, float x, float y, float z);
static void lua_transform__set_rotation(tm_entity_t* entity, float x, float y, float z);
static void lua_transform__set_scale(tm_entity_t* entity, float x, float y, float z);
static lua_vec3 lua_transform__get_position(tm_entity_t* entity);
static lua_vec3 lua_transform__get_rotation(tm_entity_t* entity);
static lua_vec3 lua_transform__get_scale(tm_entity_t* entity);
static void lua_transform__set_parent(tm_entity_t* entity, tm_entity_t* parent);

void lua_transform__register(sol::usertype<tm_entity_t> & entity_type)
{
    entity_type["set_position"] = lua_transform__set_position;
    entity_type["set_rotation"] = lua_transform__set_rotation;
    entity_type["set_scale"]    = lua_transform__set_scale;
    entity_type["get_position"] = lua_transform__get_position;
    entity_type["get_rotation"] = lua_transform__get_rotation;
    entity_type["get_scale"]    = lua_transform__get_scale;
    entity_type["set_parent"]   = lua_transform__set_parent;
}

#define LUA_TRANSFORM_COMPONENT(ret) \
    tm_entity_context_o * ctx = tm_lua_global_api.ctx; \
\
    const tm_component_type_t transform_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT); \
    if (!transform_component.index) \
        return ret; \
\
    tm_transform_component_manager_o *manager = (tm_transform_component_manager_o*)tm_entity_api->component_manager(ctx, transform_component);

void lua_transform__set_position(tm_entity_t* entity, float x, float y, float z)
{
    LUA_TRANSFORM_COMPONENT()
    tm_transform_component_api->set_position(manager, *entity, (tm_vec3_t){x, y, z});
}

void lua_transform__set_rotation(tm_entity_t* entity, float x, float y, float z)
{
    LUA_TRANSFORM_COMPONENT()

    tm_vec4_t rot = tm_quaternion_from_euler((tm_vec3_t){ x, y, z });
    tm_transform_component_api->set_rotation(manager, *entity, rot);
}

void lua_transform__set_scale(tm_entity_t* entity, float x, float y, float z)
{
    LUA_TRANSFORM_COMPONENT()

    tm_transform_component_api->set_scale(manager, *entity, (tm_vec3_t){x, y, z});
}

lua_vec3 lua_transform__get_position(tm_entity_t* entity)
{
    LUA_TRANSFORM_COMPONENT(std::make_tuple(0, 0, 0))

    tm_vec3_t vec = tm_get_position(manager, *entity);
    return std::make_tuple(vec.x, vec.y, vec.z);
}

lua_vec3 lua_transform__get_rotation(tm_entity_t* entity)
{
    LUA_TRANSFORM_COMPONENT(std::make_tuple(0, 0, 0))

    tm_vec4_t quat = tm_get_rotation(manager, *entity);
    tm_vec3_t vec = tm_quaternion_to_euler(quat);
    return std::make_tuple(vec.x, vec.y, vec.z);
}

lua_vec3 lua_transform__get_scale(tm_entity_t* entity)
{
    LUA_TRANSFORM_COMPONENT(std::make_tuple(1, 1, 1))

    tm_vec3_t vec = tm_get_scale(manager, *entity);
    return std::make_tuple(vec.x, vec.y, vec.z);
}

void lua_transform__set_parent(tm_entity_t* entity, tm_entity_t* parent)
{
    LUA_TRANSFORM_COMPONENT()

    tm_transform_component_api->set_parent(manager, *entity, *parent);
}
