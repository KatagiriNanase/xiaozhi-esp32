#include "screens/display.hpp"
#include "setting_app_assets.h"

#define SCREEN_DISPLAY_CELL_CONFIG() \
    (CellElement::LEFT_ICON | CellElement::CENTER_SLIDER | CellElement::RIGHT_ICON)

#define SCREEN_DISPLAY_BRIGHTNESS_CELL_DATA_CONFIG() \
    CellConf{\
        .left_icon= &app_icon_brightness_less_48_48,\
        .left_main_text= "Brightness",\
        .right_icon= &app_icon_brightness_more_48_48,\
        .event_id=Event::Id::VALUECHANGE \
    }

void ScreenDisplay::setupMain(lv_obj_t* parent)
{
    ScreenBase::setupMain(parent);
    ScreenBase::setupHeader("Display");
    ScreenBase::setupCont();

    setupBrightness();
}

void ScreenDisplay::setupBrightness()
{
    auto* display = addContainer(static_cast<int>(ContainerIndex::BRIGHTNESS), CELLCONTAINER_DEFAULT_CONFIG(cont_obj_, "Brightness"));
    auto* display_brightness = display->addCell(static_cast<int>(CellIndex::BRIGHTNESS_SLIDER), SCREEN_DISPLAY_CELL_CONFIG());
    display_brightness->update(SCREEN_DISPLAY_BRIGHTNESS_CELL_DATA_CONFIG());

}