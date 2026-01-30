#pragma once
#include "base.hpp"

class ScreenDisplay :public ScreenBase {
public:
    enum class ContainerIndex {
        BRIGHTNESS,
        MAX
    };

    enum class CellIndex {
        BRIGHTNESS_SLIDER,
        MAX
    };

    void setupMain(lv_obj_t* parent = nullptr) override;
private:
    void setupBrightness();
};