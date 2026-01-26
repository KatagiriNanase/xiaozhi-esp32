#pragma once
#include "app_base.hpp"


class SettingApp :public App {
public:
    void OnCreate() override;
    void OnResume() override;
    void OnPause() override;

    std::string GetAppName() const { return name_; };
    const lv_img_dsc_t* GetAppIcon() const { return &icon_; };  // Font Awesome icon
private:
    std::string name_ = std::string("Setting");
    lv_img_dsc_t icon_;
    lv_obj_t* btn_ = nullptr;
};