#include "nvs_service.h"
#include "boost/thread.hpp"

#include "esp_lib_utils.h"
#define ESP_UTILS_LOG_TAG "main"

extern "C" void app_main()
{
    NVSservice::requestInstance().begin();

    boost::thread t([]() {
        while (true) {
            esp_utils_mem_print_info();
            boost::this_thread::sleep_for(boost::chrono::seconds(5));
        }
        }
    );

    t.join();
}