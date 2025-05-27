/**
 * @file factory.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "widget.h"
#include <cstring>
#include <memory>
#include <mooncake_log.h>

using namespace widget;
using namespace widget_helper;

int WidgetFactory::create(const char* widgetType)
{
    if (_widget_parent == NULL) {
        mclog::tagWarn("WidgetFactory", "empty widget parent, use lv_screen_active()");
        _widget_parent = lv_screen_active();
    }

    WidgetInfo_t new_widget_info;
    new_widget_info.id = -1;
    new_widget_info.widget.reset();

    // 创建实例
    if (strcmp(widgetType, "base") == 0) {
        new_widget_info.widget = std::make_unique<WidgetBase>(_widget_parent);
    } else if (strcmp(widgetType, "label") == 0) {
        new_widget_info.widget = std::make_unique<WidgetLabel>(_widget_parent);
    } else if (strcmp(widgetType, "img") == 0) {
        new_widget_info.widget = std::make_unique<WidgetImg>(_widget_parent);
    } else if (strcmp(widgetType, "clock") == 0) {
        new_widget_info.widget = std::make_unique<WidgetClock>(_widget_parent);
    }

    if (new_widget_info.widget) {
        // 分配 ID，转移所有权
        new_widget_info.id = get_next_widget_id();
        _widget_list.push_back(std::move(new_widget_info));
    }

    return new_widget_info.id;
}

void WidgetFactory::destory(int widgetId)
{
    // 遍历查找对应 ID 的组件
    for (auto widget_iter = _widget_list.begin(); widget_iter != _widget_list.end(); widget_iter++) {
        if (widget_iter->id == widgetId) {
            // 把 ID 存到可用 ID 列表，防止反复创建销毁导致的 ID 溢出
            _available_widget_id_list.push_back(widget_iter->id);
            // 销毁 widget
            widget_iter = _widget_list.erase(widget_iter);
            break;
        }
    }
}

WidgetBase* WidgetFactory::getBase(int widgetId)
{
    // 遍历查找对应 ID 的 widget
    for (auto& widget_info : _widget_list) {
        if (widget_info.id == widgetId) {
            return widget_info.widget.get();
        }
    }
    return nullptr;
}

WidgetLabel* WidgetFactory::getLabel(int widgetId)
{
    auto widget = getBase(widgetId);
    if (!widget) {
        return nullptr;
    }
    if (widget->type() != WidgetType::Label) {
        return nullptr;
    }
    return static_cast<WidgetLabel*>(widget);
}

WidgetImg* WidgetFactory::getImg(int widgetId)
{
    auto widget = getBase(widgetId);
    if (!widget) {
        return nullptr;
    }
    if (widget->type() != WidgetType::Img) {
        return nullptr;
    }
    return static_cast<WidgetImg*>(widget);
}

WidgetClock* WidgetFactory::getClock(int widgetId)
{
    auto widget = getBase(widgetId);
    if (!widget) {
        return nullptr;
    }
    if (widget->type() != WidgetType::Clock) {
        return nullptr;
    }
    return static_cast<WidgetClock*>(widget);
}

int WidgetFactory::get_next_widget_id()
{
    int next_widget_id = -1;

    // 检查可用 ID 列表
    if (!_available_widget_id_list.empty()) {
        next_widget_id = _available_widget_id_list.front();
        _available_widget_id_list.erase(_available_widget_id_list.begin());
        return next_widget_id;
    }

    // 如果没有，继续自加
    next_widget_id = _next_widget_id;
    _next_widget_id++;

    return next_widget_id;
}
