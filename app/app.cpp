/**
 * @file app.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "app.h"
#include "hal/hal.h"
#include "apps/app_installer.h"
#include <mooncake.h>
#include <mooncake_log.h>
#include <string>
#include <lvgl.h>

using namespace mooncake;

static const std::string _tag = "APP";

void APP::Init(InitCallback_t callback)
{
    mclog::tagInfo(_tag, "init");

    /* ------------------------------ HAL Injection ----------------------------- */
    // 硬件抽象层注入
    mclog::tagInfo(_tag, "hal injection");
    if (callback.onHalInjection) {
        callback.onHalInjection();
    }

    /* -------------------------------- Mooncake -------------------------------- */
    // Mooncake 初始化
    mclog::tagInfo(_tag, "create mooncake");

    // 踢一脚懒加载
    GetMooncake();

    // 安装 App
    on_install_apps();
}

void APP::Update()
{
    // 更新 Mooncake
    GetMooncake().update();

    // 更新 Lvgl
    lv_timer_handler();

    // 重置看门狗，建议实现这个看门狗，避免某个生命周期回调卡死了
    HAL::SysCtrl().feedTheDog();
}

bool APP::IsDone()
{
    return false;
}

void APP::Destroy()
{
    DestroyMooncake();
    HAL::Destroy();
}
