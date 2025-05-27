/**
 * @file js_runtime.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "js_runtime.h"
#include "apps/utils/widget/widget.h"
#include "binding/binding.h"
#include <cstddef>
#include <memory>
#include <mooncake_log.h>

using namespace js_runtime;
using namespace js_binding;
using namespace widget;

static const std::string _tag = "JsRuntime";

JsRuntime::JsRuntime()
{
    mclog::tagInfo(_tag, "create instance");
    _duktape_ctx = duk_create_heap_default();
}

JsRuntime::~JsRuntime()
{
    if (_duktape_ctx != NULL) {
        mclog::tagInfo(_tag, "desctroy instance");
        duk_destroy_heap(_duktape_ctx);
    }
}

void JsRuntime::evaluate(const std::string& script)
{
    duk_eval_string(_duktape_ctx, script.c_str());
}

bool JsRuntime::callScriptFunction(const char* functionName, bool logErrorOnFailed)
{
    bool ret = true;
    duk_get_global_string(_duktape_ctx, functionName);
    if (duk_pcall(_duktape_ctx, 0) != 0) {
        if (logErrorOnFailed) {
            mclog::tagError(_tag, "call {}() error: {}", functionName, duk_safe_to_string(_duktape_ctx, -1));
            ret = false;
        }
    }
    duk_pop(_duktape_ctx); /* ignore result */
    return ret;
}

void JsRuntime::reset()
{
    mclog::tagInfo(_tag, "reset");
    if (_duktape_ctx != NULL) {
        duk_destroy_heap(_duktape_ctx);
    }
    _duktape_ctx = duk_create_heap_default();
    _widget_factory.reset();
}

void JsRuntime::loadConsoleBinding()
{
    mclog::tagInfo(_tag, "load console binding");
    duk_console_init(_duktape_ctx);
}

void JsRuntime::loadHalBinding()
{
    mclog::tagInfo(_tag, "load hal binding");
    duk_hal_init(_duktape_ctx);
}

void JsRuntime::loadWidgetBinding(lv_obj_t* widgetParent)
{
    mclog::tagInfo(_tag, "load widget binding");

    _widget_factory.reset();
    _widget_factory = std::make_unique<WidgetFactory>();
    _widget_factory->setWidgetParent(widgetParent);

    duk_widget_init(_duktape_ctx, _widget_factory.get());
}
