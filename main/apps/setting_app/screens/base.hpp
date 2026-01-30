#pragma once
#include <string>
#include <map>
#include <memory>
#include "cell_container.hpp"

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "base"

struct lv_obj_t;

class ScreenBase {
public:
    virtual ~ScreenBase() = default;  // 虚析构函数
    virtual void setupMain(lv_obj_t* parent = nullptr);
    void setupHeader(std::string title = {});
    void setupCont();
    CellContainer* addContainer(int key, const CellContainer::Config& config);
    virtual void del();
    lv_obj_t* getElementObj(int cont_key, int cell_key, CellElement cell_element);
    lv_obj_t* getObj(void) { return main_obj_; };

    template <typename T_ContainerIndex, typename T_CellIndex>
    Cell* getCell(T_ContainerIndex container_key, T_CellIndex cell_key)
    {
        auto it = container_map_.find(static_cast<int>(container_key));
        if (it == container_map_.end()) {
            ESP_UTILS_LOGE("container[%d] not exist!",container_key);
            return nullptr;
        }
        return it->second->getCellbyIndex(static_cast<int>(cell_key));
    }

protected:
    static void onHeaderTouch(lv_event_t* e);
    lv_obj_t* main_obj_ = nullptr;
    lv_obj_t* header_obj_ = nullptr;
    lv_obj_t* header_label_ = nullptr;
    std::string header_title_ = {};
    lv_obj_t* cont_obj_ = nullptr;
    std::map<int, std::unique_ptr<CellContainer>> container_map_;
};