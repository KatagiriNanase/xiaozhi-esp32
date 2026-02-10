#include "sound.hpp"
#include "cell_container.hpp"
#include "setting_app_assets.h"

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "screen_settings"

#define SCREEN_SOUND_CELL_CONFIG() \
    (CellElement::LEFT_ICON | CellElement::CENTER_SLIDER | CellElement::RIGHT_ICON)

#define SCREEN_SOUND_VOLUME_CELL_DATA_CONFIG() \
    CellConf{\
        .left_icon= &app_icon_sound_less_48_48,\
        .left_main_text= "Sound",\
        .right_icon= &app_icon_sound_more_48_48,\
        .event_id=Event::Id::VALUECHANGE \
    }

void ScreenSound::setupMain(lv_obj_t* parent)
{
    ScreenBase::setupMain(parent);
    ScreenBase::setupHeader("Settings");
    ScreenBase::setupCont();

    // Volume
    setupVolume();
}

void ScreenSound::setupVolume()
{
    auto* volume = addContainer(static_cast<int>(ContainerIndex::VOLUME), CELLCONTAINER_DEFAULT_CONFIG(cont_obj_, "Volume"));

    auto* volume_cell = volume->addCell(static_cast<int>(CellIndex::VOLUME_SLIDER), SCREEN_SOUND_CELL_CONFIG());
    volume_cell->update(SCREEN_SOUND_VOLUME_CELL_DATA_CONFIG());
}

template<>
Cell* ScreenBase::getCell<ScreenSound::ContainerIndex, ScreenSound::CellIndex>(
    ScreenSound::ContainerIndex,
    ScreenSound::CellIndex
);
