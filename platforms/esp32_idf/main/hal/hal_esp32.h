/**
 * @file hal_esp32.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-09-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <hal/hal.h>

class HalEsp32 : public HAL::HalBase {
public:
    std::string type() override
    {
        return "ESP32";
    }

    void init() override
    {
        _display_init();
    }

private:
    void _display_init();
};