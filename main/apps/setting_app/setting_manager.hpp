#pragma once
#include "setting_ui.hpp"
#include "services/nvs_service.hpp"

// STL
#include <queue>
#include <stack>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <unordered_map>

#include "esp_wifi.h"
#include "esp_event.h"

class SettingManager {
public:
    using ScreenObj = lv_obj_t*;

    SettingManager(SettingUI& ui);

    void init();
    void run();
    void del();
private:
    enum WlanState {
        DEINIT = (1UL << 0),
        INIT = (1UL << 1),
        _START = (INIT | (1UL << 2)),
        STARTING = (_START | (1UL << 3)),
        STARTED = (_START | (1UL << 4)),
        _CONNECT = (STARTED | (1UL << 5)),
        CONNECTING = (_CONNECT | (1UL << 6)),
        CONNECTED = (_CONNECT | (1UL << 7)),
        _STOP = (INIT | (1UL << 8)),
        STOPPING = (_STOP | (1UL << 9)),
        STOPPED = (_STOP | (1UL << 10)),
        _DISCONNECT = (STARTED | (1UL << 11)),
        DISCONNECTING = (_DISCONNECT | (1UL << 12)),
        DISCONNECTED = (_DISCONNECT | (1UL << 13))
    };

    enum class WlanOperation {
        DEINIT,
        INIT,
        START,
        STOP,
    };

    void processEventOnRun();
    void processScreenSettings();
    void processScreenSound();
    void processScreenDisplay();
    void processScreenWlan();
    void bindCellToScreen(Cell*, ScreenBase&);
    void bindCellValue(Cell*, const std::string& nvs_key);
    void enter(lv_obj_t*);
    void back();

    // Wlan
    void initWlan();
    void deinitWlan();
    void wlanOperationThread();
    bool forceWlanOperation(WlanOperation operation, int timeout_ms);
    bool triggerWlanOperation(WlanOperation operation, int timeout_ms);
    bool doWlanOperationInit();
    bool doWlanOperationDeinit();
    bool doWlanOperationStart();
    bool doWlanOperationStop();
    void processOnWlanEventHandler(esp_event_base_t event_base, int32_t event_id, void* event_data);
    bool waitWlanState(WlanState state, int timeout_ms);
    // “看看你的身体内有没有我”
    bool checkIsWlanState(WlanState state) { return (wlan_state_ & state) == state; }
    static void onWlanEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    // Wlan
    std::atomic<WlanState> wlan_state_ = WlanState::DEINIT;
    std::mutex wlan_state_mutex_;
    std::condition_variable wlan_state_cv_;
    std::queue<WlanOperation> wlan_queue_;
    std::mutex wlan_queue_mutex_;
    std::condition_variable wlan_queue_cv_;
    std::atomic<bool> is_wlan_operation_stopped_ = true;
    std::mutex wlan_operation_stop_mutex_;
    std::condition_variable wlan_operation_stop_cv_;
    std::thread wlan_operation_thread_;
    esp_netif_t* wlan_sta_netif_ = nullptr;
    esp_event_handler_instance_t wlan_event_handler_instance_;
    esp_event_handler_instance_t ip_event_handler_instance_;

    std::stack<lv_obj_t*> screen_stack_;
    std::unordered_map<Cell*, ScreenObj> cell_ui_map_;
    std::unordered_map<Cell*, NVSservice::Key> cell_nvs_map_;
    SettingUI& ui_;
};