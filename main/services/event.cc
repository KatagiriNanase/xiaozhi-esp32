#include "event.hpp"
#include "context.hpp"

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "event"

Event::Id& operator++(Event::Id& id)
{
    id = static_cast<Event::Id>(static_cast<int>(id) + 1);
    return id;
}

Event::Id operator++(Event::Id& id, int)
{
    Event::Id old_id = id;
    ++id;
    return old_id;
}

boost::signals2::connection Event::subscribe(Id event_id, EventSignal::slot_type slot_fun)
{
    return event_map_[event_id].connect(slot_fun);
}

void Event::publish(Id event_id, void* data)
{
    auto id_it = event_map_.find(event_id);
    if (id_it != event_map_.end()) {
        id_it->second(data);
    }
    else {
        ESP_UTILS_LOGE("No subscribers for event id: %d", event_id);
    }
}

Event::Id Event::getFreeEventId()
{
    if (!available_event_ids_.empty()) {
        Id id = *available_event_ids_.begin();
        return id;
    }

    return ++free_event_id_;
}


