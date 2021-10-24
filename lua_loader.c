struct tm_entity_api* tm_entity_api;
struct tm_transform_component_api* tm_transform_component_api;
struct tm_temp_allocator_api* tm_temp_allocator_api;
struct tm_the_truth_api* tm_the_truth_api;
struct tm_localizer_api* tm_localizer_api;
struct tm_asset_io_api* tm_asset_io_api;
struct tm_allocator_api* tm_allocator_api;
struct tm_path_api* tm_path_api;
struct tm_task_system_api* task_system;
struct tm_api_registry_api* tm_global_api_registry;
struct tm_sprintf_api* tm_sprintf_api;
struct tm_os_api* tm_os_api;
struct tm_logger_api* tm_logger_api;
struct tm_ui_api* tm_ui_api;
struct tm_properties_view_api* tm_properties_view_api;
struct tm_the_machinery_api* tm_the_machinery_api;
struct tm_docking_api* tm_docking_api;

#include <foundation/the_truth.h>
#include <foundation/api_registry.h>

#include <the_machinery/the_machinery.h>
#include <plugins/entity/entity.h>
#include <plugins/entity/transform_component.h>
#include <plugins/ui/ui.h>
#include <plugins/ui/docking.h>
#include <plugins/editor_views/properties.h>
#include <foundation/temp_allocator.h>
#include <foundation/localizer.h>
#include <foundation/asset_io.h>
#include <foundation/allocator.h>
#include <foundation/path.h>
#include <foundation/task_system.h>
#include <foundation/sprintf.h>
#include <foundation/log.h>
#include <foundation/os.h>

#include "lua_component.h"
#include "lua_script.h"
#include "lua_importer.h"
#include "lua_editor.h"

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api* reg, bool load)
{
    tm_entity_api = tm_get_api(reg, tm_entity_api);
    tm_transform_component_api = tm_get_api(reg, tm_transform_component_api);
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_temp_allocator_api = tm_get_api(reg, tm_temp_allocator_api);
    tm_localizer_api = tm_get_api(reg, tm_localizer_api);
    tm_asset_io_api = tm_get_api(reg, tm_asset_io_api);
    tm_allocator_api = tm_get_api(reg, tm_allocator_api);
    tm_path_api = tm_get_api(reg, tm_path_api);
    task_system = tm_get_api(reg, tm_task_system_api);
    tm_sprintf_api = tm_get_api(reg, tm_sprintf_api);
    tm_os_api = tm_get_api(reg, tm_os_api);
    tm_logger_api = tm_get_api(reg, tm_logger_api);
    tm_ui_api = tm_get_api(reg, tm_ui_api);
    tm_properties_view_api = tm_get_api(reg, tm_properties_view_api);
    tm_the_machinery_api = tm_get_api(reg, tm_the_machinery_api);
    tm_docking_api = tm_get_api(reg, tm_docking_api);
    tm_global_api_registry = reg;

    importer__register(load);
    editor__register(reg, load);

    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, component__truth__create_types);
    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, asset__truth__create_types);
    tm_add_or_remove_implementation(reg, load, tm_entity_create_component_i, component__create);
    tm_add_or_remove_implementation(reg, load, tm_entity_register_engines_simulation_i, component__register_engine);
}
