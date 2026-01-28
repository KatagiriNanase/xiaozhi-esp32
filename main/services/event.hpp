#pragma once

// boost
#include <boost/signals2.hpp>

// stl
#include <unordered_map>
#include <memory>

// lvgl
#include <lvgl.h>

class Event {
public:
    using EventSignal = boost::signals2::signal<void(lv_event_t*)>;

    Event() = default;
    ~Event() = default;

    Event(const Event&) = delete;
    Event(Event&&) = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&) = delete;

    boost::signals2::connection registerEvent(lv_obj_t* obj, EventSignal::slot_type slot_fun);

    void sendEvent(lv_obj_t* obj, lv_event_t* e);

    void unregisterEvent(lv_obj_t* obj);
private:
    std::unordered_map<lv_obj_t*, EventSignal> event_map_;
};