#pragma once
#include <string>

struct lv_obj_t;

class ScreenBase {
public:
    virtual ~ScreenBase() = default;  // 虚析构函数
    virtual void setupMain(lv_obj_t* parent = nullptr);
    void setupHeader(std::string title = {});
    void setupCont();
    virtual void del();
    lv_obj_t* getObj(void) { return main_obj_; };
protected:
    lv_obj_t* main_obj_ = nullptr;
    lv_obj_t* header_obj_ = nullptr;
    lv_obj_t* header_label_ = nullptr;
    std::string header_title_ = {};
    lv_obj_t* cont_obj_ = nullptr;
};