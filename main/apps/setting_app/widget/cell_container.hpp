#pragma once

#include <string>
#include <array>
#include <vector>
#include <memory>
#include <type_traits>

#include <lvgl.h>

enum class CellElement :uint32_t {
    MAIN = 0,

    // left area
    LEFT_ICON = (1U << 0),
    LEFT_MAIN_LABEL = (1U << 1),
    LEFT_MINOR_LABEL = (1U << 2),
    _LEFT_LABEL = (LEFT_MAIN_LABEL | LEFT_MINOR_LABEL),
    _LEFT_AREA = (LEFT_ICON | _LEFT_LABEL),     // 表名左侧区域可能存在icon和lable

    // right area
    RIGHT_MAIN_LABEL = (1U << 6),
    RIGHT_MINOR_LABEL = (1U << 7),
    _RIGHT_LABEL = (RIGHT_MAIN_LABEL | RIGHT_MINOR_LABEL),
    RIGHT_ICON = (1U << 9),
    RIGHT_SWITCH = (1U << 10),
    _RIGHT_AREA = (_RIGHT_LABEL | RIGHT_ICON | RIGHT_SWITCH),

    // center area
    CENTER_SLIDER = (1U << 12)
};

struct CellConf {
    // left area
    const lv_img_dsc_t* left_icon = nullptr;
    std::string left_main_text;
    std::string left_minor_text;

    // right area
    const lv_img_dsc_t* right_icon = nullptr;
    std::string right_main_text;

    // switch
    bool switch_state = false;

    // slider
    int slider_value = 0;
};

inline CellElement operator|(CellElement l, CellElement r)
{
    return static_cast<CellElement>((static_cast<std::underlying_type<CellElement>::type>(l) | static_cast<std::underlying_type<CellElement>::type>(r)));
}

inline bool operator&(CellElement l, CellElement r)
{
    return static_cast<bool>(
        static_cast<std::underlying_type<CellElement>::type>(l) & static_cast<std::underlying_type<CellElement>::type>(r)
        );
}

class Cell {
public:

    Cell(CellElement lay_mask);
    ~Cell();

    void setup(lv_obj_t* parent);
    void setupLeftArea();
    void setupCenterArea();
    void setupRightArea();
    void update(const CellConf& cell_conf);
    void setSplitLineVisible(bool visible);
    lv_obj_t* getObj(void);

private:

    static void onCellTouch(lv_event_t* e);
    CellElement layout_mask_;

    // container
    lv_obj_t* main_obj_ = nullptr;
    lv_obj_t* left_obj_ = nullptr;
    // lv_obj_t* center_obj_ = nullptr;
    lv_obj_t* right_obj_ = nullptr;

    // widget
    lv_obj_t* left_icon_obj_ = nullptr;
    lv_obj_t* left_main_label_ = nullptr;
    lv_obj_t* left_minor_label_ = nullptr;
    lv_obj_t* right_switch_ = nullptr;
    lv_obj_t* right_icon_obj_ = nullptr;
    lv_obj_t* right_main_label_ = nullptr;
    lv_obj_t* right_minor_label_ = nullptr;
    lv_obj_t* center_slider_ = nullptr;
    lv_obj_t* split_line_ = nullptr;
    std::array<lv_point_precise_t, 2> split_line_points_ = { {
        {40, 0},
        {260, 0}
    } };
};

class CellContainer {
public:
    CellContainer() = default;
    ~CellContainer();

    void setup(lv_obj_t* parent);
    void setTitle(const char* title);
    void clear() { cells_.clear(); }
    Cell* addCell(CellElement layout);
    int getCellIndex(lv_obj_t* obj) const;
    size_t getCellCount() const { return cells_.size(); }

private:
    lv_obj_t* main_obj_ = nullptr;
    lv_obj_t* title_label_ = nullptr;
    lv_obj_t* cont_obj_ = nullptr;
    std::vector<std::unique_ptr<Cell>> cells_;
};