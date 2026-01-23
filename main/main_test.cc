#include "modules/bsp/service.hpp"
#include "modules/bsp/display.hpp"
#include "boost/thread.hpp"
#include "bsp/esp-bsp.h"
#include "event.hpp"

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "main"

extern "C" void app_main()
{

    services_init();
    display_init(false);
    Event event;

    auto* btn = lv_button_create(lv_scr_act());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_center(btn);

    lv_obj_add_event_cb(btn, [](lv_event_t* e) {
        auto code = lv_event_get_code(e);
        auto* obj = lv_event_get_target(e);
        auto* event = static_cast<Event*>(lv_event_get_user_data(e));
        event->sendEvent(obj, e);
        }, LV_EVENT_CLICKED, &event);

    // 观察者
    event.connectEventSignal(btn, [](lv_event_t* e) {
        auto code = lv_event_get_code(e);
        auto* obj = lv_event_get_target(e);
        if (code == LV_EVENT_CLICKED) {
            ESP_UTILS_LOGI("hello event signal!");
        }

        });

    boost::thread t([]() {
        while (true) {
            esp_utils_mem_print_info();
            boost::this_thread::sleep_for(boost::chrono::seconds(5));
        }
        }
    );

    t.join();
}