/**
 * @file button.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "button.h"
#include "../../hal_config.h"
#include <Arduino.h>
#include <mooncake_log.h>

void ButtonArduino::init()
{
    mclog::tagInfo("btn", "init");
    pinMode(HAL_PIN_BTN_PWR, INPUT_PULLUP);
    pinMode(HAL_PIN_BTN_UP, INPUT_PULLUP);
    pinMode(HAL_PIN_BTN_OK, INPUT_PULLUP);
    pinMode(HAL_PIN_BTN_DOWN, INPUT_PULLUP);
}

bool ButtonArduino::getButton(ButtonId::ButtonId_t id)
{
    switch (id) {
        case ButtonId::Power:
            return digitalRead(HAL_PIN_BTN_PWR) == 0;
        case ButtonId::Up:
            return digitalRead(HAL_PIN_BTN_UP) == 0;
        case ButtonId::Ok:
            return digitalRead(HAL_PIN_BTN_OK) == 0;
        case ButtonId::Down:
            return digitalRead(HAL_PIN_BTN_DOWN) == 0;
        default:
            return false;
    }
    return false;
}

std::uint32_t ButtonArduino::millis()
{
    return ::millis();
}
