#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "Services"
#include "esp_lib_utils.h"
#include "service.hpp"
#include "nvs_service.hpp"

bool services_init()
{
    ESP_UTILS_LOG_TRACE_GUARD();

    /* Startup NVS Service */
    ESP_UTILS_CHECK_FALSE_RETURN(NVSservice::requestInstance().begin(), false, "Failed to begin storage NVS");

    return true;
}

