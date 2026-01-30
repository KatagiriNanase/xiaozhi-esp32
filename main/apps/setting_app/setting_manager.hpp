#pragma once
#include "setting_ui.hpp"
#include "services/nvs_service.hpp"
#include <stack>
#include <unordered_map>

class SettingManager {
public:
    using ScreenObj = lv_obj_t*;

    SettingManager(SettingUI& ui);

    void init();
    void run();
    void del();
private:
    void processSoundUI();
    void bindCellToScreen(Cell*, ScreenBase&);
    void bindCellValue(Cell*, const std::string& nvs_key);
    void enter(lv_obj_t*);
    void back();

    std::stack<lv_obj_t*> screen_stack_;
    std::unordered_map<Cell*, ScreenObj> cell_ui_map_;
    std::unordered_map<Cell*, NVSservice::Key> cell_nvs_map_;
    SettingUI& ui_;
};