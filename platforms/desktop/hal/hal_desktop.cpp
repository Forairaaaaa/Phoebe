/**
 * @file hal_desktop.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "hal_desktop.h"
#include "hal_config.h"
#include "components/system_ctrl_sdl/system_ctrl_sdl.h"
#include "components/system_config_std/system_config_std.h"
#include "components/button_sdl/button_sdl.h"
#include "components/display/display.h"
#include "components/ble/ble.h"
#include <memory>
#include <mooncake_log.h>
#include <lvgl.h>

void HalDesktop::init()
{
    lvgl_init();

    // 创建组件实例
    _components.system_control = std::make_unique<SystemControlSdl>();
    _components.system_config = std::make_unique<SystemConfigStd>();
    _components.button = std::make_unique<ButtonSdl>();
    _components.display = std::make_unique<DisplaySdl>();
    _components.display->init();
    _components.ble = std::make_unique<BlePython>();
    _components.ble->init();

    // 拉取一下保存的配置
    HAL::SysCfg().loadConfig();
    HAL::SysCfg().logConfig();
}

/* -------------------------------------------------------------------------- */
/*                                    Lvgl                                    */
/* -------------------------------------------------------------------------- */
// https://github.com/lvgl/lv_port_pc_vscode/blob/master/main/src/main.c

void HalDesktop::lvgl_init()
{
    mclog::info("lvgl init");

    lv_init();

    lv_group_set_default(lv_group_create());

    auto display = lv_sdl_window_create(HAL_SCREEN_WIDTH, HAL_SCREEN_HEIGHT);
    lv_display_set_default(display);

    // auto mouse = lv_sdl_mouse_create();
    // lv_indev_set_group(mouse, lv_group_get_default());
    // lv_indev_set_display(mouse, display);

    // LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
    // lv_obj_t* cursor_obj;
    // cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
    // lv_image_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
    // lv_indev_set_cursor(mouse, cursor_obj);           /*Connect the image  object to the driver*/

    // auto mouse_wheel = lv_sdl_mousewheel_create();
    // lv_indev_set_display(mouse_wheel, display);
    // lv_indev_set_group(mouse_wheel, lv_group_get_default());

    // auto keyboard = lv_sdl_keyboard_create();
    // lv_indev_set_display(keyboard, display);
    // lv_indev_set_group(keyboard, lv_group_get_default());
}
