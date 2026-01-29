#pragma once
#include "setting_ui.hpp"

class SettingManager {
public:
    SettingManager(SettingUI& ui);
    
    void init();
    void run();
    void del();
private:
    void processSoundUI();
    SettingUI& ui_;
};