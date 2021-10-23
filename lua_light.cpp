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

void lua_light__set_color(tm_entity_t* entity, float r, float g, float b)
{
    tm_entity_context_o * ctx = tm_lua_global_api.ctx;

    const tm_component_type_t light_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__LIGHT_COMPONENT);
    if (!light_component.index)
    	return;

    tm_light_component_t *lc = (tm_light_component_t*)tm_entity_api->get_component(ctx, *entity, light_component);
    lc->color_rgb = { r, g, b };
}
