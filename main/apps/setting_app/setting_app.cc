#include "setting_app.hpp"
#include "context.hpp"
#include "esp_lib_utils.h"
#include "gui/lv_lock.hpp"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "setting_app"

void SettingApp::OnCreate()
{
    ESP_UTILS_LOGI("OnCreate!");
}

void SettingApp::OnResume()
{
    LvLockGuard lock;
    btn_= lv_button_create(lv_scr_act());
    lv_obj_set_size(btn_, 100, 100);
    lv_obj_center(btn_);
    lv_obj_add_event_cb(btn_, [](lv_event_t* e) {
        auto* setting_app = static_cast<SettingApp*>(lv_event_get_user_data(e));
        auto* btn = lv_event_get_target_obj(e);
        setting_app->_system_context->getEvent().sendEvent(btn, e);
        }, LV_EVENT_CLICKED, this);

    auto& event = _system_context->getEvent();
    event.connectEventSignal(btn_, [](lv_event_t* e) {
        static int count = 0;
        ESP_UTILS_LOGI("Pressed[%d]!", count++);
        });
}

void SettingApp::OnPause()
{
    _system_context->getEvent().unregisterEvent(btn_);

    LvLockGuard lock;
    lv_obj_del(btn_);
    btn_ = nullptr;
}
