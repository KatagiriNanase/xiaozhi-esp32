#include "manager.hpp"
#include "context.hpp"
#include "app_base.hpp"
#include "esp_lib_utils.h"

#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "manager"

Manager::~Manager() = default;  // Declare destructor to be defined in .cc file

bool Manager::installApp(std::unique_ptr<App> app, int& ret_id)
{
    ESP_UTILS_CHECK_NULL_RETURN(app, false, "App is nullptr!");
    int new_id = next_id_++;

    app->id_ = new_id;
    app->_system_context = &Context::requestInstance();

    app->onCreate();
    ESP_UTILS_LOGI("install App[%d]:%s", new_id, app->getAppName().c_str());

    app_map_[new_id] = std::move(app);
    ret_id = new_id;

    return true;
}

bool Manager::launchApp(int id)
{

    // 避免重复启动同一个app
    ESP_UTILS_CHECK_FALSE_RETURN(id != current_app_id_, false, "App[%d] is already running", id);

    // 查找是否存在
    auto it = app_map_.find(id);
    if (it == app_map_.end()) {
        ESP_UTILS_LOGW("App[%d] not exist");
        return false;
    }

    // 停止当前App（如果存在）
    if (current_app_ != nullptr) {
        current_app_->onPause();
    }

    // 切换App
    current_app_ = it->second.get();
    current_app_id_ = current_app_->getAppId();
    current_app_->onResume();
    
    ESP_UTILS_LOGI("Launch App[%d]:%s", current_app_id_, current_app_->getAppName());
    return true;
}


