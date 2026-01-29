#pragma once
#include "base.hpp"
#include "cell_container.hpp"

#include <map>
#include <memory>


class ScreenSettings :public ScreenBase {
public:

    enum class ScreenSettingsContIndex {
        WIRELESS,
        MEDIA,
        MORE,
        MAX
    };

    enum class ScreenSettingsCellIndex {
        WLAN,
        SOUND,
        DISPLAY,
        ABOUT,
        MAX
    };
    void setupMain(lv_obj_t* parent = nullptr) override;
    Cell* getCell(ScreenSettingsContIndex cont_index, ScreenSettingsCellIndex cell_key);
private:
    void setupWireless();
    void setupMedia();
    void setupAbout();
};
