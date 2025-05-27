/**
 * @file duk_hal.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "binding.h"
#include <mooncake_log.h>
#include <hal/hal.h>

static const std::string _tag = "JS";

/* -------------------------------------------------------------------------- */
/*                               System control                               */
/* -------------------------------------------------------------------------- */
static duk_ret_t _hal_sysCtrl_delay(duk_context* ctx)
{
    std::uint32_t ms = (std::uint32_t)duk_to_uint(ctx, 0);
    HAL::SysCtrl().delay(ms);
    return 0;
}

static duk_ret_t _hal_sysCtrl_millis(duk_context* ctx)
{
    std::uint32_t ms = HAL::SysCtrl().millis();
    duk_push_uint(ctx, ms);
    return 1;
}

static duk_ret_t _hal_sysCtrl_reboot(duk_context* ctx)
{
    HAL::SysCtrl().reboot();
    return 0;
}

static duk_ret_t _hal_sysCtrl_powerOff(duk_context* ctx)
{
    HAL::SysCtrl().powerOff();
    return 0;
}

static duk_ret_t _hal_sysCtrl_feedTheDog(duk_context* ctx)
{
    HAL::SysCtrl().feedTheDog();
    return 0;
}

static duk_ret_t _hal_sysCtrl_freeHeapSize(duk_context* ctx)
{
    size_t size = HAL::SysCtrl().freeHeapSize();
    duk_push_uint(ctx, (duk_uint_t)size);
    return 1;
}

static void _duk_hal_sysCtrl_init(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, _hal_sysCtrl_delay, 1);
    duk_put_prop_string(ctx, -2, "delay");
    duk_push_c_function(ctx, _hal_sysCtrl_millis, 0);
    duk_put_prop_string(ctx, -2, "millis");
    duk_push_c_function(ctx, _hal_sysCtrl_reboot, 0);
    duk_put_prop_string(ctx, -2, "reboot");
    duk_push_c_function(ctx, _hal_sysCtrl_powerOff, 0);
    duk_put_prop_string(ctx, -2, "powerOff");
    duk_push_c_function(ctx, _hal_sysCtrl_feedTheDog, 0);
    duk_put_prop_string(ctx, -2, "feedTheDog");
    duk_push_c_function(ctx, _hal_sysCtrl_freeHeapSize, 0);
    duk_put_prop_string(ctx, -2, "freeHeapSize");
    duk_put_prop_string(ctx, -2, "sysCtrl");
}

/* -------------------------------------------------------------------------- */
/*                                     IMU                                    */
/* -------------------------------------------------------------------------- */
static duk_ret_t _hal_imu_update(duk_context* ctx)
{
    HAL::Imu().update();
    return 0;
}

static duk_ret_t _hal_imu_getData(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_number(ctx, HAL::Imu().getData().accelX);
    duk_put_prop_string(ctx, -2, "accelX");
    duk_push_number(ctx, HAL::Imu().getData().accelY);
    duk_put_prop_string(ctx, -2, "accelY");
    duk_push_number(ctx, HAL::Imu().getData().accelZ);
    duk_put_prop_string(ctx, -2, "accelZ");
    duk_push_number(ctx, HAL::Imu().getData().gyroX);
    duk_put_prop_string(ctx, -2, "gyroX");
    duk_push_number(ctx, HAL::Imu().getData().gyroY);
    duk_put_prop_string(ctx, -2, "gyroY");
    duk_push_number(ctx, HAL::Imu().getData().gyroZ);
    duk_put_prop_string(ctx, -2, "gyroZ");
    duk_push_int(ctx, HAL::Imu().getData().steps);
    duk_put_prop_string(ctx, -2, "steps");

    return 1;
}

static void _duk_hal_imu_init(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, _hal_imu_update, 0);
    duk_put_prop_string(ctx, -2, "update");
    duk_push_c_function(ctx, _hal_imu_getData, 0);
    duk_put_prop_string(ctx, -2, "getData");
    duk_put_prop_string(ctx, -2, "imu");
}

/* -------------------------------------------------------------------------- */
/*                                   Buzzer                                   */
/* -------------------------------------------------------------------------- */
static duk_ret_t _hal_buzzer_beep(duk_context* ctx)
{
    float frequency = (float)duk_to_number(ctx, 0);
    std::uint32_t duration = (std::uint32_t)duk_opt_uint(ctx, 1, 0xFFFFFFFF);
    HAL::Buzzer().beep(frequency, duration);
    return 0;
}

static duk_ret_t _hal_buzzer_stop(duk_context* ctx)
{
    HAL::Buzzer().stop();
    return 0;
}

static duk_ret_t _hal_buzzer_playRtttlMusic(duk_context* ctx)
{
    const char* rtttlMusic = duk_to_string(ctx, 0);
    HAL::Buzzer().playRtttlMusic(rtttlMusic);
    return 0;
}

static duk_ret_t _hal_buzzer_isPlaying(duk_context* ctx)
{
    bool isPlaying = HAL::Buzzer().isPlaying();
    duk_push_boolean(ctx, isPlaying);
    return 1;
}

static void _duk_hal_buzzer_init(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, _hal_buzzer_beep, 2);
    duk_put_prop_string(ctx, -2, "beep");
    duk_push_c_function(ctx, _hal_buzzer_stop, 0);
    duk_put_prop_string(ctx, -2, "stop");
    duk_push_c_function(ctx, _hal_buzzer_playRtttlMusic, 1);
    duk_put_prop_string(ctx, -2, "playRtttlMusic");
    duk_push_c_function(ctx, _hal_buzzer_isPlaying, 0);
    duk_put_prop_string(ctx, -2, "isPlaying");
    duk_put_prop_string(ctx, -2, "buzzer");
}

/* -------------------------------------------------------------------------- */
/*                                Haptic Engine                               */
/* -------------------------------------------------------------------------- */
static duk_ret_t _hal_haptic_enable(duk_context* ctx)
{
    HAL::HapticEngine().enable();
    return 0;
}

static duk_ret_t _hal_haptic_disable(duk_context* ctx)
{
    HAL::HapticEngine().disable();
    return 0;
}

static duk_ret_t _hal_haptic_playEffect(duk_context* ctx)
{
    HAL::HapticEngine().playEffect(static_cast<HapticEffect::HapticEffect_t>(duk_to_uint(ctx, 0)));
    return 0;
}

static duk_ret_t _hal_haptic_playEffects(duk_context* ctx)
{
    duk_require_object_coercible(ctx, 0);
    std::vector<HapticEffect::HapticEffect_t> effectSequence;
    duk_enum(ctx, 0, DUK_ENUM_ARRAY_INDICES_ONLY);

    while (duk_next(ctx, -1, 1)) {
        effectSequence.push_back(static_cast<HapticEffect::HapticEffect_t>(duk_to_uint(ctx, -1)));
        duk_pop_2(ctx); // pop key and value
    }
    HAL::HapticEngine().playEffects(effectSequence);
    return 0;
}

static void _duk_hal_haptic_init(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, _hal_haptic_enable, 0);
    duk_put_prop_string(ctx, -2, "enable");
    duk_push_c_function(ctx, _hal_haptic_disable, 0);
    duk_put_prop_string(ctx, -2, "disable");
    duk_push_c_function(ctx, _hal_haptic_playEffect, 1);
    duk_put_prop_string(ctx, -2, "playEffect");
    duk_push_c_function(ctx, _hal_haptic_playEffects, 1);
    duk_put_prop_string(ctx, -2, "playEffects");
    duk_put_prop_string(ctx, -2, "haptic");
}

/* -------------------------------------------------------------------------- */
/*                               Battery Monitor                              */
/* -------------------------------------------------------------------------- */
static duk_ret_t _hal_battery_voltage(duk_context* ctx)
{
    float voltage = HAL::BatteryMonitor().voltage();
    duk_push_number(ctx, voltage);
    return 1;
}

static duk_ret_t _hal_battery_percent(duk_context* ctx)
{
    float percent = HAL::BatteryMonitor().percent();
    duk_push_number(ctx, percent);
    return 1;
}

static duk_ret_t _hal_battery_state(duk_context* ctx)
{
    auto bat_state = HAL::BatteryMonitor().state();
    switch (bat_state) {
        case BatteryState::NotConnected: {
            duk_push_string(ctx, "NC");
            break;
        }
        case BatteryState::Charging: {
            duk_push_string(ctx, "CHARGING");
            break;
        }
        case BatteryState::Normal: {
            duk_push_string(ctx, "NORMAL");
            break;
        }
        case BatteryState::Low: {
            duk_push_string(ctx, "LOW");
            break;
        }
        case BatteryState::Dead: {
            duk_push_string(ctx, "DEAD");
            break;
        }
        default: {
            duk_push_string(ctx, "?");
            break;
        }
    }
    return 1;
}

static void _duk_hal_battery_init(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_c_function(ctx, _hal_battery_voltage, 0);
    duk_put_prop_string(ctx, -2, "voltage");
    duk_push_c_function(ctx, _hal_battery_percent, 0);
    duk_put_prop_string(ctx, -2, "percent");
    duk_push_c_function(ctx, _hal_battery_state, 0);
    duk_put_prop_string(ctx, -2, "state");
    duk_put_prop_string(ctx, -2, "battery");
}

/* -------------------------------------------------------------------------- */
/*                                   Button                                   */
/* -------------------------------------------------------------------------- */
#define _BTN_ID_POWER 0
#define _BTN_ID_UP    1
#define _BTN_ID_OK    2
#define _BTN_ID_DOWN  3

static hal_components::Button_Class& _get_btn_by_id(duk_int_t buttonId)
{
    switch (buttonId) {
        case _BTN_ID_POWER: {
            return HAL::BtnPower();
        }
        case _BTN_ID_UP: {
            return HAL::BtnUp();
        }
        case _BTN_ID_OK: {
            return HAL::BtnOk();
        }
        case _BTN_ID_DOWN: {
            return HAL::BtnDown();
        }
        default: {
            mclog::tagError(_tag, "no btn id {}", (int)buttonId);
            return HAL::BtnOk();
        }
    }
}

#define _BTN_API_ID_WAS_CLICKED        0
#define _BTN_API_ID_WAS_DOUBLE_CLICKED 1
#define _BTN_API_ID_WAS_HOLD           2
#define _BTN_API_ID_IS_PRESSED         3
#define _BTN_API_ID_IS_HOLDING         4

static duk_ret_t _hal_btn_api(duk_context* ctx)
{
    // 取 magic flag 的十位为按钮 id，个位为 api id
    duk_int_t magic_flag = duk_get_current_magic(ctx);
    int button_id = magic_flag / 10;
    int api_id = magic_flag % 10;

    bool result = false;
    switch (api_id) {
        case _BTN_API_ID_WAS_CLICKED: {
            result = _get_btn_by_id(button_id).wasClicked();
            break;
        }
        case _BTN_API_ID_WAS_DOUBLE_CLICKED: {
            result = _get_btn_by_id(button_id).wasDoubleClicked();
            break;
        }
        case _BTN_API_ID_WAS_HOLD: {
            result = _get_btn_by_id(button_id).wasHold();
            break;
        }
        case _BTN_API_ID_IS_PRESSED: {
            result = _get_btn_by_id(button_id).isPressed();
            break;
        }
        case _BTN_API_ID_IS_HOLDING: {
            result = _get_btn_by_id(button_id).isHolding();
            break;
        }
        default: {
            mclog::tagError(_tag, "no btn api {}", (int)api_id);
        }
    }

    duk_push_boolean(ctx, result);
    return 1;
}

static duk_ret_t _hal_btn_update(duk_context* ctx)
{
    HAL::BtnUpdate();
    return 0;
}

static void _duk_add_hal_btn(duk_context* ctx, int buttonId, const char* buttonName)
{
    duk_push_object(ctx);

    duk_push_c_function(ctx, _hal_btn_api, 0);
    duk_set_magic(ctx, -1, buttonId * 10 + _BTN_API_ID_WAS_CLICKED);
    duk_put_prop_string(ctx, -2, "wasClicked");

    duk_push_c_function(ctx, _hal_btn_api, 0);
    duk_set_magic(ctx, -1, buttonId * 10 + _BTN_API_ID_WAS_DOUBLE_CLICKED);
    duk_put_prop_string(ctx, -2, "wasDoubleClicked");

    duk_push_c_function(ctx, _hal_btn_api, 0);
    duk_set_magic(ctx, -1, buttonId * 10 + _BTN_API_ID_WAS_HOLD);
    duk_put_prop_string(ctx, -2, "wasHold");

    duk_push_c_function(ctx, _hal_btn_api, 0);
    duk_set_magic(ctx, -1, buttonId * 10 + _BTN_API_ID_IS_PRESSED);
    duk_put_prop_string(ctx, -2, "isPressed");

    duk_push_c_function(ctx, _hal_btn_api, 0);
    duk_set_magic(ctx, -1, buttonId * 10 + _BTN_API_ID_IS_HOLDING);
    duk_put_prop_string(ctx, -2, "isHolding");

    duk_put_prop_string(ctx, -2, buttonName);
}

static void _duk_hal_btn_init(duk_context* ctx)
{
    duk_push_c_function(ctx, _hal_btn_update, 0);
    duk_put_prop_string(ctx, -2, "btnUpdate");

    _duk_add_hal_btn(ctx, _BTN_ID_POWER, "btnPower");
    _duk_add_hal_btn(ctx, _BTN_ID_UP, "btnUp");
    _duk_add_hal_btn(ctx, _BTN_ID_OK, "btnOk");
    _duk_add_hal_btn(ctx, _BTN_ID_DOWN, "btnDown");
}

void js_binding::duk_hal_init(duk_context* ctx)
{
    duk_push_object(ctx);
    _duk_hal_sysCtrl_init(ctx);
    _duk_hal_imu_init(ctx);
    _duk_hal_buzzer_init(ctx);
    _duk_hal_haptic_init(ctx);
    _duk_hal_battery_init(ctx);
    _duk_hal_btn_init(ctx);
    duk_put_global_string(ctx, "hal");
}
