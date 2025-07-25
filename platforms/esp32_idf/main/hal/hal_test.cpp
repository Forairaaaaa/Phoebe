/**
 * @file hal_test.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include "hal/hal.h"
#include "hal_esp32.h"
#include "hal_config.h"
#include <mooncake_log.h>
#include <Arduino.h>
#include <lvgl.h>
#include "components/utils/Adafruit_DRV2605/Adafruit_DRV2605.h"
#include "components/utils/Adafruit_MAX1704X/Adafruit_MAX1704X.h"

void HalEsp32::hal_test()
{
    /* ---------------------------------- Test ---------------------------------- */
    // imu_test();
    // buzzer_test();
    // haptic_test();
    // haptic_engine_test();
    // max1704_test();
    // battery_monitor_test();
}

void HalEsp32::imu_test()
{
    while (1) {
        HAL::Imu().update();
        mclog::info("{:0.2f}\t{:0.2f}\t{:0.2f}\t|\t{:0.2f}\t{:0.2f}\t{:0.2f}", HAL::Imu().getData().accelX,
                    HAL::Imu().getData().accelY, HAL::Imu().getData().accelZ, HAL::Imu().getData().gyroX,
                    HAL::Imu().getData().gyroY, HAL::Imu().getData().gyroZ);

        delay(50);
        HAL::SysCtrl().feedTheDog();
    }
}

void HalEsp32::buzzer_test()
{
    while (1) {
        mclog::info("is playing: {}", HAL::Buzzer().isPlaying());
        HAL::Buzzer().playRtttlMusic("NokiaTun:d=4,o=5,b=225:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a");

        while (HAL::Buzzer().isPlaying()) {
            mclog::info("is playing: {}", HAL::Buzzer().isPlaying());
            HAL::SysCtrl().feedTheDog();
            delay(1000);
        }

        // 打断测试
        int interval = 2000;
        while (interval >= 500) {
            HAL::Buzzer().playRtttlMusic("NokiaTun:d=4,o=5,b=225:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a");

            mclog::info("delay {}", interval);
            delay(interval);
            interval -= 500;
            HAL::SysCtrl().feedTheDog();
        }

        interval = 500;
        while (interval >= 20) {
            HAL::Buzzer().playRtttlMusic("NokiaTun:d=4,o=5,b=225:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a");

            mclog::info("delay {}", interval);
            delay(interval);
            interval -= 20;
            HAL::SysCtrl().feedTheDog();
        }
    }
}

void HalEsp32::haptic_test()
{
    // https://github.com/adafruit/Adafruit_DRV2605_Library/blob/master/examples/basic/basic.ino

    pinMode(HAL_PIN_HAPTIC_EN, OUTPUT);
    digitalWrite(HAL_PIN_HAPTIC_EN, 1);
    delay(100);

    Adafruit_DRV2605 shit;
    shit.init(HAL_I2C_BUS_PORT_NUM);
    shit.selectLibrary(6);
    shit.setMode(DRV2605_MODE_INTTRIG);

    uint8_t effect = 1;
    while (1) {
        // set the effect to play
        shit.setWaveform(0, effect); // play effect
        shit.setWaveform(1, 0);      // end waveform

        // play the effect!
        mclog::info("effect: {}", effect);
        shit.go();

        // wait a bit
        delay(1000);

        effect++;
        if (effect > 117) {
            effect = 1;
        }

        HAL::SysCtrl().feedTheDog();
    }
}

void HalEsp32::haptic_engine_test()
{
    // while (1) {
    //     HAL::SysCtrl().feedTheDog();
    //     HAL::HapticEngine().playEffect(HapticEffect::StrongClick100);
    //     delay(1000);
    // }

    uint8_t effect = 1;
    while (1) {
        mclog::info("effect: {}", effect);
        HAL::HapticEngine().playEffect(static_cast<HapticEffect::HapticEffect_t>(effect));

        delay(1000);

        effect++;
        if (effect > 117) {
            effect = 1;
        }

        HAL::SysCtrl().feedTheDog();
    }
}

void HalEsp32::max1704_test()
{
    Adafruit_MAX17048 shit;
    mclog::info("init: {}", shit.begin(HAL_I2C_BUS_PORT_NUM));
    mclog::info("chip id: 0x{:02X}", shit.getChipID());

    // 创建一个标签用于显示电压和电量
    lv_obj_t* label = lv_label_create(lv_scr_act()); // 在当前活动屏幕上创建标签
    lv_label_set_text(label, "Initializing...");     // 设置初始文本
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);    // 将标签对齐到屏幕顶部中间位置

    pinMode(HAL_PIN_PWR_IS_USB_IN, INPUT);

    while (1) {
        // 获取电压和电量百分比
        float voltage = shit.cellVoltage();
        float percentage = shit.cellPercent();
        bool usb_in = digitalRead(HAL_PIN_PWR_IS_USB_IN) == 1;

        // 打印到控制台
        mclog::info("v: {:.3f}v p: {:.1f}% usb in: {}", voltage, percentage, usb_in);

        // 更新标签内容
        lv_label_set_text(label,
                          fmt::format("v: {:.3f}v\np: {:.1f}%\nusb in: {}", voltage, percentage, usb_in).c_str());

        // 喂狗（防止看门狗复位）
        HAL::SysCtrl().feedTheDog();

        // 刷新 LVGL
        lv_timer_handler(); // 刷新 LVGL 的任务处理
        delay(1000);
    }
}

void HalEsp32::battery_monitor_test()
{
    // 创建一个标签用于显示电压和电量
    lv_obj_t* label = lv_label_create(lv_scr_act()); // 在当前活动屏幕上创建标签
    lv_label_set_text(label, "Initializing...");     // 设置初始文本
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);    // 将标签对齐到屏幕顶部中间位置

    while (1) {
        // 获取电压和电量百分比
        float voltage = HAL::BatteryMonitor().voltage();
        float percentage = HAL::BatteryMonitor().percent();
        int state = (int)HAL::BatteryMonitor().state();

        // 打印到控制台
        mclog::info("v: {:.3f}v p: {:.1f}% state: {}", voltage, percentage, state);

        // 更新标签内容
        lv_label_set_text(label, fmt::format("v: {:.3f}v\np: {:.1f}%\nstate: {}", voltage, percentage, state).c_str());

        // 喂狗（防止看门狗复位）
        HAL::SysCtrl().feedTheDog();

        // 刷新 LVGL
        lv_timer_handler(); // 刷新 LVGL 的任务处理
        delay(500);
    }
}
