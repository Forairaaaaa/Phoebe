/*
 *  Minimal 'console' binding.
 *
 *  https://github.com/DeveloperToolsWG/console-object/blob/master/api.md
 *  https://developers.google.com/web/tools/chrome-devtools/debug/console/console-reference
 *  https://developer.mozilla.org/en/docs/Web/API/console
 */

#include "binding.h"
#include <stdio.h>
#include <stdarg.h>
#include <mooncake_log.h>

static const std::string _tag = "JS";

/* XXX: Add some form of log level filtering. */

/* XXX: Should all output be written via e.g. console.write(formattedMsg)?
 * This would make it easier for user code to redirect all console output
 * to a custom backend.
 */

/* XXX: Init console object using duk_def_prop() when that call is available. */

static duk_ret_t duk__console_log_helper(duk_context* ctx,
                                         const char* error_name,
                                         mclog::LogLevel_t logLevel = mclog::level_info)
{
    duk_idx_t n = duk_get_top(ctx);
    duk_idx_t i;

    duk_get_global_string(ctx, "console");
    duk_get_prop_string(ctx, -1, "format");

    for (i = 0; i < n; i++) {
        if (duk_check_type_mask(ctx, i, DUK_TYPE_MASK_OBJECT)) {
            /* Slow path formatting. */
            duk_dup(ctx, -1); /* console.format */
            duk_dup(ctx, i);
            duk_call(ctx, 1);
            duk_replace(ctx, i); /* arg[i] = console.format(arg[i]); */
        }
    }

    duk_pop_2(ctx);

    duk_push_string(ctx, " ");
    duk_insert(ctx, 0);
    duk_join(ctx, n);

    if (error_name) {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "%s", duk_require_string(ctx, -1));
        duk_push_string(ctx, "name");
        duk_push_string(ctx, error_name);
        duk_def_prop(ctx, -3, DUK_DEFPROP_FORCE | DUK_DEFPROP_HAVE_VALUE); /* to get e.g. 'Trace: 1 2 3' */
        duk_get_prop_string(ctx, -1, "stack");
    }

    // fprintf(output, "%s\n", duk_to_string(ctx, -1));
    // if (flags & DUK_CONSOLE_FLUSH) {
    //     fflush(output);
    // }

    switch (logLevel) {
        case mclog::level_info: {
            mclog::tagInfo(_tag, "{}", duk_to_string(ctx, -1));
            break;
        }
        case mclog::level_warn: {
            mclog::tagWarn(_tag, "{}", duk_to_string(ctx, -1));
            break;
        }
        case mclog::level_error: {
            mclog::tagError(_tag, "{}", duk_to_string(ctx, -1));
            break;
        }
        case mclog::level_debug: {
            mclog::tagDebug(_tag, "{}", duk_to_string(ctx, -1));
            break;
        }
    }

    return 0;
}

static duk_ret_t duk__console_assert(duk_context* ctx)
{
    if (duk_to_boolean(ctx, 0)) {
        return 0;
    }
    duk_remove(ctx, 0);

    return duk__console_log_helper(ctx, "AssertionError", mclog::level_error);
}

static duk_ret_t duk__console_log(duk_context* ctx)
{
    return duk__console_log_helper(ctx, NULL);
}

static duk_ret_t duk__console_trace(duk_context* ctx)
{
    return duk__console_log_helper(ctx, "Trace", mclog::level_error);
}

static duk_ret_t duk__console_info(duk_context* ctx)
{
    return duk__console_log_helper(ctx, NULL);
}

static duk_ret_t duk__console_warn(duk_context* ctx)
{
    return duk__console_log_helper(ctx, NULL, mclog::level_warn);
}

static duk_ret_t duk__console_error(duk_context* ctx)
{
    return duk__console_log_helper(ctx, "Error", mclog::level_error);
}

static duk_ret_t duk__console_dir(duk_context* ctx)
{
    /* For now, just share the formatting of .log() */
    return duk__console_log_helper(ctx, NULL);
}

static void duk__console_reg_vararg_func(duk_context* ctx, duk_c_function func, const char* name)
{
    duk_push_c_function(ctx, func, DUK_VARARGS);
    duk_push_string(ctx, "name");
    duk_push_string(ctx, name);
    duk_def_prop(ctx, -3,
                 DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_FORCE); /* Improve stacktraces by displaying function name */
    duk_put_prop_string(ctx, -2, name);
}

void js_binding::duk_console_init(duk_context* ctx)
{
    duk_push_object(ctx);

    /* Custom function to format objects; user can replace.
     * For now, try JX-formatting and if that fails, fall back
     * to ToString(v).
     */
    duk_eval_string(ctx,
                    "(function (E) {"
                    "return function format(v){"
                    "try{"
                    "return E('jx',v);"
                    "}catch(e){"
                    "return String(v);" /* String() allows symbols, ToString() internal algorithm doesn't. */
                    "}"
                    "};"
                    "})(Duktape.enc)");
    duk_put_prop_string(ctx, -2, "format");

    duk__console_reg_vararg_func(ctx, duk__console_assert, "assert");
    duk__console_reg_vararg_func(ctx, duk__console_log, "log");
    duk__console_reg_vararg_func(ctx, duk__console_log, "debug"); /* alias to console.log */
    duk__console_reg_vararg_func(ctx, duk__console_trace, "trace");
    duk__console_reg_vararg_func(ctx, duk__console_info, "info");

    duk__console_reg_vararg_func(ctx, duk__console_warn, "warn");
    duk__console_reg_vararg_func(ctx, duk__console_error, "error");
    duk__console_reg_vararg_func(ctx, duk__console_error, "exception"); /* alias to console.error */
    duk__console_reg_vararg_func(ctx, duk__console_dir, "dir");

    duk_put_global_string(ctx, "console");
}
