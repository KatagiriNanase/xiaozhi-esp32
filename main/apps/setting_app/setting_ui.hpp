#pragma once
#include <lvgl.h>

#include "screens/settings.hpp"
#include "screens/sound.hpp"
#include "screens/display.hpp"

class SettingManager;

class SettingUI {
public:
    void setup();
    void del();
    lv_obj_t* getObj() { return main_screen_; };
private:
    friend class SettingManager;
    lv_obj_t* main_screen_ = nullptr;
    lv_obj_t* btn_ = nullptr;
    ScreenSettings screen_settings_;
    ScreenSound screen_sound_;
    ScreenDisplay screen_display_;
};