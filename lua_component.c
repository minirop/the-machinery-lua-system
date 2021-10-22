extern struct tm_entity_api* tm_entity_api;
extern struct tm_transform_component_api* tm_transform_component_api;
extern struct tm_temp_allocator_api* tm_temp_allocator_api;
extern struct tm_the_truth_api* tm_the_truth_api;
extern struct tm_localizer_api* tm_localizer_api;
extern struct tm_logger_api* tm_logger_api;

#include <plugins/entity/entity.h>
#include <plugins/entity/transform_component.h>
#include <plugins/editor_views/properties.h>
#include <plugins/the_machinery_shared/component_interfaces/editor_ui_interface.h>

#include <foundation/api_registry.h>
#include <foundation/carray.inl>
#include <foundation/localizer.h>
#include <foundation/math.inl>
#include <foundation/the_truth.h>
#include <foundation/log.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "lua_component.h"
#include "lua_script.h"
#include "lua_sol.h"
#include "lua_api.h"

enum {
    TM_TT_PROP__LUA_COMPONENT__SCRIPT,
};

struct tm_lua_component_t {
    lua_State* L;
};

struct tm_lua_global_api tm_lua_global_api = {
    .ctx = NULL,
};

static const char* component__category(void)
{
    return TM_LOCALIZE("Scripts");
}

static tm_ci_editor_ui_i* editor_aspect = &(tm_ci_editor_ui_i){
    .category = component__category
};

void component__truth__create_types(struct tm_the_truth_o* tt)
{
    tm_the_truth_property_definition_t lua_component_properties[] = {
        [TM_TT_PROP__LUA_COMPONENT__SCRIPT] = { "script", .type = TM_THE_TRUTH_PROPERTY_TYPE_REFERENCE, .type_hash = TM_TT_TYPE_HASH__LUA_SCRIPT },
    };

    const tm_tt_type_t lua_component_type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__LUA_COMPONENT, lua_component_properties, TM_ARRAY_COUNT(lua_component_properties));
    const tm_tt_id_t default_object = tm_the_truth_api->quick_create_object(tt, TM_TT_NO_UNDO_SCOPE, TM_TT_TYPE_HASH__LUA_COMPONENT, -1);
    tm_the_truth_api->set_default_object(tt, lua_component_type, default_object);

    tm_the_truth_api->set_aspect(tt, lua_component_type, TM_CI_EDITOR_UI, editor_aspect);
    tm_the_truth_api->set_property_aspect(tt, lua_component_type, TM_TT_PROP__LUA_COMPONENT__SCRIPT, TM_TT_PROP_ASPECT__PROPERTIES__ASSET_PICKER, TM_TT_TYPE__LUA_SCRIPT);
}

static bool component__load_asset(tm_component_manager_o* man, struct tm_entity_commands_o *commands, tm_entity_t e, void* c_vp, const tm_the_truth_o* tt, tm_tt_id_t asset)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_sol__register(L);
    lua_sol__set_entity(L, e);

    const tm_the_truth_object_o* asset_r = tm_tt_read(tt, asset);
    tm_tt_id_t id = tm_the_truth_api->get_reference(tt, asset_r, TM_TT_PROP__LUA_COMPONENT__SCRIPT);
    if (id.u64)
    {
        tm_tt_buffer_t buffer = tm_the_truth_api->get_buffer(tt, tm_tt_read(tt, id), TM_TT_PROP__LUA_SCRIPT__DATA);
        luaL_loadbuffer(L, buffer.data, buffer.size, "<asset name>");
        if (lua_isfunction(L, -1))
        {
            if (lua_pcall(L, 0, 0, 0))
            {
                tm_logger_api->printf(TM_LOG_TYPE_ERROR, "Could not create lua component: %s\n", lua_tostring(L, -1));
                lua_pop(L, 1);
                lua_close(L);

                return false;
            }
        }
        else
        {
            tm_logger_api->printf(TM_LOG_TYPE_ERROR, "Could not load lua script: %s\n", lua_tostring(L, -1));
            return false;
        }
    }

    struct tm_lua_component_t* c = c_vp;
    c->L = L;

    return true;
}

static void component__remove(tm_component_manager_o *manager, struct tm_entity_commands_o *commands, tm_entity_t e, void *data)
{
    struct tm_lua_component_t* c = data;
    lua_close(c->L);
}

void component__create(struct tm_entity_context_o* ctx)
{
    tm_component_i component = {
        .name = TM_TT_TYPE__LUA_COMPONENT,
        .bytes = sizeof(struct tm_lua_component_t),
        .load_asset = component__load_asset,
        .remove = component__remove,
    };

    tm_entity_api->register_component(ctx, &component);
}

static void engine_update__lua_component(tm_engine_o* inst, tm_engine_update_set_t* data, struct tm_entity_commands_o *commands)
{
    for (const tm_entity_blackboard_value_t *bb = data->blackboard_start; bb != data->blackboard_end; ++bb)
    {
        if (TM_STRHASH_EQUAL(bb->id, TM_ENTITY_BB__EDITOR))
        {
            return;
        }
    }

    tm_lua_global_api.ctx = (tm_entity_context_o *)inst;

    TM_INIT_TEMP_ALLOCATOR(ta);

    for (tm_engine_update_array_t *a = data->arrays; a < data->arrays + data->num_arrays; ++a)
    {
        struct tm_lua_component_t *lua_component = a->components[0];

        for (uint32_t i = 0; i < a->n; ++i)
        {
            lua_State* L = lua_component[i].L;

            lua_getglobal(L, "update");
            if (lua_isfunction(L, -1))
            {
                if (lua_pcall(L, 0, 0, 0))
                {
                    tm_logger_api->printf(TM_LOG_TYPE_ERROR, "Could not call `update` function: %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
            else
            {
                tm_logger_api->printf(TM_LOG_TYPE_ERROR, "`update` function not found!\n");
                lua_pop(L, 1);
            }
        }
    }

    TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
}

static bool engine_filter__lua_component(tm_engine_o* inst, const tm_component_type_t* components, uint32_t num_components, const tm_component_mask_t* mask)
{
    return tm_entity_mask_has_component(mask, components[0]);
}

void component__register_engine(struct tm_entity_context_o* ctx)
{
    const tm_component_type_t lua_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__LUA_COMPONENT);

    const tm_engine_i lua_component_engine = {
        .ui_name = TM_TT_TYPE__LUA_COMPONENT_ENGINE,
        .hash  = TM_TT_TYPE_HASH__LUA_COMPONENT_ENGINE,
        .exclusive = true,
        .num_components = 1,
        .components = { lua_component },
        .writes = { false },
        .update = engine_update__lua_component,
        .filter = engine_filter__lua_component,
        .inst = (tm_engine_o*)ctx,
    };
    tm_entity_api->register_engine(ctx, &lua_component_engine);
}
