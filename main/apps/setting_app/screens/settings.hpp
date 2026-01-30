#pragma once
#include "base.hpp"
#include "cell_container.hpp"

#include <map>
#include <memory>


class ScreenSettings :public ScreenBase {
public:

    enum class ContainerIndex {
        WIRELESS,
        MEDIA,
        MORE,
        MAX
    };

    enum class CellIndex {
        WLAN,
        SOUND,
        DISPLAY,
        ABOUT,
        MAX
    };
    void setupMain(lv_obj_t* parent = nullptr) override;
    
private:
    void setupWireless();
    void setupMedia();
    void setupAbout();
};
