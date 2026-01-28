#include "event.hpp"
#include "context.hpp"
#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "event"

boost::signals2::connection Event::registerEvent(lv_obj_t* obj, EventSignal::slot_type slot_fun)
{
    // obj删除时，自动unregister事件
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* target = lv_event_get_target_obj(e);
        Context::requestInstance().getEvent().unregisterEvent(target);
        }, LV_EVENT_DELETE, nullptr);

    return event_map_[obj].connect(slot_fun);
}

void Event::sendEvent(lv_obj_t* obj, lv_event_t* e)
{
    auto it = event_map_.find(obj);
    if (it == event_map_.end()) {
        ESP_UTILS_LOGW("obj[%0#x] not exist!");
        return;
    }

    it->second(e);
}

/**
 *@brief 删除此obj的所有handler
 *
 * @param obj
 */
void Event::unregisterEvent(lv_obj_t* obj)
{
    auto obj_it = event_map_.find(obj);
    if (obj_it == event_map_.end()) {
        ESP_UTILS_LOGW("not found obj[%0#x]", obj);
        return;
    }

    event_map_.erase(obj_it);
    ESP_UTILS_LOGI("unregisterEvent[%0#x]", obj);
}

