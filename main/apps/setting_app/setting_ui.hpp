#pragma once
#include <lvgl.h>

#include "screens/settings.hpp"


class SettingUI {
public:
    void setup();
    void del();
    lv_obj_t* getObj() { return main_screen_; };
private:
    lv_obj_t* main_screen_ = nullptr;
    lv_obj_t* btn_ = nullptr;
    ScreenSettings screen_settings_;
};