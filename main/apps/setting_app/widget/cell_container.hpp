#pragma once

#include <string>
#include <array>
#include <memory>
#include <list>
#include <map>
#include <utility>
#include <type_traits>
#include <lvgl.h>

#include "services/event.hpp"

#define CELLCONTAINER_DEFAULT_CONFIG(parent_param, title_param) \
    ( \
        CellContainer::Config{ \
            .parent = parent_param,     \
            .title = title_param        \
        } \
    )

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

    Event::Id event_id;
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
    void setupLeftArea(lv_obj_t* parent);
    void setupCenterArea(lv_obj_t* parent);
    void setupRightArea(lv_obj_t* parent);
    void update(const CellConf& cell_conf);
    void setSplitLineVisible(bool visible);
    lv_obj_t* getElementObj(CellElement cell_element);
    lv_obj_t* getObj(void);
    int getVal(void) { return val_; }
    Event::Id getEventId(void) { return event_id_; }

private:

    static void onCellTouch(lv_event_t* e);
    CellElement layout_mask_;
    lv_obj_t* main_obj_;
    lv_obj_t* split_line_ = nullptr;
    Event::Id event_id_;
    int val_;
    std::array<lv_point_precise_t, 2> split_line_points_ = { {
        {40, 0},
        {260, 0}
    } };
    std::map<CellElement, lv_obj_t*> element_map_;
};

class CellContainer {
public:

    struct Config {
        lv_obj_t* parent;
        std::string title;
    };

    CellContainer(const Config&);
    ~CellContainer();

    void clear() { cells_.clear(); }
    Cell* addCell(int key, CellElement layout);
    int getCellIndex(lv_obj_t* obj) const;
    Cell* getCellbyIndex(int key) const;
    size_t getCellCount() const { return cells_.size(); }
    lv_obj_t* getObj() { return main_obj_; }

private:
    void setup(lv_obj_t* parent);
    void setTitle(const char* title);

    lv_obj_t* main_obj_ = nullptr;
    lv_obj_t* title_label_ = nullptr;
    lv_obj_t* cont_obj_ = nullptr;
    std::list < std::pair<int, std::unique_ptr<Cell>>> cells_;
};