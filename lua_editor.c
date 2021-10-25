extern struct tm_temp_allocator_api* tm_temp_allocator_api;
extern struct tm_allocator_api* tm_allocator_api;
extern struct tm_the_truth_api* tm_the_truth_api;
extern struct tm_ui_api* tm_ui_api;
extern struct tm_localizer_api* tm_localizer_api;
extern struct tm_sprintf_api* tm_sprintf_api;

#include "lua_editor.h"
#include "lua_script.h"

#include <foundation/the_truth.h>
#include <foundation/macros.h>
#include <foundation/allocator.h>
#include <foundation/temp_allocator.h>
#include <foundation/api_registry.h>
#include <foundation/string.inl>
#include <foundation/carray.inl>
#include <foundation/carray_print.inl>

#include <plugins/ui/ui.h>
#include <plugins/ui/docking.h>
#include <plugins/ui/ui_custom.h>
#include <the_machinery/the_machinery_tab.h>

struct tm_tab_o
{
    tm_tab_i tm_tab_i;
    tm_allocator_i allocator;

    tm_tt_id_t asset;
    tm_the_truth_o *tt;
};

static void tab__ui(tm_tab_o *tab, tm_ui_o *ui, const tm_ui_style_t *uistyle_in, tm_rect_t rect)
{
    tm_ui_style_t *uistyle = (tm_ui_style_t[]){*uistyle_in};
    if (tab->asset.u64)
    {
        TM_INIT_TEMP_ALLOCATOR_WITH_ADAPTER(ta, a);
        const char *text = tm_the_truth_api->get_string(tab->tt, tm_tt_read(tab->tt, tab->asset), TM_TT_PROP__LUA_SCRIPT__TEXT);

        char *buffer = NULL;
        tm_carray_temp_print(&buffer, ta, text);

        tm_ui_buffers_t uib = tm_ui_api->buffers(ui);

        const int margin = 10;
        static float scroll_y = 0;
        static tm_tt_id_t scroll_id = { 0 };
        if (scroll_id.u64 != tab->asset.u64) {
            scroll_id = tab->asset;
            scroll_y = 0;
        }
        tm_rect_t canvas_r = { .w = rect.w - margin * 2, .h = 5000.f };
        tm_ui_scrollview_t scrollview = {
            .rect = { .x = rect.x + margin, .y = rect.y + margin, .w = rect.w - 2 * margin, .h = rect.h - 2 * margin },
            .canvas = canvas_r,
            .visibility_y = TM_UI_SCROLLBAR_VISIBILITY_WHEN_NEEDED,
        };
        tm_rect_t scroll_content_r;
        tm_ui_api->begin_scrollview(ui, uistyle_in, &scrollview, NULL, &scroll_y, &scroll_content_r);

        tm_ui_textedit_t textedit = { 0 };
        textedit.id = tm_ui_api->make_id(ui);
        textedit.rect.x = scroll_content_r.x;
        textedit.rect.y = scroll_content_r.y - scroll_y;
        textedit.rect.w = canvas_r.w;
        textedit.rect.h = canvas_r.h;
        tm_rect_t caret_rect = { 0 };
        bool res = tm_ui_api->multiline_textedit(ui, uistyle_in, &textedit, &buffer, a, &caret_rect);
        if (caret_rect.y - rect.y > rect.h && fabsf(uib.input->mouse_wheel) < 1.f)
            scroll_y += caret_rect.h;
        else if (caret_rect.y < rect.y && fabs(uib.input->mouse_wheel) < 1.f)
            scroll_y -= caret_rect.h;

        tm_ui_api->end_scrollview(ui, NULL, &scroll_y, true);

        if (res)
        {
            tm_the_truth_object_o *text_w = tm_the_truth_api->write(tab->tt, tab->asset);
            tm_the_truth_api->set_string(tab->tt, text_w, TM_TT_PROP__LUA_SCRIPT__TEXT, buffer);
            tm_the_truth_api->commit(tab->tt, text_w, TM_TT_NO_UNDO_SCOPE);
        }

        TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
    }
    else
    {
        rect.h = 20;
        tm_ui_text_t *text = &(tm_ui_text_t){.align = TM_UI_ALIGN_CENTER, .text = "Please open a .lua asset.", .rect = rect};
        tm_ui_api->text(ui, uistyle, text);
    }
}

static const char *tab__create_menu_name(void)
{
    return "Lua Editor";
}

static const char *tab__title(tm_tab_o *tab, struct tm_ui_o *ui)
{
    return "Lua Editor";
}

static tm_tab_vt *lua_tab_vt;

static tm_tab_i *tab__create(tm_tab_create_context_t *context, tm_ui_o *ui)
{
    tm_allocator_i allocator = tm_allocator_api->create_child(context->allocator, "Lua Editor");
    uint64_t *id = context->id;

    tm_tab_o *tab = tm_alloc(&allocator, sizeof(tm_tab_o));
    *tab = (tm_tab_o){
        .tm_tab_i = {
            .vt = lua_tab_vt,
            .inst = (tm_tab_o *)tab,
            .root_id = *id,
        },
        .allocator = allocator,
    };

    *id += 1000000;
    return &tab->tm_tab_i;
}

static void tab__destroy(tm_tab_o *tab)
{
    tm_allocator_i a = tab->allocator;
    tm_free(&a, tab, sizeof(*tab));
    tm_allocator_api->destroy_child(&a);
}

static void tab__set_root(tm_tab_o *inst, struct tm_the_truth_o *tt, tm_tt_id_t root)
{
    inst->asset = root;
    inst->tt = tt;
}

static tm_tab_vt_root_t tab__root(tm_tab_o *tab)
{
    return (tm_tab_vt_root_t){tab->tt, tab->asset};
}

static tm_tab_vt *lua_tab_vt = &(tm_tab_vt){
    .name = TM_LUA_TAB_VT_NAME,
    .name_hash = TM_LUA_TAB_VT_NAME_HASH,
    .create_menu_name = tab__create_menu_name,
    .create = tab__create,
    .destroy = tab__destroy,
    .title = tab__title,
    .ui = tab__ui,
    .set_root = tab__set_root,
    .root = tab__root,
};

void editor__register(struct tm_api_registry_api* reg, bool load)
{
	tm_add_or_remove_implementation(reg, load, tm_tab_vt, lua_tab_vt);
}
