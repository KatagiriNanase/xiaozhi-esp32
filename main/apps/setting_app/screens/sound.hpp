#pragma once
#include "base.hpp"

class ScreenSound :public ScreenBase {
public:

    enum class ScreenSoundContainerIndex {
        VOLUME,
        MAX
    };

    enum class ScreenSoundCellIndex {
        VOLUME_SLIDER,
        MAX
    };

    void setupMain(lv_obj_t* parent = nullptr) override;
private:
    void setupVolume();
};