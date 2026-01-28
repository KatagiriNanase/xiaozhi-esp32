#include "cell_container.hpp"
#include "services/context.hpp"
#include "setting_app_assets.h"
#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "cell_container"


Cell::Cell(CellElement layout_mask)
    :layout_mask_(layout_mask)
{

}

Cell::~Cell()
{
    if (main_obj_ && lv_obj_is_valid(main_obj_)) {
        lv_obj_del(main_obj_);
        main_obj_ = nullptr;
    }
}

void Cell::setup(lv_obj_t* parent)
{
    main_obj_ = lv_obj_create(parent);
    lv_obj_set_width(main_obj_, LV_PCT(100));
    lv_obj_set_height(main_obj_, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(main_obj_, 10, 0);
    lv_obj_set_style_bg_opa(main_obj_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(main_obj_, 0, 0);
    // lv_obj_set_style_bg_color(main_obj_, lv_color_hex(0x747476), 0);
    lv_obj_remove_flag(main_obj_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(main_obj_, LV_SCROLLBAR_MODE_OFF);

    lv_obj_add_event_cb(main_obj_, onCellTouch, LV_EVENT_CLICKED, nullptr);

    // 底部分割线
    split_line_ = lv_line_create(main_obj_);
    lv_line_set_points(split_line_, split_line_points_.data(), split_line_points_.size());
    lv_obj_set_style_line_width(split_line_, 2, 0);
    lv_obj_set_style_line_color(split_line_, lv_color_white(), 0);
    lv_obj_align(split_line_, LV_ALIGN_BOTTOM_MID, 0, 7);

    // left area
    if (layout_mask_ & CellElement::_LEFT_AREA)
        setupLeftArea();

    // center area
    if (layout_mask_ & CellElement::CENTER_SLIDER)
        setupCenterArea();

    // right area
    if (layout_mask_ & CellElement::_RIGHT_AREA)
        setupRightArea();
}

void Cell::setupLeftArea()
{
    // left obj
    left_obj_ = lv_obj_create(main_obj_);
    lv_obj_set_size(left_obj_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(left_obj_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(left_obj_, 0, 0);
    lv_obj_set_style_pad_row(left_obj_, 5, 0);  // icon和label隔开点
    lv_obj_set_style_border_width(left_obj_, 0, 0);
    lv_obj_set_flex_flow(left_obj_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(left_obj_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(left_obj_, LV_ALIGN_LEFT_MID, 0, 0);

    // left icon
    if (layout_mask_ & CellElement::LEFT_ICON) {
        left_icon_obj_ = lv_img_create(left_obj_);
        lv_obj_set_size(left_icon_obj_, IMG_WIDTH, IMG_WIDTH);
    }

    // left label
    if (layout_mask_ & CellElement::_LEFT_LABEL) {
        if (layout_mask_ & CellElement::_LEFT_LABEL) {
            left_main_label_ = lv_label_create(left_obj_);
            lv_obj_set_style_text_color(left_main_label_, lv_color_white(), 0);
            lv_obj_set_style_text_font(left_main_label_, &ui_font_DejaVuSans16, 0);
        }
    }

}

void Cell::setupCenterArea()
{
    if (layout_mask_ & CellElement::CENTER_SLIDER) {
        center_slider_ = lv_slider_create(main_obj_);
        lv_obj_set_width(center_slider_, LV_PCT(70));
        lv_slider_set_range(center_slider_, 0, 100);
        lv_obj_center(center_slider_);
    }
}

void Cell::setupRightArea()
{
    // right obj
    right_obj_ = lv_obj_create(main_obj_);
    lv_obj_set_size(right_obj_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(right_obj_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(right_obj_, 0, 0);
    lv_obj_set_style_pad_row(right_obj_, 5, 0);  // icon和label隔开点
    lv_obj_set_style_border_width(right_obj_, 0, 0);
    lv_obj_set_flex_flow(right_obj_, LV_FLEX_FLOW_ROW_REVERSE);
    lv_obj_set_flex_align(right_obj_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(right_obj_, LV_ALIGN_RIGHT_MID, 0, 0);

    // right switch
    if (layout_mask_ & CellElement::RIGHT_SWITCH) {
        right_switch_ = lv_switch_create(right_obj_);
        lv_obj_set_size(right_switch_, 40, 20);
    }

    // right icon
    if (layout_mask_ & CellElement::RIGHT_ICON) {
        right_icon_obj_ = lv_img_create(right_obj_);
        lv_obj_set_size(right_icon_obj_, IMG_WIDTH, IMG_HEIGTH);
    }

    // right text
    if (layout_mask_ & CellElement::_RIGHT_LABEL) {
        if (layout_mask_ & CellElement::RIGHT_MAIN_LABEL) {
            right_main_label_ = lv_label_create(right_obj_);
            lv_obj_set_style_text_font(right_main_label_, &ui_font_DejaVuSans16, 0);
            lv_obj_set_style_text_color(right_main_label_, lv_color_white(), 0);
        }
    }

}

void Cell::update(const CellConf& cell_conf)
{
    // left icon
    if (left_icon_obj_) {
        lv_img_set_src(left_icon_obj_, cell_conf.left_icon);
    }
    // left label
    if (left_main_label_) {
        lv_label_set_text(left_main_label_, cell_conf.left_main_text.c_str());
    }
    // right icon
    if (right_icon_obj_) {
        lv_img_set_src(right_icon_obj_, cell_conf.right_icon);
    }
    // right label
    if (right_main_label_) {
        lv_label_set_text(right_main_label_, cell_conf.right_main_text.c_str());
    }
    // right switch
    if (right_switch_) {
        if (cell_conf.switch_state)
            lv_obj_add_state(right_switch_, LV_STATE_CHECKED);
        else
            lv_obj_remove_state(right_switch_, LV_STATE_CHECKED);
    }
    // center slider
    if (center_slider_) {
        lv_slider_set_value(center_slider_, cell_conf.slider_value, LV_ANIM_OFF);
    }
}

void Cell::setSplitLineVisible(bool visible)
{
    if (!split_line_) return;

    if (visible) {
        lv_obj_remove_flag(split_line_, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_add_flag(split_line_, LV_OBJ_FLAG_HIDDEN);
    }
}

lv_obj_t* Cell::getObj(void)
{
    return main_obj_;
}

void Cell::onCellTouch(lv_event_t* e)
{
    auto* obj = lv_event_get_target_obj(e);
    auto code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_UTILS_LOGI("switch screen!");
        Context::requestInstance().getEvent().sendEvent(obj, e);
    }
}

CellContainer::~CellContainer()
{
    cells_.clear();

    if (main_obj_ && lv_obj_is_valid(main_obj_)) {
        lv_obj_del(main_obj_);
        main_obj_ = nullptr;
    }
}

void CellContainer::setup(lv_obj_t* parent)
{
    // Main
    main_obj_ = lv_obj_create(parent);
    lv_obj_set_size(main_obj_, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(main_obj_, LV_OPA_TRANSP, 0);
    // lv_obj_set_style_pad_all(main_obj_, 10, 0);
    lv_obj_set_style_pad_hor(main_obj_, 25, 0);
    lv_obj_set_style_border_width(main_obj_, 0, 0);
    lv_obj_set_style_pad_column(main_obj_, 5, 0);
    lv_obj_set_flex_flow(main_obj_, LV_FLEX_FLOW_COLUMN);
    lv_obj_remove_flag(main_obj_, LV_OBJ_FLAG_SCROLLABLE);

    // Title
    title_label_ = lv_label_create(main_obj_);
    lv_obj_set_style_text_font(title_label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title_label_, lv_color_white(), 0);
    lv_obj_add_flag(title_label_, LV_OBJ_FLAG_HIDDEN);

    // Cont
    cont_obj_ = lv_obj_create(main_obj_);
    lv_obj_set_size(cont_obj_, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(cont_obj_, lv_color_hex(0x747476), 0);
    lv_obj_set_style_radius(cont_obj_, 12, 0);
    lv_obj_set_style_pad_all(cont_obj_, 0, 0);
    lv_obj_set_style_border_width(cont_obj_, 0, 0);
    lv_obj_set_flex_flow(cont_obj_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_column(cont_obj_, 0, 0);
    lv_obj_remove_flag(cont_obj_, LV_OBJ_FLAG_SCROLLABLE);

}

void CellContainer::setTitle(const char* title)
{
    ESP_UTILS_CHECK_NULL_EXIT(title, "Title is null!");
    lv_label_set_text(title_label_, title);
    lv_obj_remove_flag(title_label_, LV_OBJ_FLAG_HIDDEN);
}

Cell* CellContainer::addCell(CellElement layout)
{
    auto new_cell = std::make_unique<Cell>(layout);
    new_cell->setup(cont_obj_);

    new_cell->setSplitLineVisible(false);

    if (!cells_.empty()) {
        cells_.back()->setSplitLineVisible(true);
    }

    Cell* ptr = new_cell.get();
    cells_.emplace_back(std::move(new_cell));

    return ptr;
}

int CellContainer::getCellIndex(lv_obj_t* obj) const
{
    for (size_t i = 0;i < cells_.size();++i) {
        if (cells_[i]->getObj() == obj)
            return static_cast<int>(i);
    }
    return -1;
}

