#ifdef __cplusplus
extern "C" {
#endif

extern struct tm_entity_api* tm_entity_api;
extern struct tm_lua_global_api tm_lua_global_api;

#include <plugins/default_render_pipe/light_component.h>
#include <plugins/entity/entity.h>
#include "lua_api.h"

#ifdef __cplusplus
}
#endif

#include "lua_light.h"

static void lua_light__set_type(tm_entity_t* entity, int type);
static int lua_light__get_type(tm_entity_t* entity);
static void lua_light__set_unit_type(tm_entity_t* entity, int type);
static int lua_light__get_unit_type(tm_entity_t* entity);
static void lua_light__set_color_type(tm_entity_t* entity, int type);
static int lua_light__get_color_type(tm_entity_t* entity);
static void lua_light__set_color(tm_entity_t* entity, float r, float g, float b);
static lua_vec3 lua_light__get_color(tm_entity_t* entity);
static void lua_light__set_enabled(tm_entity_t* entity, bool enabled);
static bool lua_light__is_enabled(tm_entity_t* entity);

void lua_light__register(sol::state_view & lua, sol::usertype<tm_entity_t> & entity_type)
{
    entity_type["set_light_type"] = lua_light__set_type;
    entity_type["get_light_type"] = lua_light__get_type;
    entity_type["set_light_unit_type"] = lua_light__set_unit_type;
    entity_type["get_light_unit_type"] = lua_light__get_unit_type;
    entity_type["set_light_color_type"] = lua_light__set_color_type;
    entity_type["get_light_color_type"] = lua_light__get_color_type;
    entity_type["set_light_color"] = lua_light__set_color;
    entity_type["get_light_color"] = lua_light__get_color;
    entity_type["set_light_enabled"] = lua_light__set_enabled;
    entity_type["is_light_enabled"] = lua_light__is_enabled;

    lua["light"] = lua.create_table_with(
        "point", TM_LIGHT_TYPE_POINT,
        "spot", TM_LIGHT_TYPE_SPOT,
        "directional", TM_LIGHT_TYPE_DIRECTIONAL,
        "ibl", TM_LIGHT_TYPE_IBL,
        "sphere", TM_LIGHT_TYPE_AREA_SPHERE,
        "rectangle", TM_LIGHT_TYPE_AREA_RECT,
        "disk", TM_LIGHT_TYPE_AREA_DISK,

        "candela", TM_LIGHT_COMPONENT__UNIT_TYPE__CANDELA,
        "lumen", TM_LIGHT_COMPONENT__UNIT_TYPE__LUMEN,
        "lux", TM_LIGHT_COMPONENT__UNIT_TYPE__LUX,
        "nits", TM_LIGHT_COMPONENT__UNIT_TYPE__NITS,
        "ev100", TM_LIGHT_COMPONENT__UNIT_TYPE__EV100,

        "rgb", TM_LIGHT_COMPONENT__COLOR_TYPE__RGB,
        "kelvin", TM_LIGHT_COMPONENT__COLOR_TYPE__KELVIN
        );
}

#define LUA_LIGHT_COMPONENT(ret) \
    tm_entity_context_o * ctx = tm_lua_global_api.ctx; \
\
    const tm_component_type_t light_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__LIGHT_COMPONENT); \
    if (!light_component.index) \
        return ret; \
\
    tm_light_component_t *lc = (tm_light_component_t*)tm_entity_api->get_component(ctx, *entity, light_component);

void lua_light__set_type(tm_entity_t* entity, int type)
{
    LUA_LIGHT_COMPONENT()
    lc->type = (tm_light_component_type)type;
}

int lua_light__get_type(tm_entity_t* entity)
{
    LUA_LIGHT_COMPONENT(TM_LIGHT_TYPE_POINT)
    return lc->type;
}

void lua_light__set_unit_type(tm_entity_t* entity, int type)
{
    LUA_LIGHT_COMPONENT()
    lc->unit_type = (tm_light_component_unit_type)type;
}

int lua_light__get_unit_type(tm_entity_t* entity)
{
    LUA_LIGHT_COMPONENT(TM_LIGHT_COMPONENT__UNIT_TYPE__CANDELA)
    return lc->unit_type;
}

void lua_light__set_color_type(tm_entity_t* entity, int type)
{
    LUA_LIGHT_COMPONENT()
    lc->color_type = (tm_light_component_color_type)type;
}

int lua_light__get_color_type(tm_entity_t* entity)
{
    LUA_LIGHT_COMPONENT(TM_LIGHT_COMPONENT__COLOR_TYPE__RGB)
    return lc->color_type;
}

void lua_light__set_color(tm_entity_t* entity, float r, float g, float b)
{
    LUA_LIGHT_COMPONENT()
    lc->color_rgb = { r, g, b };
}

lua_vec3 lua_light__get_color(tm_entity_t* entity)
{
    LUA_LIGHT_COMPONENT(std::make_tuple(0, 0, 0))
    tm_vec3_t vec = lc->color_rgb;
    return std::make_tuple(vec.x, vec.y, vec.z);
}

void lua_light__set_enabled(tm_entity_t* entity, bool enabled)
{
    LUA_LIGHT_COMPONENT()
    lc->enabled = enabled;
}

bool lua_light__is_enabled(tm_entity_t* entity)
{
    LUA_LIGHT_COMPONENT(false)
    return lc->enabled;
}
