#include "settings.hpp"
#include "setting_app_assets.h"
#include "services/context.hpp"
#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "screen_settings"

#define SCREEN_SETTINGS_CELL_CONFIG() \
    (CellElement::LEFT_ICON | CellElement::LEFT_MAIN_LABEL | CellElement::RIGHT_ICON)

#define SCREEN_SETTINGS_WLAN_CELL_DATA_CONFIG() \
    CellConf{\
        .left_icon= &app_icon_wireless_wlan_48_48,\
        .left_main_text= "Wlan",\
        .right_icon= &app_icon_arrow_right_48_48,\
        .right_main_text= "Off",\
        .event_id=Event::Id::ENTER \
    }

#define SCREEN_SETTINGS_SOUND_CELL_DATA_CONFIG() \
CellConf{\
    .left_icon= &app_icon_media_sound_48_48,\
    .left_main_text= "Sound",\
    .right_icon= &app_icon_arrow_right_48_48,\
    .event_id=Event::Id::ENTER \
}

#define SCREEN_SETTINGS_DISPLAY_CELL_DATA_CONFIG() \
CellConf{\
    .left_icon= &app_icon_media_display_48_48,\
    .left_main_text= "Display",\
    .right_icon= &app_icon_arrow_right_48_48,\
    .event_id=Event::Id::ENTER \
}

#define SCREEN_SETTINGS_MORE_CELL_DATA_CONFIG() \
CellConf{\
    .left_icon= &app_icon_more_about_48_48,\
    .left_main_text= "More",\
    .right_icon= &app_icon_arrow_right_48_48,\
    .event_id=Event::Id::ENTER \
}

#define SCREEN_SETTINGS_RESTART_CELL_DATA_CONFIG() \
CellConf{\
    .left_icon= &app_icon_more_restart_48_48,\
    .left_main_text= "Restart",\
    .event_id=Event::Id::CUSTOM \
}

void ScreenSettings::setupMain(lv_obj_t* parent)
{
    // 调用基类构建框架
    ScreenBase::setupMain(parent);
    ScreenBase::setupHeader("Launcher");
    ScreenBase::setupCont();

    container_map_.clear();
    // Wireless Cont
    setupWireless();
    // Media Cont
    setupMedia();
    // About
    setupAbout();

}

void ScreenSettings::setupWireless()
{
    auto* wireless = addContainer(static_cast<int>(ContainerIndex::WIRELESS), CELLCONTAINER_DEFAULT_CONFIG(cont_obj_, "Wireless"));
    // wlan
    auto cell_element = SCREEN_SETTINGS_CELL_CONFIG() | CellElement::RIGHT_MAIN_LABEL;
    auto* wlan_cell = wireless->addCell(static_cast<int>(CellIndex::WLAN), cell_element);
    wlan_cell->update(SCREEN_SETTINGS_WLAN_CELL_DATA_CONFIG());
}

void ScreenSettings::setupMedia()
{
    auto* media = addContainer(static_cast<int>(ContainerIndex::MEDIA), CELLCONTAINER_DEFAULT_CONFIG(cont_obj_, "Media"));
    // sound
    auto* sound_cell = media->addCell(static_cast<int>(CellIndex::SOUND), SCREEN_SETTINGS_CELL_CONFIG());
    sound_cell->update(SCREEN_SETTINGS_SOUND_CELL_DATA_CONFIG());

    // display
    auto* display_cell = media->addCell(static_cast<int>(CellIndex::DISPLAY), SCREEN_SETTINGS_CELL_CONFIG());
    display_cell->update(SCREEN_SETTINGS_DISPLAY_CELL_DATA_CONFIG());

}

void ScreenSettings::setupAbout()
{
    auto* about = addContainer(static_cast<int>(ContainerIndex::MORE), CELLCONTAINER_DEFAULT_CONFIG(cont_obj_, "about"));
    auto* more_cell = about->addCell(static_cast<int>(CellIndex::ABOUT), SCREEN_SETTINGS_CELL_CONFIG());
    more_cell->update(SCREEN_SETTINGS_MORE_CELL_DATA_CONFIG());

    auto* restart_cell = about->addCell(static_cast<int>(CellIndex::RESTART), CellElement::LEFT_ICON | CellElement::LEFT_MAIN_LABEL);
    restart_cell->update(SCREEN_SETTINGS_RESTART_CELL_DATA_CONFIG());

}

template<>
Cell* ScreenBase::getCell<ScreenSettings::ContainerIndex, ScreenSettings::CellIndex>(
    ScreenSettings::ContainerIndex container_key,
    ScreenSettings::CellIndex cell_key
);
