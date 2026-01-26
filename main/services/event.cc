#include "event.hpp"
#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "event"

boost::signals2::connection Event::connectEventSignal(void* obj, EventSignal::slot_type slot_fun)
{
    return event_map_[obj].connect(slot_fun);
}

void Event::sendEvent(void* obj, lv_event_t* e)
{
    event_map_[obj](e);
}

/**
 *@brief 删除此obj的所有handler
 *
 * @param obj
 */
void Event::unregisterEvent(void* obj)
{
    auto obj_it = event_map_.find(obj);
    if (obj_it == event_map_.end()) {
        ESP_UTILS_LOGW("not found obj[%0#x]", obj);
        return;
    }

    event_map_.erase(obj_it);
    ESP_UTILS_LOGI("unregisterEvent[%0#x]", obj);
}
