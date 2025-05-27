/**
 * @file duk_widget.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "binding.h"
#include <mooncake_log.h>
#include <hal/hal.h>

using namespace widget;
using namespace widget_helper;

static const std::string _tag = "JS";
static const char* _widget_factory_global_name = "__wf";
static const char* _error_msg_null_widget_factory = "Widget factory is null";
static const char* _error_msg_invalid_widget_id = "Invalid widget ID";
static const char* _error_msg_parse_json_failed = "Parse JSON failed";

static WidgetFactory* get_widget_factory(duk_context* ctx)
{
    duk_get_global_string(ctx, _widget_factory_global_name);
    WidgetFactory* widget_facarory = (WidgetFactory*)duk_get_pointer(ctx, -1);
    duk_pop(ctx);
    return widget_facarory;
}

/* -------------------------------------------------------------------------- */
/*                               Widget Factory                               */
/* -------------------------------------------------------------------------- */
static duk_ret_t widget_factory_create(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    const char* type = duk_to_string(ctx, 0);
    int widget_id = widget_factory->create(type);
    duk_push_int(ctx, widget_id);
    return 1;
}

static duk_ret_t widget_factory_destroy(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    widget_factory->destory(widget_id);
    return 0;
}

static void duk_widget_factory_init(duk_context* ctx)
{
    duk_push_c_function(ctx, widget_factory_create, 1);
    duk_put_prop_string(ctx, -2, "create");
    duk_push_c_function(ctx, widget_factory_destroy, 1);
    duk_put_prop_string(ctx, -2, "destroy");
}

/* -------------------------------------------------------------------------- */
/*                                 Widget Base                                */
/* -------------------------------------------------------------------------- */
static duk_ret_t widget_base_set_align(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    const char* alignment = duk_to_string(ctx, 1);
    widget->setAlign(alignment);
    return 0;
}

static duk_ret_t widget_base_set_bg_color(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    const char* hexColor = duk_to_string(ctx, 1);
    widget->setBgColor(hexColor);
    return 0;
}

static duk_ret_t widget_base_set_pos(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    int32_t x = duk_to_int(ctx, 1);
    int32_t y = duk_to_int(ctx, 2);
    widget->setPos(x, y);
    return 0;
}

static duk_ret_t widget_base_set_size(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    int32_t w = duk_to_int(ctx, 1);
    int32_t h = duk_to_int(ctx, 2);
    widget->setSize(w, h);
    return 0;
}

static duk_ret_t widget_base_set_scrollbar_mode(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    lv_scrollbar_mode_t mode = (lv_scrollbar_mode_t)duk_to_int(ctx, 1);
    widget->setScrollbarMode(mode);
    return 0;
}

static duk_ret_t widget_base_set_radius(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    int32_t value = duk_to_int(ctx, 1);
    widget->setRadius(value);
    return 0;
}

static duk_ret_t widget_base_set_border_width(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    int32_t value = duk_to_int(ctx, 1);
    widget->setBorderWidth(value);
    return 0;
}

static duk_ret_t widget_base_set_border_color(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    const char* hexColor = duk_to_string(ctx, 1);
    widget->setBorderColor(hexColor);
    return 0;
}

static duk_ret_t widget_base_set_rotation(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetBase* widget = widget_factory->getBase(widget_id);
    if (!widget) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    int32_t value = duk_to_int(ctx, 1);
    widget->setRotation(value);
    return 0;
}

static void duk_widget_base_init(duk_context* ctx)
{
    duk_push_c_function(ctx, widget_base_set_align, 2);
    duk_put_prop_string(ctx, -2, "setAlign");
    duk_push_c_function(ctx, widget_base_set_bg_color, 2);
    duk_put_prop_string(ctx, -2, "setBgColor");
    duk_push_c_function(ctx, widget_base_set_pos, 3);
    duk_put_prop_string(ctx, -2, "setPos");
    duk_push_c_function(ctx, widget_base_set_size, 3);
    duk_put_prop_string(ctx, -2, "setSize");
    duk_push_c_function(ctx, widget_base_set_scrollbar_mode, 2);
    duk_put_prop_string(ctx, -2, "setScrollbarMode");
    duk_push_c_function(ctx, widget_base_set_radius, 2);
    duk_put_prop_string(ctx, -2, "setRadius");
    duk_push_c_function(ctx, widget_base_set_border_width, 2);
    duk_put_prop_string(ctx, -2, "setBorderWidth");
    duk_push_c_function(ctx, widget_base_set_border_color, 2);
    duk_put_prop_string(ctx, -2, "setBorderColor");
    duk_push_c_function(ctx, widget_base_set_rotation, 2);
    duk_put_prop_string(ctx, -2, "setRotation");
}

/* -------------------------------------------------------------------------- */
/*                                Widget Label                                 */
/* -------------------------------------------------------------------------- */
static duk_ret_t widget_set_label_font(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetLabel* label = widget_factory->getLabel(widget_id);
    if (!label) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    const char* fontName = duk_to_string(ctx, 1);
    label->setFont(fontName);
    return 0;
}

static duk_ret_t widget_set_label_text_color(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetLabel* label = widget_factory->getLabel(widget_id);
    if (!label) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    const char* hexColor = duk_to_string(ctx, 1);
    label->setTextColor(hexColor);
    return 0;
}

static duk_ret_t widget_set_label_text(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetLabel* label = widget_factory->getLabel(widget_id);
    if (!label) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    const char* text = duk_to_string(ctx, 1);
    label->setText(text);
    return 0;
}

static void duk_widget_label_init(duk_context* ctx)
{
    duk_push_c_function(ctx, widget_set_label_font, 2);
    duk_put_prop_string(ctx, -2, "setLabelFont");
    duk_push_c_function(ctx, widget_set_label_text_color, 2);
    duk_put_prop_string(ctx, -2, "setLabelTextColor");
    duk_push_c_function(ctx, widget_set_label_text, 2);
    duk_put_prop_string(ctx, -2, "setLabelText");
}

/* -------------------------------------------------------------------------- */
/*                               Widget Clock                                  */
/* -------------------------------------------------------------------------- */
static duk_ret_t widget_set_clock_style(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetClock* clock = widget_factory->getClock(widget_id);
    if (!clock) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    const char* style_json = duk_to_string(ctx, 1);
    if (!clock->setStyle(style_json)) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_parse_json_failed);
    }

    return 0;
}

static duk_ret_t widget_update_clock(duk_context* ctx)
{
    auto widget_factory = get_widget_factory(ctx);
    if (!widget_factory) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_null_widget_factory);
    }

    int widget_id = duk_to_int(ctx, 0);
    WidgetClock* clock = widget_factory->getClock(widget_id);
    if (!clock) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, _error_msg_invalid_widget_id);
    }

    clock->update();
    return 0;
}

static void duk_widget_clock_init(duk_context* ctx)
{
    duk_push_c_function(ctx, widget_set_clock_style, 2);
    duk_put_prop_string(ctx, -2, "setClockStyle");
    duk_push_c_function(ctx, widget_update_clock, 1);
    duk_put_prop_string(ctx, -2, "updateClock");
}

void js_binding::duk_widget_init(duk_context* ctx, WidgetFactory* widgetFactory)
{
    // 将 Widget factory 实例指针存到 context
    duk_push_pointer(ctx, (void*)widgetFactory);
    duk_put_global_string(ctx, _widget_factory_global_name);

    duk_push_object(ctx);
    duk_widget_factory_init(ctx);
    duk_widget_base_init(ctx);
    duk_widget_label_init(ctx);
    duk_widget_clock_init(ctx);
    duk_put_global_string(ctx, "widget");
}
