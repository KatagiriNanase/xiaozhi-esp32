#include "modules/bsp/service.hpp"
#include "modules/bsp/display.hpp"
#include "boost/thread.hpp"
#include "bsp/esp-bsp.h"

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "main"

extern "C" void app_main()
{

    services_init();
    display_init(true);

    boost::thread t([]() {
        while (true) {
            esp_utils_mem_print_info();
            boost::this_thread::sleep_for(boost::chrono::seconds(5));
        }
        }
    );

    t.join();
}