extern struct tm_the_truth_api* tm_the_truth_api;

#include <foundation/the_truth.h>
#include <foundation/macros.h>
#include <foundation/the_truth_assets.h>

#include <plugins/the_machinery_shared/asset_aspects.h>
#include <plugins/editor_views/properties.h>

#include "lua_script.h"

void asset__truth__create_types(struct tm_the_truth_o* tt)
{
    static tm_the_truth_property_definition_t asset_properties[] = {
        { "data", TM_THE_TRUTH_PROPERTY_TYPE_BUFFER },
    };
    const tm_tt_type_t type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__LUA_SCRIPT, asset_properties, TM_ARRAY_COUNT(asset_properties));
    tm_the_truth_api->set_aspect(tt, type, TM_TT_ASPECT__FILE_EXTENSION, "lua");
}
