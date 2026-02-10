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
    auto& event = Context::requestInstance().getEvent();
    // back
    event.subscribe(Event::Id::BACK, [this](void* data) {
        back();
        });

    // enter
    event.subscribe(Event::Id::ENTER, [this](void* data) {
        auto* cell = static_cast<Cell*>(data);
        auto cell_obj_it = cell_ui_map_.find(cell);
        ESP_UTILS_CHECK_FALSE_EXIT(cell_obj_it != cell_ui_map_.end(), "cell not found in map!");
        enter(cell_obj_it->second);
        });

    // value change
    event.subscribe(Event::Id::VALUECHANGE, [this](void* data) {
        auto* cell = static_cast<Cell*>(data);
        auto& nvs_service = NVSservice::requestInstance();
        // 查表
        auto cell_val_it = cell_nvs_map_.find(cell);
        ESP_UTILS_CHECK_FALSE_EXIT(cell_val_it != cell_nvs_map_.end(), "cell not found in map!");
        int val = lv_slider_get_value(cell->getElementObj(CellElement::CENTER_SLIDER));
        nvs_service.setLocalParam(cell_val_it->second, val, cell);
        });

    // restart
    auto* restart_cell = ui_.screen_settings_.getCell(ScreenSettings::ContainerIndex::MORE, ScreenSettings::CellIndex::RESTART);
    event.subscribe(restart_cell->getEventId(), [this](void* data) {
        esp_restart();
        });
}

void SettingManager::run()
{
    // Sound Cell
    auto* sound_cell = ui_.screen_settings_.getCell(ScreenSettings::ContainerIndex::MEDIA, ScreenSettings::CellIndex::SOUND);
    bindCellToScreen(sound_cell, ui_.screen_sound_);

    // Brightness Cell
    auto* brightness_cell = ui_.screen_settings_.getCell(ScreenSettings::ContainerIndex::MEDIA, ScreenSettings::CellIndex::DISPLAY);
    bindCellToScreen(brightness_cell, ui_.screen_display_);

    auto& nvs_service = NVSservice::requestInstance();

    // Sound Screen
    auto* sound_volume = ui_.screen_sound_.getCell(ScreenSound::ContainerIndex::VOLUME, ScreenSound::CellIndex::VOLUME_SLIDER);
    bindCellValue(sound_volume, NVSservice::SETTINGS_VOLUME);

    auto* volume_slider = sound_volume->getElementObj(CellElement::CENTER_SLIDER);
    NVSservice::Value val;
    ESP_UTILS_CHECK_FALSE_EXIT(nvs_service.getLocalParam(NVSservice::SETTINGS_VOLUME, val), "get volume slider failed!");
    ESP_UTILS_CHECK_FALSE_EXIT(std::holds_alternative<int>(val), "Invalid volume type!");
    lv_slider_set_value(volume_slider, std::get<int>(val), LV_ANIM_OFF);

    // Display brightness
    auto* display_brightness = ui_.screen_display_.getCell(ScreenDisplay::ContainerIndex::BRIGHTNESS, ScreenDisplay::CellIndex::BRIGHTNESS_SLIDER);
    bindCellValue(display_brightness, NVSservice::SETTINGS_BRIGHTNESS);

    auto* brightness_slider = display_brightness->getElementObj(CellElement::CENTER_SLIDER);
    ESP_UTILS_CHECK_FALSE_EXIT(nvs_service.getLocalParam(NVSservice::SETTINGS_BRIGHTNESS, val), "get brightness val failed!");
    ESP_UTILS_CHECK_FALSE_EXIT(std::holds_alternative<int>(val), "Invalid brightness slider type");
    lv_slider_set_value(brightness_slider, std::get<int>(val), LV_ANIM_OFF);

}

void SettingManager::del()
{

}

void SettingManager::processSoundUI()
{

}

void SettingManager::bindCellToScreen(Cell* cell, ScreenBase& screen)
{
    ESP_UTILS_CHECK_NULL_EXIT(cell, "cell is null!");
    ScreenObj screen_obj = screen.getObj();

    cell_ui_map_[cell] = screen_obj;
}

void SettingManager::bindCellValue(Cell* cell, const std::string& nvs_key)
{
    ESP_UTILS_CHECK_NULL_EXIT(cell, "cell is null!");
    cell_nvs_map_[cell] = nvs_key;
}

void SettingManager::enter(lv_obj_t* screen)
{
    ESP_UTILS_CHECK_NULL_EXIT(screen, "screen is nullptr");
    // 压栈
    screen_stack_.push(lv_scr_act());
    ESP_UTILS_LOGI("screen stack push,size[%d]", screen_stack_.size());
    // 切换屏幕
    lv_scr_load(screen);
}

void SettingManager::back()
{
    // ESP_UTILS_CHECK_FALSE_EXIT(!screen_stack_.empty(), "screen stack is empty!");
    if (screen_stack_.empty()) {
        ESP_UTILS_LOGE("screen stack is empty!");
        return;
    }
    auto* screen = screen_stack_.top();
    screen_stack_.pop();
    ESP_UTILS_LOGI("screen stack pop,remain[%d]", screen_stack_.size());
    lv_scr_load(screen);
}



