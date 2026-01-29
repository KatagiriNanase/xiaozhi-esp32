#include "setting_manager.hpp"
#include "screens/settings.hpp"
#include "services/context.hpp"

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "setting_manager"

SettingManager::SettingManager(SettingUI& ui)
    :ui_(ui)
{

}

void SettingManager::init()
{

}

void SettingManager::run()
{
    // Sound
    auto* sound_cell = ui_.screen_settings_.getCell(ScreenSettings::ScreenSettingsContIndex::MEDIA, ScreenSettings::ScreenSettingsCellIndex::SOUND);

    Context::requestInstance().getEvent().registerEvent(sound_cell->getObj(), [this](lv_event_t* e) {
        auto code = lv_event_get_code(e);
        if (code == LV_EVENT_CLICKED) {
            lv_scr_load(ui_.screen_sound_.getObj());
            ESP_UTILS_LOGI("switch to screen sound!");
        }
        });

}

void SettingManager::del()
{

}

void SettingManager::processSoundUI()
{

}
