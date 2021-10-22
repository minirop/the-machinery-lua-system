extern struct tm_the_truth_api* tm_the_truth_api;
extern struct tm_asset_io_api* tm_asset_io_api;
extern struct tm_temp_allocator_api* tm_temp_allocator_api;
extern struct tm_allocator_api* tm_allocator_api;
extern struct tm_path_api* tm_path_api;
extern struct tm_task_system_api* task_system;
extern struct tm_api_registry_api* tm_global_api_registry;
extern struct tm_sprintf_api* tm_sprintf_api;
extern struct tm_localizer_api* tm_localizer_api;
extern struct tm_os_api* tm_os_api;
extern struct tm_logger_api* tm_logger_api;

#include <foundation/the_truth.h>
#include <foundation/macros.h>
#include <foundation/the_truth_assets.h>
#include <foundation/asset_io.h>
#include <foundation/carray_print.inl>
#include <foundation/string.inl>
#include <foundation/localizer.h>
#include <foundation/sprintf.h>
#include <foundation/os.h>
#include <foundation/buffer.h>
#include <foundation/task_system.h>
#include <foundation/path.h>
#include <foundation/log.h>
#include <foundation/api_registry.h>

#include <plugins/editor_views/asset_browser.h>

#include "lua_script.h"

static bool asset_io__enabled(struct tm_asset_io_o *inst)
{
    return true;
}

static bool asset_io__can_import(struct tm_asset_io_o *inst, const char *extension)
{
    return tm_strcmp_ignore_case(extension, "lua") == 0;
}

static bool asset_io__can_reimport(struct tm_asset_io_o *inst, struct tm_the_truth_o *tt, tm_tt_id_t asset)
{
    const tm_tt_id_t object = tm_the_truth_api->get_subobject(tt, tm_tt_read(tt, asset), TM_TT_PROP__ASSET__OBJECT);
    return object.type == tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__LUA_SCRIPT).u64;
}

static void asset_io__importer_extensions_string(struct tm_asset_io_o *inst, char **output, struct tm_temp_allocator_i *ta, const char *separator)
{
    //tm_carray_temp_printf(output, ta, "lua");
}

static void asset_io__importer_description_string(struct tm_asset_io_o *inst, char **output, struct tm_temp_allocator_i *ta, const char *separator)
{
    //tm_carray_temp_printf(output, ta, ".lua");
}

struct task__import_lua
{
    uint64_t bytes;
    struct tm_asset_io_import args;
    char file[8];
};

static void task__import_lua(void *data, uint64_t task_id)
{
    struct task__import_lua *task = (struct task__import_lua *)data;
    const struct tm_asset_io_import *args = &task->args;
    const char *txt_file = task->file;
    tm_the_truth_o *tt = args->tt;
    tm_file_stat_t stat = tm_os_api->file_system->stat(txt_file);
    if (stat.exists)
    {
        tm_buffers_i *buffers = tm_the_truth_api->buffers(tt);
        void *buffer = buffers->allocate(buffers->inst, stat.size, false);
        tm_file_o f = tm_os_api->file_io->open_input(txt_file);
        const int64_t read = tm_os_api->file_io->read(f, buffer, stat.size);
        tm_os_api->file_io->close(f);

        if (read == (int64_t)stat.size)
        {
        	const uint32_t buffer_id = buffers->add(buffers->inst, buffer, stat.size, 0);
            const tm_tt_type_t plugin_asset_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__LUA_SCRIPT);
            const tm_tt_id_t asset_id = tm_the_truth_api->create_object_of_type(tt, plugin_asset_type, TM_TT_NO_UNDO_SCOPE);
            tm_the_truth_object_o *asset_obj = tm_the_truth_api->write(tt, asset_id);
            tm_the_truth_api->set_buffer(tt, asset_obj, TM_TT_PROP__LUA_SCRIPT__DATA, buffer_id);
            if (args->reimport_into.u64)
            {
                tm_the_truth_api->retarget_write(tt, asset_obj, args->reimport_into);
                tm_the_truth_api->commit(tt, asset_obj, args->undo_scope);
                tm_the_truth_api->destroy_object(tt, asset_id, args->undo_scope);
            }
            else
            {
                tm_the_truth_api->commit(tt, asset_obj, args->undo_scope);
                const char *asset_name = tm_path_api->base(tm_str(txt_file)).data;
                struct tm_asset_browser_add_asset_api *add_asset = tm_get_api(tm_global_api_registry, tm_asset_browser_add_asset_api);
                const tm_tt_id_t current_dir = add_asset->current_directory(add_asset->inst, args->ui);
                const bool should_select = args->asset_browser.u64 && tm_the_truth_api->version(tt, args->asset_browser) == args->asset_browser_version_at_start;
                add_asset->add(add_asset->inst, current_dir, asset_id, asset_name, args->undo_scope, should_select, args->ui, 0, 0);
            }
        }
        else
        {
            tm_logger_api->printf(TM_LOG_TYPE_INFO, "import lua: cound not read %s\n", txt_file);
        }
    }
    else
    {
        tm_logger_api->printf(TM_LOG_TYPE_INFO, "import lua: cound not find %s\n", txt_file);
    }
    tm_free(args->allocator, task, task->bytes);
}

static uint64_t asset_io__import_asset(struct tm_asset_io_o *inst, const char *file, const struct tm_asset_io_import *args)
{
    const uint64_t bytes = sizeof(struct task__import_lua) + strlen(file);
    struct task__import_lua *task = tm_alloc(args->allocator, bytes);
    *task = (struct task__import_lua){
        .bytes = bytes,
        .args = *args,
    };
    strcpy(task->file, file);
    return task_system->run_task(task__import_lua, task, "Import Lua Script");
}

static struct tm_asset_io_i txt_asset_io = {
    .enabled = asset_io__enabled,
    .can_import = asset_io__can_import,
    .can_reimport = asset_io__can_reimport,
    .importer_extensions_string = asset_io__importer_extensions_string,
    .importer_description_string = asset_io__importer_description_string,
    .import_asset = asset_io__import_asset
};

void importer__register(bool load)
{
    if (load)
        tm_asset_io_api->add_asset_io(&txt_asset_io);
    else
        tm_asset_io_api->remove_asset_io(&txt_asset_io);
}
