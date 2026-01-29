#include "setting_app.hpp"
#include "context.hpp"
#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "setting_app"

SettingApp::SettingApp()
    :manager_(ui_)
{

}

void SettingApp::onCreate()
{
    ESP_UTILS_LOGI("onCreate!");
}

void SettingApp::onResume()
{
    // 主线程不加LvLock
    
    ui_.setup();
    
    // 独立线程加锁

}

void SettingApp::onPause()
{

}
