#include "wlan.hpp"
#include "cell_container.hpp"
#include "setting_app_assets.h"

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "screen_wlan"

#define SCREEN_SOUND_CELL_CONFIG() \
    (CellElement::LEFT_ICON | CellElement::CENTER_SLIDER | CellElement::RIGHT_ICON)

#define SCREEN_WLAN_SWITCH_CELL_DATA_CONFIG() \
    CellConf{\
        .left_main_text= "Wlan",\
        .event_id=Event::Id::VALUECHANGE \
    }

void ScreenWlan::setupMain(lv_obj_t* parent)
{
    ScreenBase::setupMain(parent);
    ScreenBase::setupHeader("Settings");
    ScreenBase::setupCont();

    // switch
    setupWlanSwitch();
    // connected
    setupConnectedNet();
    // avaliable networks
    setupAvaliableNet();
}

bool ScreenWlan::setConnectedNetVisible(bool is_visible)
{
    auto* connected_container = getContainer(ContainerIndex::CONNECTED)->getObj();
    ESP_UTILS_CHECK_NULL_RETURN(connected_container, false, "connected container is null");
    is_visible ? (lv_obj_remove_flag(connected_container, LV_OBJ_FLAG_HIDDEN)) : (lv_obj_add_flag(connected_container, LV_OBJ_FLAG_HIDDEN));
    return true;
}

bool ScreenWlan::setAvaliableNetVisible(bool is_visible)
{
    auto* avaliable_container = getContainer(ContainerIndex::AVAILABLE)->getObj();
    ESP_UTILS_CHECK_NULL_RETURN(avaliable_container, false, "avaliabl container is null");
    is_visible ? (lv_obj_remove_flag(avaliable_container, LV_OBJ_FLAG_HIDDEN)) : (lv_obj_add_flag(avaliable_container, LV_OBJ_FLAG_HIDDEN));
    return true;
}

void ScreenWlan::setupWlanSwitch()
{
    auto* container = addContainer(static_cast<int>(ContainerIndex::SWITCH), CELLCONTAINER_DEFAULT_CONFIG(cont_obj_, std::string()));
    auto* cell = container->addCell(static_cast<int>(CellIndex::SWITCH), (CellElement::LEFT_MAIN_LABEL | CellElement::RIGHT_SWITCH));
    auto* left_label = cell->getElementObj(CellElement::LEFT_MAIN_LABEL);
    cell->update(SCREEN_WLAN_SWITCH_CELL_DATA_CONFIG());

}

void ScreenWlan::setupConnectedNet()
{
    auto* container = addContainer(static_cast<int>(ContainerIndex::CONNECTED), CELLCONTAINER_DEFAULT_CONFIG(cont_obj_, "Connected"));
    auto* cell = container->addCell(static_cast<int>((CellIndex::CONNECTED)), (CellElement::LEFT_MAIN_LABEL | CellElement::RIGHT_ICON));

    // hide
    lv_obj_add_flag(container->getObj(), LV_OBJ_FLAG_HIDDEN);
}

void ScreenWlan::setupAvaliableNet()
{
    auto* container = addContainer(static_cast<int>(ContainerIndex::AVAILABLE), CELLCONTAINER_DEFAULT_CONFIG(cont_obj_, "Available"));
}

template<>
Cell* ScreenBase::getCell<ScreenWlan::ContainerIndex, ScreenWlan::CellIndex>(
    ScreenWlan::ContainerIndex,
    ScreenWlan::CellIndex
);


