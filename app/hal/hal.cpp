/**
 * @file hal.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "hal.h"
#include "hal/components/battery_monitor.h"
#include "hal/components/button.h"
#include <memory>
#include <string>
#include <mooncake_log.h>

/* -------------------------------------------------------------------------- */
/*                                  Singleton                                 */
/* -------------------------------------------------------------------------- */
// 提供一个可注入的全局单例

static std::unique_ptr<HAL::HalBase> _hal_instance;
static const std::string _tag = "HAL";

HAL::HalBase& HAL::Get()
{
    if (!_hal_instance) {
        mclog::tagWarn(_tag, "getting null hal, auto inject base");
        _hal_instance = std::make_unique<HalBase>();
    }
    return *_hal_instance.get();
}

void HAL::Inject(std::unique_ptr<HalBase> hal)
{
    if (!hal) {
        mclog::tagError(_tag, "pass null hal");
        return;
    }

    // 销毁已有实例，储存新实例
    Destroy();
    _hal_instance = std::move(hal);

    // 看看何方神圣
    mclog::tagInfo(_tag, "injecting hal type: {}", _hal_instance->type());

    // 初始化
    mclog::tagInfo(_tag, "invoke init");
    _hal_instance->init();
    mclog::tagInfo(_tag, "hal injected");
}

void HAL::Destroy()
{
    _hal_instance.reset();
}

/* -------------------------------------------------------------------------- */
/*                              Components Getter                             */
/* -------------------------------------------------------------------------- */
// 组件获取接口，如果当前没有实例，则懒加载一个基类，这样就算某个平台没有适配某个组件，也不会炸

hal_components::SystemControlBase& HAL::HalBase::SysCtrl()
{
    if (!_components.system_control) {
        mclog::tagWarn(_tag, "getting null sys ctrl component");
        _components.system_control = std::make_unique<hal_components::SystemControlBase>();
    }
    return *_components.system_control.get();
}

hal_components::ImuBase& HAL::HalBase::Imu()
{
    if (!_components.imu) {
        mclog::tagWarn(_tag, "getting null imu component");
        _components.imu = std::make_unique<hal_components::ImuBase>();
    }
    return *_components.imu.get();
}

hal_components::SystemConfigBase& HAL::HalBase::SysCfg()
{
    if (!_components.system_config) {
        mclog::tagWarn(_tag, "getting null sys cfg component");
        _components.system_config = std::make_unique<hal_components::SystemConfigBase>();
    }
    return *_components.system_config.get();
}

hal_components::BuzzerBase& HAL::HalBase::Buzzer()
{
    if (!_components.buzzer) {
        mclog::tagWarn(_tag, "getting null buzzer component");
        _components.buzzer = std::make_unique<hal_components::BuzzerBase>();
    }
    return *_components.buzzer.get();
}

hal_components::DisplayBase& HAL::HalBase::Display()
{
    if (!_components.display) {
        mclog::tagWarn(_tag, "getting null display component");
        _components.display = std::make_unique<hal_components::DisplayBase>();
    }
    return *_components.display.get();
}

hal_components::HapticEngineBase& HAL::HalBase::HapticEngine()
{
    if (!_components.haptic_engine) {
        mclog::tagWarn(_tag, "getting null haptic engine component");
        _components.haptic_engine = std::make_unique<hal_components::HapticEngineBase>();
    }
    return *_components.haptic_engine.get();
}

hal_components::BatteryMonitorBase& HAL::HalBase::BatteryMonitor()
{
    if (!_components.battery_monitor) {
        mclog::tagWarn(_tag, "getting null battery monitor component");
        _components.battery_monitor = std::make_unique<hal_components::BatteryMonitorBase>();
    }
    return *_components.battery_monitor.get();
}

hal_components::ButtonBase& HAL::HalBase::Button()
{
    if (!_components.button) {
        mclog::tagWarn(_tag, "getting null button component");
        _components.button = std::make_unique<hal_components::ButtonBase>();
    }
    return *_components.button.get();
}

hal_components::BleBase& HAL::HalBase::Ble()
{
    if (!_components.ble) {
        mclog::tagWarn(_tag, "getting null ble component");
        _components.ble = std::make_unique<hal_components::BleBase>();
    }
    return *_components.ble.get();
}
