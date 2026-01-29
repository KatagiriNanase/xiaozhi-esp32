#pragma once
#include "app_base.hpp"
#include "setting_ui.hpp"   
#include "setting_manager.hpp"


class SettingApp :public App {
public:

    SettingApp();
    
    void onCreate() override;
    void onResume() override;
    void onPause() override;

    std::string getAppName() const { return name_; };
    const lv_img_dsc_t* getAppIcon() const { return &icon_; };  // Font Awesome icon
    lv_obj_t* getObj() { return ui_.getObj(); }
private:
    std::string name_ = std::string("Setting");
    lv_img_dsc_t icon_;

    SettingUI ui_;
    SettingManager manager_;
};