/**
 * @file app_template.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date <date></date>
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "app_template.h"
#include "hal/hal.h"
#include <algorithm>
#include <anim/lv_example_anim.h>
#include <benchmark/lv_demo_benchmark.h>
#include <cstdint>
#include <mooncake.h>
#include <mooncake_log.h>
#include <lvgl.h>
#include <random>
#include <src/core/lv_obj_pos.h>
#include <src/core/lv_obj_style.h>
#include <src/core/lv_obj_style_gen.h>
#include <src/display/lv_display.h>
#include <src/misc/lv_area.h>
#include <src/misc/lv_color.h>
#include <src/misc/lv_style_gen.h>
#include <src/misc/lv_timer.h>
#include <lv_demos.h>
#include <src/themes/mono/lv_theme_mono.h>
#include <src/widgets/canvas/lv_canvas.h>
#include <stress/lv_demo_stress.h>
#include <widgets/lv_demo_widgets.h>
#include <widgets/lv_example_widgets.h>

using namespace mooncake;

AppTemplate::AppTemplate()
{
    // 配置 App 信息
    setAppInfo().name = "AppTemplate";
}

void AppTemplate::onCreate()
{
    mclog::tagInfo(getAppInfo().name, "on create");

    // 打开自己
    open();
}

static lv_obj_t* canvas;
static uint8_t* canvas_buffer;

#define COLOR_HIGH lv_color_white()
#define COLOR_LOW  lv_color_black()
// #define COLOR_HIGH lv_color_black()
// #define COLOR_LOW  lv_color_white()

const char* phrases[9] = {"FUCK  AROUND FUCK OFF WHAT THE FUCK?   FUCK UP FUCK YOU",
                          "WHAT THE FUCK?  FUCK YOU FUCK OFF FUCK UP FUCK AROUND",
                          "FUCK  YOU  FUCK OFF WHAT THE  FUCK?  FUCK AROUND FUCK UP",
                          "FUCK OFF FUCK UP WHAT  THE FUCK?   FUCK  AROUND FUCK YOU",
                          "FUCK  UP FUCK  YOU FUCK  AROUND WHAT THE FUCK? FUCK OFF",
                          "WHAT THE FUCK? FUCK AROUND   FUCK OFF FUCK  YOU FUCK UP",
                          "FUCK  OFF FUCK AROUND WHAT THE FUCK? FUCK YOU FUCK UP",
                          "FUCK YOU   WHAT  THE FUCK? FUCK   OFF FUCK AROUND FUCK UP",
                          "FUCK  AROUND   WHAT  THE FUCK? FUCK UP   FUCK OFF FUCK YOU"};

void AppTemplate::onOpen()
{
    mclog::tagInfo(getAppInfo().name, "on open");

    lv_theme_t* theme_mono = lv_theme_mono_init(lv_display_get_default(), false, &lv_font_montserrat_14);
    lv_display_set_theme(lv_display_get_default(), theme_mono);

    // lv_demo_widgets();
    // lv_demo_stress();
    // lv_demo_benchmark();

    // lv_example_switch_1();
    // lv_example_label_1();
    // lv_example_anim_2();

    lv_obj_set_style_bg_color(lv_screen_active(), COLOR_LOW, 0);

    canvas_buffer = new uint8_t[136 * 136 * 2]{0};
    canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_buffer(canvas, canvas_buffer, 136, 136, LV_COLOR_FORMAT_RGB565);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);

    for (int i = 0; i < 9; i++) {
        lv_obj_t* label;
        label = lv_label_create(lv_screen_active());
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
        lv_obj_set_width(label, 160);
        lv_label_set_text(label, phrases[i]);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, -5 + i * 20);
        lv_obj_set_style_text_color(label, COLOR_HIGH, 0);
    }
}

void AppTemplate::onRunning()
{
    update_clock();
    lv_timer_handler();

    HAL::BtnUpdate();
    if (HAL::BtnPower().wasClicked()) {
        mclog::info("??");
    }
    if (HAL::BtnPower().wasDoubleClicked()) {
        mclog::info("bye");
        HAL::SysCtrl().powerOff();
    }
    if (HAL::BtnPower().wasHold()) {
        mclog::info("jijiji");
    }
}

void AppTemplate::onClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");
}

void AppTemplate::update_clock()
{
    if (HAL::SysCtrl().millis() - _clock_time_count < 1000) {
        return;
    }

    mclog::info("hi");

    lv_canvas_fill_bg(canvas, COLOR_LOW, LV_OPA_COVER);

    time_t now;
    struct tm* tm_info;
    time(&now);
    tm_info = localtime(&now);

    int hour = tm_info->tm_hour % 12; // 12小时制
    int minute = tm_info->tm_min;
    int second = tm_info->tm_sec;

    // 计算针的位置
    float hour_angle = (hour + minute / 60.0) * 30 * (3.14159 / 180);    // 360 / 12 = 30
    float minute_angle = (minute + second / 60.0) * 6 * (3.14159 / 180); // 360 / 60 = 6
    float second_angle = second * 6 * (3.14159 / 180);                   // 360 / 60 = 6

    // 中心点
    int center_x = 136 / 2;
    int center_y = 136 / 2;

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_line_dsc_t dsc;
    lv_draw_line_dsc_init(&dsc);

    dsc.color = COLOR_HIGH;
    dsc.width = 8;
    dsc.round_end = 0;
    dsc.round_start = 0;
    dsc.p1.x = center_x;
    dsc.p1.y = center_y;
    dsc.p2.x = center_x + (int)(32 * cos(hour_angle - 1.5708));
    dsc.p2.y = center_y + (int)(32 * sin(hour_angle - 1.5708));
    lv_draw_line(&layer, &dsc);

    dsc.width = 4;
    dsc.round_end = 0;
    dsc.round_start = 0;
    dsc.p1.x = center_x;
    dsc.p1.y = center_y;
    dsc.p2.x = center_x + (int)(50 * cos(minute_angle - 1.5708));
    dsc.p2.y = center_y + (int)(50 * sin(minute_angle - 1.5708));
    lv_draw_line(&layer, &dsc);

    dsc.width = 3;
    dsc.round_end = 0;
    dsc.round_start = 0;
    dsc.p1.x = center_x;
    dsc.p1.y = center_y;
    dsc.p2.x = center_x + (int)(90 * cos(second_angle - 1.5708));
    dsc.p2.y = center_y + (int)(90 * sin(second_angle - 1.5708));
    lv_draw_line(&layer, &dsc);

    lv_canvas_finish_layer(canvas, &layer);

    _clock_time_count = HAL::SysCtrl().millis();
}
