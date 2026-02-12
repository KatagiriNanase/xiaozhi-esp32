#pragma once
#include "base.hpp"

class ScreenWlan :public ScreenBase {
public:

    enum class ContainerIndex {
        SWITCH,
        CONNECTED,
        AVAILABLE,
        MAX,
    };

    enum class CellIndex {
        SWITCH,
        CONNECTED,
        MAX
    };

    void setupMain(lv_obj_t* parent = nullptr) override;
    bool setConnectedNetVisible(bool is_visible);
    bool setAvaliableNetVisible(bool is_visible);
private:
    void setupWlanSwitch();
    void setupConnectedNet();
    void setupAvaliableNet();
};

