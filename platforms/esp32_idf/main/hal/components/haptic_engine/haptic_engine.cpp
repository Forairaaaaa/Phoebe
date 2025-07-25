/**
 * @file haptic_engine.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-11
 *
 * @copyright Copyright (c) 2024
 *
 */
// https://github.com/adafruit/Adafruit_DRV2605_Library/blob/master/examples/basic/basic.ino
#include "haptic_engine.h"
#include "../../hal_config.h"
#include <Arduino.h>
#include <mooncake_log.h>

static const char* _tag = "haptic";

HapticEngineDRV2605L::HapticEngineDRV2605L()
{
    _drv2605 = new Adafruit_DRV2605;
}

HapticEngineDRV2605L::~HapticEngineDRV2605L()
{
    delete _drv2605;
}

bool HapticEngineDRV2605L::init()
{
    mclog::tagInfo(_tag, "dev2605 init");

    // 配置使能脚，默认使能
    pinMode(HAL_PIN_HAPTIC_EN, OUTPUT);
    enable();
    delay(5);

    // 初始化驱动
    _drv2605->init(HAL_I2C_BUS_PORT_NUM);
    _drv2605->selectLibrary(6);
    _drv2605->setMode(DRV2605_MODE_INTTRIG);

    return true;
}

void HapticEngineDRV2605L::enable()
{
    mclog::tagInfo(_tag, "enable dev2605");
    digitalWrite(HAL_PIN_HAPTIC_EN, 1);
}

void HapticEngineDRV2605L::disable()
{
    mclog::tagInfo(_tag, "disable dev2605");
    digitalWrite(HAL_PIN_HAPTIC_EN, 0);
}

void HapticEngineDRV2605L::playEffect(const HapticEffect::HapticEffect_t& effect)
{
    _drv2605->setWaveform(0, static_cast<uint8_t>(effect));
    _drv2605->setWaveform(1, 0);
    _drv2605->go();
}

void HapticEngineDRV2605L::playEffects(const std::vector<HapticEffect::HapticEffect_t>& effectSequence)
{
    if (effectSequence.empty()) {
        mclog::tagError(_tag, "empty effect");
        return;
    }

    int count = 0;
    for (const auto& effect : effectSequence) {
        _drv2605->setWaveform(count, static_cast<uint8_t>(effect));
        count++;
        if (count > 7) {
            break;
        }
    }
    _drv2605->setWaveform(count, 0);
    _drv2605->go();
}
