#include "setting_manager.hpp"
#include "screens/settings.hpp"
#include "services/context.hpp"

#include <chrono>   

#include "esp_lib_utils.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "setting_manager"

#define WLAN_OPERATION_THREAD_NAME              "wlan_operation"
#define WLAN_OPERATION_THREAD_STACK_SIZE        (6 * 1024)
#define WLAN_OPERATION_THREAD_STACK_CAPS_EXT    (true)

// WLAN
#define WLAN_INIT_MODE_DEFAULT          WIFI_MODE_STA
#define WLAN_INIT_WAIT_TIMEOUT_MS       (5000)
#define WLAN_START_WAIT_TIMEOUT_MS      (1000)
#define WLAN_STOP_WAIT_TIMEOUT_MS       (1000)

SettingManager::SettingManager(SettingUI& ui)
    :ui_(ui)
{

}

void SettingManager::init()
{
    initWlan();
}

void SettingManager::run()
{
    processEventOnRun();

    processScreenSettings();
    processScreenSound();
    processScreenDisplay();
    processScreenWlan();
}

void SettingManager::del()
{

}

void SettingManager::processEventOnRun()
{
    auto& event = Context::requestInstance().getEvent();
    // back
    event.subscribe(Event::Id::BACK, [this](void* data) {
        back();
        });

    // enter
    event.subscribe(Event::Id::ENTER, [this](void* data) {
        auto* cell = static_cast<Cell*>(data);
        auto cell_obj_it = cell_ui_map_.find(cell);
        ESP_UTILS_CHECK_FALSE_EXIT(cell_obj_it != cell_ui_map_.end(), "cell not found in map!");
        enter(cell_obj_it->second);
        });

    // value change
    event.subscribe(Event::Id::VALUECHANGE, [this](void* data) {
        auto* cell = static_cast<Cell*>(data);
        auto& nvs_service = NVSservice::requestInstance();
        // 查表
        auto cell_val_it = cell_nvs_map_.find(cell);
        ESP_UTILS_CHECK_FALSE_EXIT(cell_val_it != cell_nvs_map_.end(), "cell not found in map!");
        // 判断类型
        lv_obj_t* obj = nullptr;
        if ((obj = cell->getElementObj(CellElement::CENTER_SLIDER))) {
            int val = lv_slider_get_value(obj);
            nvs_service.setLocalParam(cell_val_it->second, val, cell);
        }
        else if ((obj = cell->getElementObj(CellElement::RIGHT_SWITCH))) {
            ESP_UTILS_LOGI("right switch value changed!");
            bool switch_flag = lv_obj_has_state(obj, LV_STATE_CHECKED);
            nvs_service.setLocalParam(cell_val_it->second, switch_flag, cell);
        }
        else
            ESP_UTILS_LOGW("Failed to determine the type of obj");
        });

    // restart
    auto* restart_cell = ui_.screen_settings_.getCell(ScreenSettings::ContainerIndex::MORE, ScreenSettings::CellIndex::RESTART);
    event.subscribe(restart_cell->getEventId(), [this](void* data) {
        esp_restart();
        });
}

void SettingManager::processScreenSettings()
{
    // Sound Cell
    auto* sound_cell = ui_.screen_settings_.getCell(ScreenSettings::ContainerIndex::MEDIA, ScreenSettings::CellIndex::SOUND);
    bindCellToScreen(sound_cell, ui_.screen_sound_);

    // Brightness Cell
    auto* brightness_cell = ui_.screen_settings_.getCell(ScreenSettings::ContainerIndex::MEDIA, ScreenSettings::CellIndex::DISPLAY);
    bindCellToScreen(brightness_cell, ui_.screen_display_);

    // Wlan Cell
    auto* wlan_cell = ui_.screen_settings_.getCell(ScreenSettings::ContainerIndex::WIRELESS, ScreenSettings::CellIndex::WLAN);
    bindCellToScreen(wlan_cell, ui_.screen_wlan_);

}

void SettingManager::processScreenSound()
{
    auto& nvs_service = NVSservice::requestInstance();

    // Sound Screen
    auto* sound_volume = ui_.screen_sound_.getCell(ScreenSound::ContainerIndex::VOLUME, ScreenSound::CellIndex::VOLUME_SLIDER);
    bindCellValue(sound_volume, NVSservice::SETTINGS_VOLUME);

    auto* volume_slider = sound_volume->getElementObj(CellElement::CENTER_SLIDER);
    NVSservice::Value val;
    ESP_UTILS_CHECK_FALSE_EXIT(nvs_service.getLocalParam(NVSservice::SETTINGS_VOLUME, val), "get volume slider failed!");
    ESP_UTILS_CHECK_FALSE_EXIT(std::holds_alternative<int>(val), "Invalid volume type!");
    lv_slider_set_value(volume_slider, std::get<int>(val), LV_ANIM_OFF);
}

void SettingManager::processScreenDisplay()
{
    auto& nvs_service = NVSservice::requestInstance();
    NVSservice::Value val;
    // Display brightness
    auto* display_brightness = ui_.screen_display_.getCell(ScreenDisplay::ContainerIndex::BRIGHTNESS, ScreenDisplay::CellIndex::BRIGHTNESS_SLIDER);
    bindCellValue(display_brightness, NVSservice::SETTINGS_BRIGHTNESS);

    auto* brightness_slider = display_brightness->getElementObj(CellElement::CENTER_SLIDER);
    ESP_UTILS_CHECK_FALSE_EXIT(nvs_service.getLocalParam(NVSservice::SETTINGS_BRIGHTNESS, val), "get brightness val failed!");
    ESP_UTILS_CHECK_FALSE_EXIT(std::holds_alternative<int>(val), "Invalid brightness slider type");
    lv_slider_set_value(brightness_slider, std::get<int>(val), LV_ANIM_OFF);
}

void SettingManager::processScreenWlan()
{
    auto& nvs_service = NVSservice::requestInstance();
    NVSservice::Value val;
    // subscribe switch flag
    nvs_service.connectEventSignal([&](const NVSservice::Event& event) {
        if (event.key != NVSservice::SETTINGS_WLAN_SWITCH || event.operation != NVSservice::Operation::UpdateNVS)
            return;
        NVSservice::Value val;
        ESP_UTILS_CHECK_FALSE_EXIT(nvs_service.getLocalParam(NVSservice::SETTINGS_WLAN_SWITCH, val), "get wlan switch flag val failed!");
        ESP_UTILS_CHECK_FALSE_EXIT(std::holds_alternative<int>(val), "Invalid wlan switch flag type");

        auto* wlan_label = ui_.screen_settings_.getElementObj((int)ScreenSettings::ContainerIndex::WIRELESS, (int)ScreenSettings::CellIndex::WLAN, CellElement::RIGHT_MAIN_LABEL);
        ESP_UTILS_CHECK_NULL_EXIT(wlan_label, "get wlan label failed!");
        if (static_cast<bool>(std::get<int>(val))) {
            lv_label_set_text(wlan_label, "On");
            forceWlanOperation(WlanOperation::START, 0);
        }
        else {
            lv_label_set_text(wlan_label, "Off");
            forceWlanOperation(WlanOperation::STOP, 0);
        }

        });
    // wlan switch cell
    auto* wlan_switch_cell = ui_.screen_wlan_.getCell(ScreenWlan::ContainerIndex::SWITCH, ScreenWlan::CellIndex::SWITCH);
    bindCellValue(wlan_switch_cell, NVSservice::SETTINGS_WLAN_SWITCH);

    auto* wlan_switch = wlan_switch_cell->getElementObj(CellElement::RIGHT_SWITCH);
    ESP_UTILS_CHECK_NULL_EXIT(wlan_switch, "get wlan label failed!");

    auto* wlan_label = ui_.screen_settings_.getElementObj((int)ScreenSettings::ContainerIndex::WIRELESS, (int)ScreenSettings::CellIndex::WLAN, CellElement::RIGHT_MAIN_LABEL);
    ESP_UTILS_CHECK_NULL_EXIT(wlan_label, "get wlan label failed!");

    ESP_UTILS_CHECK_FALSE_EXIT(nvs_service.getLocalParam(NVSservice::SETTINGS_WLAN_SWITCH, val), "get wlan switch flag val failed!");
    ESP_UTILS_CHECK_FALSE_EXIT(std::holds_alternative<int>(val), "Invalid wlan switch flag type");

    if (static_cast<bool>(std::get<int>(val))) {
        lv_obj_add_state(wlan_switch, LV_STATE_CHECKED);
        lv_label_set_text(wlan_label, "On");
        forceWlanOperation(WlanOperation::START, 0);
    }
    else {
        lv_obj_remove_state(wlan_switch, LV_STATE_CHECKED);
        lv_label_set_text(wlan_label, "Off");
        ui_.screen_wlan_.setAvaliableNetVisible(false);
        ui_.screen_wlan_.setConnectedNetVisible(false);
    }
}

void SettingManager::bindCellToScreen(Cell* cell, ScreenBase& screen)
{
    ESP_UTILS_CHECK_NULL_EXIT(cell, "cell is null!");
    ScreenObj screen_obj = screen.getObj();

    cell_ui_map_[cell] = screen_obj;
}

void SettingManager::bindCellValue(Cell* cell, const std::string& nvs_key)
{
    ESP_UTILS_CHECK_NULL_EXIT(cell, "cell is null!");
    cell_nvs_map_[cell] = nvs_key;
}

void SettingManager::enter(lv_obj_t* screen)
{
    ESP_UTILS_CHECK_NULL_EXIT(screen, "screen is nullptr");
    // 压栈
    screen_stack_.push(lv_scr_act());
    ESP_UTILS_LOGI("screen stack push,size[%d]", screen_stack_.size());
    // 切换屏幕
    lv_scr_load(screen);
}

void SettingManager::back()
{
    // ESP_UTILS_CHECK_FALSE_EXIT(!screen_stack_.empty(), "screen stack is empty!");
    if (screen_stack_.empty()) {
        ESP_UTILS_LOGE("screen stack is empty!");
        return;
    }
    auto* screen = screen_stack_.top();
    screen_stack_.pop();
    ESP_UTILS_LOGI("screen stack pop,remain[%d]", screen_stack_.size());
    lv_scr_load(screen);
}

void SettingManager::initWlan()
{
    // init operation thread
    {
        esp_utils::thread_config_guard thread_config(esp_utils::ThreadConfig{
            .name = WLAN_OPERATION_THREAD_NAME,
            .stack_size = WLAN_OPERATION_THREAD_STACK_SIZE,
            .stack_in_ext = WLAN_OPERATION_THREAD_STACK_CAPS_EXT
            });

        wlan_operation_thread_ = std::thread([this]() {wlanOperationThread();});
    }

    forceWlanOperation(WlanOperation::INIT, 0);
    // triggerWlanOperation(WlanOperation::INIT, 0);

}

void SettingManager::deinitWlan()
{

}

void SettingManager::wlanOperationThread()
{
    while (true) {

        WlanOperation target_state;
        {
            std::unique_lock<std::mutex> lock(wlan_queue_mutex_);
            wlan_queue_cv_.wait(lock, [this]() {return !wlan_queue_.empty();});

            target_state = wlan_queue_.front();
            wlan_queue_.pop();
        }
        is_wlan_operation_stopped_ = false;

        esp_utils::function_guard end_guard([this]() {
            is_wlan_operation_stopped_ = true;
            // 保证等待方已睡眠
            std::unique_lock<std::mutex> stop_lock(wlan_operation_stop_mutex_);
            wlan_operation_stop_cv_.notify_all();
            });

        switch (target_state) {
        case WlanOperation::INIT:
            ESP_UTILS_CHECK_FALSE_EXIT(doWlanOperationInit(), "Do WLAN operation init failed");
            break;
        case WlanOperation::START:
            ESP_UTILS_CHECK_FALSE_EXIT(doWlanOperationStart(), "Do WLAN operation start failed");
            break;
        case WlanOperation::STOP:
            ESP_UTILS_CHECK_FALSE_EXIT(doWlanOperationStop(), "Do WLAN operation stop failed");
            break;
        default:
            break;
        }

    }

}

// 检查当前的状态后再触发入队操作
bool SettingManager::forceWlanOperation(WlanOperation operation, int timeout_ms)
{
    switch (operation) {
    case WlanOperation::INIT:
        if (checkIsWlanState(WlanState::INIT))
            break;
        ESP_UTILS_CHECK_FALSE_RETURN(triggerWlanOperation(operation, timeout_ms), false, "Trigger WLAN operation init failed");
        break;
    case WlanOperation::START:

        if (checkIsWlanState(WlanState::STARTED))
            break;

        // Force WLAN operation initialize first
        ESP_UTILS_CHECK_FALSE_RETURN(
            forceWlanOperation(WlanOperation::INIT, WLAN_INIT_WAIT_TIMEOUT_MS), false,
            "Force WLAN operation init failed"
        );

        ESP_UTILS_CHECK_FALSE_RETURN(
            triggerWlanOperation(operation, timeout_ms), false,
            "Trigger WLAN operation start failed"
        );
        break;
    case WlanOperation::STOP:
        if (!checkIsWlanState(WlanState::_START))
            break;

        ESP_UTILS_CHECK_FALSE_RETURN(
            triggerWlanOperation(operation, timeout_ms), false,
            "Trigger WLAN operation stop failed"
        );
        break;
    default:
        break;
    }
    return true;
}

bool SettingManager::triggerWlanOperation(WlanOperation operation, int timeout_ms)
{
    {
        std::unique_lock<std::mutex> lock(wlan_queue_mutex_);
        wlan_queue_.push(operation);
        wlan_queue_cv_.notify_all();
    }

    if (timeout_ms > 0) {
        std::unique_lock<std::mutex> stop_lock(wlan_operation_stop_mutex_);
        auto status = wlan_operation_stop_cv_.wait_for(stop_lock, std::chrono::milliseconds(timeout_ms), [this]() {return is_wlan_operation_stopped_.load();});
        ESP_UTILS_CHECK_FALSE_RETURN(status, false, "Wait for operation finish timeout");
    }

    return true;
}

bool SettingManager::doWlanOperationInit()
{
    if (wlan_state_.load() & WlanState::INIT) {
        ESP_UTILS_LOGD("Ignore operation!");
        return true;
    }

    esp_utils::function_guard end_guard([this]() {
        ESP_UTILS_CHECK_FALSE_EXIT(doWlanOperationDeinit(), "Do WLAN operation deinit failed!");
        });

    ESP_UTILS_CHECK_ERROR_RETURN(esp_netif_init(), false, "Init netif failed");

    esp_err_t error = esp_event_loop_create_default();
    if (error == ESP_ERR_INVALID_STATE) {
        ESP_UTILS_LOGW("Default event loop already created");
    }
    else {
        ESP_UTILS_CHECK_ERROR_RETURN(error, false, "Create default event loop failed");
    }

    wlan_sta_netif_ = esp_netif_create_default_wifi_sta();
    ESP_UTILS_CHECK_NULL_RETURN(wlan_sta_netif_, false, "craete default STA netif failed");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_UTILS_CHECK_ERROR_RETURN(esp_wifi_init(&cfg), false, "Initialize WLAN failed!");

    ESP_UTILS_CHECK_ERROR_RETURN(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, onWlanEventHandler, this, &wlan_event_handler_instance_), false, "Register Wlan event handler failed");

    ESP_UTILS_CHECK_ERROR_RETURN(esp_event_handler_instance_register(WIFI_EVENT, IP_EVENT_STA_GOT_IP, onWlanEventHandler, this, &ip_event_handler_instance_), false, "Register IP event handler failed");

    ESP_UTILS_CHECK_ERROR_RETURN(esp_wifi_set_mode(WLAN_INIT_MODE_DEFAULT), false, "Set WLAN mode failed");

    wlan_state_ = WlanState::INIT;

    end_guard.release();
    return true;
}

bool SettingManager::doWlanOperationDeinit()
{
    return true;
}

bool SettingManager::doWlanOperationStart()
{
    wlan_state_ = WlanState::STARTING;
    ESP_UTILS_CHECK_ERROR_RETURN(esp_wifi_start(), false, "do Wlan Operation Start failed!");

    ESP_UTILS_CHECK_FALSE_RETURN(waitWlanState(WlanState::STARTED, WLAN_START_WAIT_TIMEOUT_MS), false, "wait wlan started failed!");
    return true;
}

bool SettingManager::doWlanOperationStop()
{
    wlan_state_ = WlanState::STOPPING;
    ESP_UTILS_CHECK_ERROR_RETURN(esp_wifi_stop(), false, "do Wlan Operation Stop failed");

    ESP_UTILS_CHECK_FALSE_RETURN(waitWlanState(WlanState::STOPPED, WLAN_STOP_WAIT_TIMEOUT_MS), false, "wait wlan stopped failed!");
    return true;
}

void SettingManager::onWlanEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_UTILS_CHECK_FALSE_EXIT((event_base == WIFI_EVENT) || (event_base == IP_EVENT), "Invalid event!");

    auto* setting_manager = static_cast<SettingManager*>(event_handler_arg);
    ESP_UTILS_CHECK_NULL_EXIT(setting_manager, "event_handler_arg is null!");

    setting_manager->processOnWlanEventHandler(event_base, event_id, event_data);
}

void SettingManager::processOnWlanEventHandler(esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
        case WIFI_EVENT_STA_START:
            wlan_state_ = WlanState::STARTED;
            break;
        case WIFI_EVENT_STA_STOP:
            wlan_state_ = WlanState::STOPPED;
            break;
        case WIFI_EVENT_STA_DISCONNECTED:

            break;
        default:
            break;
        }
    }
    else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            wlan_state_ = WlanState::CONNECTED;
        }
    }

    std::unique_lock<std::mutex> state_lock(wlan_state_mutex_);
    wlan_state_cv_.notify_all();
}

bool SettingManager::waitWlanState(WlanState state, int timeout_ms)
{

    ESP_UTILS_CHECK_FALSE_RETURN(timeout_ms > 0, false, "Invalid timeout_ms");

    std::unique_lock<std::mutex> state_lock(wlan_state_mutex_);
    auto status = wlan_state_cv_.wait_for(state_lock, std::chrono::milliseconds(timeout_ms), [this, state]() {return checkIsWlanState(state);});

    ESP_UTILS_CHECK_FALSE_RETURN(status, false, "Wait time out!");

    return status;
}



