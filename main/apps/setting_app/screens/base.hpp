#pragma once
#include <string>
#include <map>
#include <memory>

#include "cell_container.hpp"

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
protected:
    lv_obj_t* main_obj_ = nullptr;
    lv_obj_t* header_obj_ = nullptr;
    lv_obj_t* header_label_ = nullptr;
    std::string header_title_ = {};
    lv_obj_t* cont_obj_ = nullptr;
    std::map<int, std::unique_ptr<CellContainer>> container_map_;
};