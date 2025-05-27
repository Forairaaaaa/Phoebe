/**
 * @file ble.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-11-22
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ble.h"
#include <mooncake_log.h>
#include <iostream>
#include <thread>
#include <cstdio>
#include <memory>
#include <shared/shared.h>

static const std::string _tag = "BlePython";

static void _ble_python_daemon()
{
    mclog::tagInfo(_tag, "start ble python daemon");

    // 创建管道
    std::string script_path = "../../platforms/desktop/hal/components/ble/desktop_ble_server.py";
    FILE* pipe = popen(("python3 -u " + script_path).c_str(), "r");
    if (!pipe) {
        mclog::tagError(_tag, "popen python script failed");
        return;
    }

    // 实时读取输出
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        mclog::tagInfo(_tag, "get msg: {}", buffer);
        SharedData::Borrow();
        SharedData::Ble().messageList.push_back(buffer);
        SharedData::Return();
    }

    // 关闭管道
    pclose(pipe);
    mclog::tagInfo(_tag, "ble python daemon stop");
}

void BlePython::init()
{
    mclog::tagInfo(_tag, "init");

    std::thread ble_python_daemon(_ble_python_daemon);
    ble_python_daemon.detach();
}
