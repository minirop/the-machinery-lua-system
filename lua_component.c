extern struct tm_entity_api* tm_entity_api;
extern struct tm_transform_component_api* tm_transform_component_api;
extern struct tm_temp_allocator_api* tm_temp_allocator_api;
extern struct tm_the_truth_api* tm_the_truth_api;
extern struct tm_localizer_api* tm_localizer_api;
extern struct tm_logger_api* tm_logger_api;
extern struct tm_ui_api* tm_ui_api;
extern struct tm_properties_view_api* tm_properties_view_api;

#include <plugins/entity/entity.h>
#include <plugins/entity/transform_component.h>
#include <plugins/ui/ui.h>
#include <plugins/editor_views/properties.h>
#include <plugins/the_machinery_shared/component_interfaces/editor_ui_interface.h>

#include <foundation/api_registry.h>
#include <foundation/carray.inl>
#include <foundation/localizer.h>
#include <foundation/math.inl>
#include <foundation/rect.inl>
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
    .category = component__category,
};

static const char *component__get_display_name(void)
{
    return TM_LOCALIZE("Lua script");
}

static float draw_lua_property(const char* name, lua_State* L, tm_properties_ui_args_t *args, tm_rect_t item_rect)
{
    tm_ui_o *ui = args->ui;
    tm_ui_style_t *uistyle = args->uistyle;

    const tm_rect_t label_r = tm_rect_split_left(item_rect, args->metrics[TM_PROPERTIES_METRIC_LABEL_WIDTH], args->metrics[TM_PROPERTIES_METRIC_MARGIN], 0);
    const tm_rect_t control_r = tm_rect_split_left(item_rect, args->metrics[TM_PROPERTIES_METRIC_LABEL_WIDTH], args->metrics[TM_PROPERTIES_METRIC_MARGIN], 1);

    int type = lua_type(L, -1);

    switch (type)
    {
    case LUA_TNUMBER:
    case LUA_TSTRING:
    case LUA_TTABLE:
        tm_ui_api->label(ui, uistyle, &(tm_ui_label_t){ .rect = label_r, .text = name });
        break;
    }

    if (type == LUA_TNUMBER)
    {
        const tm_ui_spinner_t s = {
            .id = tm_ui_api->make_id(ui),
            .rect = control_r,
        };

        double initial = lua_tonumber(L, -1);
        double value = initial;
        enum tm_ui_interaction_result_t res = tm_ui_api->spinner(ui, uistyle, &s, &value, &initial);
        if (res == TM_UI_INTERACTION_RESULT_COMMIT)
        {
            // TODO
        }
    }
    else if (type == LUA_TSTRING)
    {
        const tm_ui_textedit_t s = {
            .id = tm_ui_api->make_id(ui),
            .rect = control_r,
        };

        char buffer[128] = {};
        strcpy(buffer, lua_tostring(L, -1));
        if (tm_ui_api->textedit(ui, uistyle, &s, buffer, 128))
        {
            // TODO
        }
    }
    else if (type == LUA_TTABLE)
    {
        lua_len(L, -1);
        int size = lua_tointeger(L, -1);
        lua_pop(L, 1);

        const char* items[size];
        memset(items, 0, size*sizeof(const char*));

        int index = 0;
        lua_pushnil(L);
        while (lua_next(L, -2))
        {
            items[index++] = lua_tostring(L, -1);
            lua_pop(L, 1);
        }

        const tm_ui_dropdown_t c = {
            .id = tm_ui_api->make_id(ui),
            .rect = control_r,
            .num_items = size,
            .items = items,
        };

        uint32_t selected = 0;
        if (tm_ui_api->dropdown(ui, uistyle, &c, &selected))
        {
            // TODO
        }
    }
    else if (type == LUA_TBOOLEAN)
    {
        const tm_ui_checkbox_t c = {
            .id = tm_ui_api->make_id(ui),
            .rect = item_rect,
            .text = name,
        };

        bool checked = lua_toboolean(L, -1);
        if (tm_ui_api->checkbox(ui, uistyle, &c, &checked))
        {
            // TODO
        }
    }
    else
    {
        return item_rect.y;
    }

    return item_rect.y + item_rect.h + 5;
}

static float component__custom_ui(tm_properties_ui_args_t *args, tm_rect_t item_rect, tm_tt_id_t object, uint32_t indent)
{
    item_rect.y = tm_properties_view_api->ui_object_default(args, item_rect, object, indent);

    tm_the_truth_o *tt = args->tt;

    const tm_the_truth_object_o* asset_r = tm_tt_read(tt, object);
    tm_tt_id_t id = tm_the_truth_api->get_reference(tt, asset_r, TM_TT_PROP__LUA_COMPONENT__SCRIPT);
    if (id.u64)
    {
        tm_tt_buffer_t buffer = tm_the_truth_api->get_buffer(tt, tm_tt_read(tt, id), TM_TT_PROP__LUA_SCRIPT__DATA);

        lua_State* L = luaL_newstate();
        luaL_openlibs(L);

        lua_sol__register(L);

        luaL_loadbuffer(L, buffer.data, buffer.size, "reading properties");
        if (lua_isfunction(L, -1))
        {
            if (lua_pcall(L, 0, 0, 0) == LUA_OK)
            {
                lua_getglobal(L, "properties");

                if (lua_istable(L, -1))
                {
                    lua_pushnil(L);
                    while (lua_next(L, -2))
                    {
                        if (lua_istable(L, -1))
                        {
                            lua_pushnil(L);
                            if (lua_next(L, -2))
                            {
                                const char *name = lua_tostring(L, -2);
                                item_rect.y = draw_lua_property(name, L, args, item_rect);
                                lua_pop(L, 2);
                            }
                        }

                        lua_pop(L, 1);
                    }
                }

                lua_pop(L, -1);
            }
            else
            {
                lua_pop(L, 1);
            }
        }
        else
        {
            lua_pop(L, 1);
        }

        lua_close(L);
    }

    return item_rect.y;
}

static tm_properties_aspect_i *properties_aspect = &(tm_properties_aspect_i){
    .get_type_display_name = component__get_display_name,
    .custom_ui = component__custom_ui,
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
    tm_the_truth_api->set_aspect(tt, lua_component_type, TM_TT_ASPECT__PROPERTIES, properties_aspect);
    tm_the_truth_api->set_property_aspect(tt, lua_component_type, TM_TT_PROP__LUA_COMPONENT__SCRIPT, TM_TT_PROP_ASPECT__PROPERTIES__ASSET_PICKER, TM_TT_TYPE__LUA_SCRIPT);
}

static bool component__load_asset(tm_component_manager_o* man, struct tm_entity_commands_o *commands, tm_entity_t e, void* c_vp, const tm_the_truth_o* tt, tm_tt_id_t asset)
{
    struct tm_lua_component_t* c = c_vp;
    lua_State* L = c->L;

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

    return true;
}

static void component__add(tm_component_manager_o *manager, struct tm_entity_commands_o *commands, tm_entity_t e, void *data)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_sol__register(L);
    lua_sol__set_entity(L, e);

    struct tm_lua_component_t* c = data;
    c->L = L;
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
        .add = component__add,
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
