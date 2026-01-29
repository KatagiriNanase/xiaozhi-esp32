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
        MORE
    };

    enum class ScreenSettingsCellIndex {
        WLAN,
        SOUND,
        DISPLAY,
        ABOUT,
    };
    void setupMain(lv_obj_t* parent = nullptr) override;
private:
    void setupWireless();
    void setupMedia();
    void setupAbout();

    std::map<ScreenSettingsContIndex, std::unique_ptr<CellContainer>> container_map_;
};
