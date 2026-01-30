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

    enum class Id {
        ENTER,
        BACK,
        VALUECHANGE,
        MAX
    };

    using EventSignal = boost::signals2::signal<void(void*)>;

    Event() = default;
    ~Event() = default;

    Event(const Event&) = delete;
    Event(Event&&) = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&) = delete;

    boost::signals2::connection subscribe(Id event_id, EventSignal::slot_type slot_fun);
    void publish(Id event_id, void* data = nullptr);


private:
    std::unordered_map<Id, EventSignal> event_map_;
};