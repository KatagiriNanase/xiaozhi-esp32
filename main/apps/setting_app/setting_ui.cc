#include "setting_ui.hpp"
#include "setting_manager.hpp"
#include "context.hpp"
#include "gui/lv_lock.hpp"
#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "setting_ui"


void SettingUI::setup()
{
    screen_settings_.setupMain();
    screen_sound_.setupMain();
    screen_display_.setupMain();
    screen_wlan_.setupMain();
    main_screen_ = screen_settings_.getObj();
    lv_obj_center(main_screen_);
    lv_scr_load(main_screen_);
}

void SettingUI::del()
{
    screen_settings_.del();
}
