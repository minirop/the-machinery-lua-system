extern struct tm_transform_component_api* tm_transform_component_api;
extern struct tm_entity_api* tm_entity_api;
extern struct tm_lua_global_api tm_lua_global_api;

#include <plugins/entity/transform_component.h>
#include <plugins/entity/entity.h>
#include <foundation/math.inl>

#include "lua_api.h"
#include "lua_transform.h"

void lua_transform__set_position(tm_entity_t* entity, float x, float y, float z)
{
    tm_entity_context_o * ctx = tm_lua_global_api.ctx;

    const tm_component_type_t oc = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT);
    tm_transform_component_manager_o *manager = (tm_transform_component_manager_o*)tm_entity_api->component_manager(ctx, oc);

    tm_transform_component_api->set_position(manager, *entity, (tm_vec3_t){x, y, z});
}

void lua_transform__set_rotation(tm_entity_t* entity, float x, float y, float z)
{
    tm_entity_context_o * ctx = tm_lua_global_api.ctx;

    const tm_component_type_t oc = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT);
    tm_transform_component_manager_o *manager = (tm_transform_component_manager_o*)tm_entity_api->component_manager(ctx, oc);

    tm_vec4_t rot = tm_quaternion_from_euler((tm_vec3_t){ x, y, z });
    tm_transform_component_api->set_rotation(manager, *entity, rot);
}
