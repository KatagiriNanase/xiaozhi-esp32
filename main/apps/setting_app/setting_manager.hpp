#pragma once
#include "setting_ui.hpp"

class SettingManager {
public:
    SettingManager(SettingUI& ui);

    void init();
    void run();
    void del();
private:
    SettingUI& ui_;
};