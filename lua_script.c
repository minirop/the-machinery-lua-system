extern struct tm_the_truth_api* tm_the_truth_api;
extern struct tm_the_machinery_api* tm_the_machinery_api;
extern struct tm_docking_api* tm_docking_api;

#include <foundation/the_truth.h>
#include <foundation/macros.h>
#include <foundation/the_truth_assets.h>
#include <the_machinery/the_machinery.h>

#include <plugins/the_machinery_shared/asset_aspects.h>
#include <plugins/editor_views/properties.h>
#include <plugins/ui/ui.h>
#include <plugins/ui/docking.h>

#include "lua_script.h"
#include "lua_editor.h"

static void open_asset(struct tm_application_o *app, struct tm_ui_o *ui, struct tm_tab_i *from_tab, tm_the_truth_o *tt, tm_tt_id_t asset, enum tm_asset_open_mode open_mode)
{
    const tm_docking_find_tab_opt_t opt = {
        .from_tab = from_tab,
        .in_ui = ui,
        .exclude_pinned = true,
    };

    const bool pin = open_mode == TM_ASSET_OPEN_MODE_CREATE_TAB_AND_PIN;
    tm_tab_i *tab = tm_the_machinery_api->create_or_select_tab(app, ui, TM_LUA_TAB_VT_NAME, &opt);

    if (pin)
        tm_docking_api->pin_object(tab, tt, asset);
    else
        tab->vt->set_root(tab->inst, tt, asset);
}

static tm_asset_open_aspect_i *open_i = &(tm_asset_open_aspect_i){
    .open = open_asset,
};

void asset__truth__create_types(struct tm_the_truth_o* tt)
{
    static tm_the_truth_property_definition_t asset_properties[] = {
        { "data", TM_THE_TRUTH_PROPERTY_TYPE_STRING, .editor = TM_THE_TRUTH__EDITOR__HIDDEN },
    };
    const tm_tt_type_t type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__LUA_SCRIPT, asset_properties, TM_ARRAY_COUNT(asset_properties));
    tm_the_truth_api->set_aspect(tt, type, TM_TT_ASPECT__FILE_EXTENSION, "lua");
    tm_the_truth_api->set_aspect(tt, type, TM_TT_ASPECT__ASSET_OPEN, open_i);
}
