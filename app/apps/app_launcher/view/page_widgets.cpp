/**
 * @file page_widgets.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "page.h"
#include "../../utils/launcher_widget/launcher_widget.h"
#include <hal/hal.h>
#include <memory>
#include <mooncake.h>
#include <mooncake_log.h>

using namespace mooncake;
using namespace SmoothUIToolKit;
using namespace smooth_widget;

static const char* _tag = "PageWidgets";

static constexpr int _canvas_start_up_x = 150;
static constexpr int _canvas_start_up_y = 100;
static constexpr int _canvas_start_up_w = 20;
static constexpr int _canvas_start_up_h = 40;
static constexpr int _canvas_x = 10;
static constexpr int _canvas_y = 10;
static constexpr int _canvas_w = 124;
static constexpr int _canvas_h = 68;

void LauncherPageWidgets::onCreate()
{
    _canvas_list.resize(2);
}

void LauncherPageWidgets::onShow()
{
    mclog::tagInfo(_tag, "on show");
    handle_show_widget_canvas();
}

void LauncherPageWidgets::onForeground()
{
    if (isOnSubPage()) {
        handle_sub_page_input();
    }

    for (auto& canvas : _canvas_list) {
        if (canvas) {
            canvas->updateSmoothing();
        }
    }

    if (_mouse) {
        _mouse->updateSmoothing();
        if (!isOnSubPage()) {
            if (_mouse->isAllSmoothingFinish()) {
                _mouse.reset();
                // mclog::tagInfo(_tag, "free shit");
            }
        }
    }
}

void LauncherPageWidgets::onBackground()
{
    int i = 0;
    for (auto& canvas : _canvas_list) {
        if (canvas) {
            canvas->updateSmoothing();
            if (canvas->isAllSmoothingFinish()) {
                canvas.reset();
                // mclog::tagInfo(_tag, "free shit");
                handle_destroy_launcher_widget(i);
            }
        }
        i++;
    }

    if (_mouse) {
        _mouse->updateSmoothing();
        if (_mouse->isAllSmoothingFinish()) {
            _mouse.reset();
            // mclog::tagInfo(_tag, "free shit");
        }
    }
}

void LauncherPageWidgets::onHide()
{
    mclog::tagInfo(_tag, "on hide");
    handle_hide_widget_canvas();
}

void LauncherPageWidgets::onEnterSubPage()
{
    if (!_mouse) {
        _mouse = std::make_unique<SmoothWidgetMouse>(lv_screen_active());
    }

    _mouse->clearAllTargets();
    _mouse->addTarget(_canvas_list[0].get());
    _mouse->addTarget(_canvas_list[1].get());
    _mouse->show();
}

void LauncherPageWidgets::onQuitSubPage()
{
    _mouse->hide();
}

void LauncherPageWidgets::handle_show_widget_canvas()
{
    int i = 0;
    for (auto& canvas : _canvas_list) {
        if (!canvas) {
            canvas = std::make_unique<SmoothWidgetBase>(lv_screen_active());
            canvas->setRadius(16);
            canvas->smoothPosition().setTransitionPath(EasingPath::easeOutQuad);
            canvas->smoothPosition().jumpTo(_canvas_start_up_x, _canvas_start_up_y);
            canvas->smoothSize().setTransitionPath(EasingPath::easeOutBack);
            canvas->smoothSize().jumpTo(_canvas_start_up_w, _canvas_start_up_h);

            handle_create_launcher_widget(i);
        }

        canvas->smoothPosition().setDelay(i * 100);
        canvas->smoothPosition().setDuration(300);
        canvas->smoothPosition().moveTo(_canvas_x, _canvas_y + i * 80);
        canvas->smoothSize().setDelay(i * 100 + 100);
        canvas->smoothSize().setDuration(800);
        canvas->smoothSize().moveTo(_canvas_w, _canvas_h);

        i++;
    }
}

void LauncherPageWidgets::handle_hide_widget_canvas()
{
    int i = 0;
    for (auto& canvas : _canvas_list) {
        canvas->smoothPosition().setDelay((_canvas_list.size() - 1 - i) * 50 + 100);
        canvas->smoothPosition().setDuration(200);
        canvas->smoothPosition().moveTo(_canvas_start_up_x, _canvas_start_up_y);
        canvas->smoothSize().setDelay((_canvas_list.size() - 1 - i) * 100);
        canvas->smoothSize().setDuration(600);
        canvas->smoothSize().moveTo(_canvas_start_up_w, _canvas_start_up_h);

        i++;
    }
}

void LauncherPageWidgets::handle_sub_page_input()
{
    if (HAL::BtnUp().wasClicked()) {
        _mouse->goLast();
    }

    if (HAL::BtnDown().wasClicked()) {
        _mouse->goNext();
    }

    if (HAL::BtnOk().wasPressed()) {
        _mouse->press();
    }

    if (HAL::BtnOk().wasReleased()) {
        _mouse->release();
    }

    if (HAL::BtnPower().wasClicked()) {
        quitSubPage();
    }
}

void LauncherPageWidgets::handle_create_launcher_widget(int canvasIndex)
{
    // TODO
    // get type by config
    if (canvasIndex == 0) {
        _widget_a_ability_id = launcher_widget_factory::create(_canvas_list[canvasIndex].get(), "time");
    } else if (canvasIndex == 1) {
        _widget_b_ability_id = launcher_widget_factory::create(_canvas_list[canvasIndex].get(), "date");
    }
}

void LauncherPageWidgets::handle_destroy_launcher_widget(int canvasIndex)
{
    if (canvasIndex == 0) {
        GetMooncake().destroyExtension(_widget_a_ability_id);
    } else if (canvasIndex == 1) {
        GetMooncake().destroyExtension(_widget_b_ability_id);
    }
}
