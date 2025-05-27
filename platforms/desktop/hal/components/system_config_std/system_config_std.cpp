/**
 * @file system_config_std.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-10-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "system_config_std.h"
#include "fmt/base.h"
#include <cstdio>
#include <mooncake_log.h>
#include <ArduinoJson.h>
#include <string>

static const char* _tag = "syscfg";

SystemConfigStd::SystemConfigStd(const std::string& rootPath)
{
    _system_config_path = rootPath + "system_config.json";
}

bool SystemConfigStd::loadConfig()
{
    mclog::tagInfo(_tag, "load config from fs");

    // 打开配置文件
    FILE* config_file = fopen(_system_config_path.c_str(), "rb");
    if (config_file == NULL) {
        // 如果没有，新建一个
        mclog::warn("{} not exist, try creating", _system_config_path);
        saveConfig();
        backup_config_file();
        return true;
    }

    // 读取文件内容
    char* file_content = 0;
    long file_length = 0;
    fseek(config_file, 0, SEEK_END);
    file_length = ftell(config_file);
    fseek(config_file, 0, SEEK_SET);
    file_content = (char*)malloc(file_length);

    // 如果内存申请失败（通常是文件损坏导致的文件大小问题），尝试读取备份文件
    if (!file_content) {
        fclose(config_file);

        mclog::error("malloc failed, size: {}", file_length);

        std::string backup_path = _system_config_path + ".bk";
        mclog::tagInfo(_tag, "try {}", backup_path);

        config_file = fopen(backup_path.c_str(), "rb");

        // 如果备份打开失败，重建所有
        if (config_file == NULL) {
            mclog::error("open backup failed, try recreating..");
            saveConfig();
            backup_config_file();
            return true;
        }
    } else {
        fread(file_content, 1, file_length, config_file);
        fclose(config_file);
    }

    // 解析 json 内容并保存到当前配置
    if (!parse_json_and_copy_config(file_content)) {
        // 如果解析失败，则重建
        mclog::tagInfo(_tag, " try recreating..");
        saveConfig();
        backup_config_file();
    }

    free(file_content);
    return true;
}

bool SystemConfigStd::saveConfig()
{
    mclog::tagInfo(_tag, "save config to fs");

    // 备份原来的
    backup_config_file();

    // 将当前配置序列化 json
    std::string json_content = create_config_json();

    // 打开配置文件并写入
    mclog::tagInfo(_tag, "open {}", _system_config_path);
    FILE* config_file = fopen(_system_config_path.c_str(), "wb");
    if (config_file == NULL) {
        mclog::error("open failed");
        return false;
    }

    fputs(json_content.c_str(), config_file);
    fclose(config_file);

    mclog::tagInfo(_tag, "config saved at {}", _system_config_path);
    return true;
}

std::string SystemConfigStd::create_config_json()
{
    JsonDocument doc;

    // 复制配置
    doc["mute"] = _config.mute;
    doc["hapticFeedback"] = _config.hapticFeedback;
    doc["watchFace"] = _config.watchFace;
    doc["widgetA"] = _config.widgetA;
    doc["widgetB"] = _config.widgetB;

    // 序列化 json
    std::string json_content;
    if (serializeJson(doc, json_content) == 0) {
        json_content.clear();
        mclog::error("serialize failed");
    }

    return json_content;
}

bool SystemConfigStd::parse_json_and_copy_config(char* jsonContent)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonContent);
    if (error != DeserializationError::Ok) {
        mclog::error("parse json failed");
        return false;
    }

    // 复制配置
    _config.mute = doc["mute"];
    _config.hapticFeedback = doc["hapticFeedback"];
    _config.watchFace = doc["watchFace"].as<std::string>();
    _config.widgetA = doc["widgetA"].as<std::string>();
    _config.widgetB = doc["widgetB"].as<std::string>();

    return true;
}

void SystemConfigStd::backup_config_file()
{
    mclog::tagInfo(_tag, "create config backup");

    // 打开配置文件
    mclog::tagInfo(_tag, "try open {}", _system_config_path);
    FILE* config_file = fopen(_system_config_path.c_str(), "rb");
    if (config_file == NULL) {
        mclog::error("open failed");
        return;
    }

    // 创建备份配置文件
    std::string backup_path = _system_config_path + ".bk";
    mclog::tagInfo(_tag, "try open {}", backup_path);
    FILE* config_backup_file = fopen(backup_path.c_str(), "wb");
    if (config_backup_file == NULL) {
        mclog::error("open failed");
        return;
    }

    // 复制
    char* buffer = new char[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, 1024, config_file)) > 0) {
        fwrite(buffer, 1, bytesRead, config_backup_file);
    }
    delete[] buffer;

    fclose(config_file);
    fclose(config_backup_file);
}

void SystemConfigStd::logConfig()
{
    mclog::tagInfo(_tag, "current system config:");
    fmt::println("mute: {}", _config.mute);
    fmt::println("hapticFeedback: {}", _config.hapticFeedback);
    fmt::println("watchFace: {}", _config.watchFace);
    fmt::println("widgetA: {}", _config.widgetA);
    fmt::println("widgetB: {}", _config.widgetB);
}
