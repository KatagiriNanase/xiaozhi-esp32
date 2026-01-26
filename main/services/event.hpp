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

    boost::signals2::connection connectEventSignal(void* obj, EventSignal::slot_type slot_fun);

    void sendEvent(void* obj, lv_event_t* e);

    void unregisterEvent(void* obj);
private:
    std::unordered_map<void*, EventSignal> event_map_;
};