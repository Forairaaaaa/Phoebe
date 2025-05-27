/**
 * @file debug.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "debug.h"
#include <mooncake_log.h>
#include <hal/hal.h>

#ifndef ENABLE_DEBUG_OUTPUT
#define ENABLE_DEBUG_OUTPUT 1
#endif

void debug::log_heap_usage()
{
#if ENABLE_DEBUG_OUTPUT
    static auto current_free_heap = HAL::SysCtrl().freeHeapSize();
    mclog::debug("free heap: {}, {} used since last call", HAL::SysCtrl().freeHeapSize(),
                 current_free_heap - HAL::SysCtrl().freeHeapSize());
    current_free_heap = HAL::SysCtrl().freeHeapSize();
#endif
}
