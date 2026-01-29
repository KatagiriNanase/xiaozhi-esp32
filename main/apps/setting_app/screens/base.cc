#include "base.hpp"
#include "setting_app_assets.h"
#include <lvgl.h>

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "base"

#define HEADER_OBJ_HEIGHT 60

void ScreenBase::setupMain(lv_obj_t* parent)
{
    if (parent == nullptr)
        main_obj_ = lv_obj_create(nullptr);
    else
        main_obj_ = parent;

    // Main
    lv_obj_set_size(main_obj_, LV_PCT(100), LV_PCT(100));
    // lv_obj_center(main_obj_);
    lv_obj_set_style_bg_color(main_obj_, lv_color_hex(0x222325), 0);
    lv_obj_set_style_pad_all(main_obj_, 0, 0);
    lv_obj_set_style_border_width(main_obj_, 0, 0);
    lv_obj_remove_flag(main_obj_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(main_obj_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_column(main_obj_, 0, 0);
}

void ScreenBase::del()
{
    if (main_obj_ && lv_obj_is_valid(main_obj_))
        lv_obj_del(main_obj_);
    main_obj_ = nullptr;
    header_obj_ = nullptr;
    header_label_ = nullptr;
    cont_obj_ = nullptr;

}

lv_obj_t* ScreenBase::getElementObj(int cont_key, int cell_key, CellElement cell_element)
{
    auto container_it = container_map_.find(cont_key);
    ESP_UTILS_CHECK_FALSE_RETURN(container_it != container_map_.end(), nullptr, "Find container[%d] failed", cont_key);

    auto* cell = container_it->second->getCellbyIndex(cell_key);
    ESP_UTILS_CHECK_NULL_RETURN(cell, nullptr, "Find cell[%d] failed", cell_key);

    return cell->getElementObj(cell_element);
}

void ScreenBase::setupHeader(std::string title)
{
    ESP_UTILS_CHECK_NULL_EXIT(main_obj_, "main_obj is null");
    header_obj_ = lv_obj_create(main_obj_);
    lv_obj_set_size(header_obj_, LV_PCT(100), HEADER_OBJ_HEIGHT);
    lv_obj_set_style_bg_opa(header_obj_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(header_obj_, 0, 0);
    lv_obj_set_flex_flow(header_obj_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_obj_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(header_obj_, 0, 0);
    lv_obj_set_style_pad_right(header_obj_, 5, 0);
    lv_obj_add_flag(header_obj_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(header_obj_, LV_OBJ_FLAG_SCROLLABLE);

    auto* header_arrow = lv_img_create(header_obj_);
    lv_obj_set_size(header_arrow, IMG_WIDTH, IMG_HEIGTH);
    lv_img_set_src(header_arrow, &app_icon_arrow_left_48_48);

    header_label_ = lv_label_create(header_obj_);
    lv_obj_center(header_label_);
    lv_obj_set_style_text_font(header_label_, &ui_font_DejaVuSans16, 0);
    lv_obj_set_style_text_color(header_label_, lv_palette_main(LV_PALETTE_RED), 0);
    if (!title.empty()) {
        lv_label_set_text(header_label_, title.c_str());
        header_title_ = title;
    }

}

void ScreenBase::setupCont()
{
    ESP_UTILS_CHECK_NULL_EXIT(main_obj_, "main_obj_ is null!");

    cont_obj_ = lv_obj_create(main_obj_);
    lv_obj_set_width(cont_obj_, lv_pct(100));
    lv_obj_set_flex_grow(cont_obj_, 1);
    lv_obj_set_style_bg_opa(cont_obj_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont_obj_, 0, 0);
    lv_obj_set_style_pad_all(cont_obj_, 0, 0);
    lv_obj_set_flex_flow(cont_obj_, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(cont_obj_, LV_OBJ_FLAG_SCROLLABLE);
    // 保留滚动条   
    // lv_obj_set_scrollbar_mode(cont_obj_, LV_SCROLLBAR_MODE_OFF);
}

CellContainer* ScreenBase::addContainer(int key, const CellContainer::Config& config)
{
    auto container = std::make_unique<CellContainer>(config);
    if (container_map_.find(key) == container_map_.end()) {
        auto* container_ptr = container.get();
        container_map_[key] = std::move(container);
        ESP_UTILS_LOGI("Add container[%d]", key);
        return container_ptr;
    }
    ESP_UTILS_LOGE("container[%d] already exists!", key);
    return nullptr;
}
