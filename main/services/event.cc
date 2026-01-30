#include "event.hpp"
#include "context.hpp"

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "event"

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


