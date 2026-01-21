# XiaoZhi 架构重构方案 - 引入 App 系统

## 一、现状分析

### 1.1 当前架构问题

XiaoZhi 项目当前是一个**单体架构**，所有功能都耦合在 `Application` 类中：

```
Application (单例)
├── 状态管理 (DeviceStateMachine)
├── 协议管理 (Protocol: WebSocket/MQTT)
├── 音频服务 (AudioService)
├── 显示控制 (Display)
├── MCP 服务器 (McpServer)
├── OTA 更新 (Ota)
└── 事件处理 (EventGroup + 各种回调)
```

**主要耦合问题：**

1. **业务逻辑与基础设施紧耦合**
   - AI 对话逻辑、WiFi 配置、OTA、设置等都直接写在 `Application::Run()` 事件循环中
   - 难以添加新的功能模块（如计算器、游戏、音乐播放器）

2. **UI 与业务逻辑混合**
   - `Display` 直接被 `Application` 操作
   - 每个 DeviceState 对应固定的 UI 显示逻辑
   - 无法实现多页面/多场景切换

3. **状态机过于复杂**
   - `DeviceState` 包含了设备状态、业务状态、UI 状态
   - 状态转换逻辑散落在多处
   - 不同功能的共享状态难以管理

4. **无法支持多 App**
   - 只有一个主循环（`Run()`）
   - 没有 App 生命周期管理
   - 没有 App 切换机制

### 1.2 Brookesia 架构优势

ESP-Brookesia 采用了**分层架构 + App 框架**：

```
┌─────────────────────────────────────────┐
│         Application Layer (Apps)        │  ← 各种独立 App
├─────────────────────────────────────────┤
│          System Framework               │  ← App 管理、系统服务
├─────────────────────────────────────────┤
│         Service Layer                   │  ← WiFi/Audio/NVS 等服务
├─────────────────────────────────────────┤
│              HAL (ESP-IDF)              │  ← 硬件抽象
└─────────────────────────────────────────┘
```

**核心设计：**

- **App 基类**：所有 App 继承自 `esp_brookesia::systems::base::App`
- **生命周期管理**：`init()` → `run()` → `back()` → `close()`
- **资源自动管理**：UI 资源自动追踪和释放
- **多 App 注册**：通过 AppManager 注册和切换 App
- **服务化**：基础设施（WiFi/Audio/NVS）作为独立服务

## 二、重构目标

### 2.1 核心目标

1. **将现有 XiaoZhi 功能封装为一个 App**（`XiaozhiApp`）
2. **支持多 App 注册和切换**（类似 Brookesia）
3. **保持向后兼容**：不破坏现有 70+ 板子支持
4. **最小化改动**：渐进式重构，可分步实施

### 2.2 设计原则

- **单一职责**：每个 App 只负责一个功能域
- **依赖注入**：App 通过构造函数/Setter 获取服务依赖
- **事件驱动**：App 间通信通过事件总线
- **资源隔离**：每个 App 拥有独立的 UI 空间

## 三、新架构设计

### 3.1 分层架构

```
┌──────────────────────────────────────────────────────────┐
│                    App Layer                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │ XiaozhiApp   │  │ SettingsApp  │  │  CalculatorApp│ │
│  │  (AI对话)    │  │  (系统设置)  │  │  (计算器)    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
├──────────────────────────────────────────────────────────┤
│                  App Framework                           │
│  ┌──────────────────────────────────────────────────┐   │
│  │        AppManager (App 注册/切换/生命周期)       │   │
│  └──────────────────────────────────────────────────┘   │
├──────────────────────────────────────────────────────────┤
│                  Service Layer                           │
│  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐        │
│  │Audio │ │Network│ │Display│ │  NVS │ │ MCP  │        │
│  │Service│ │Service│ │Service│ │Service│ │Server│       │
│  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘        │
├──────────────────────────────────────────────────────────┤
│                  HAL Layer                               │
│  ┌──────────────────────────────────────────────────┐   │
│  │           Board (硬件抽象，保持现有设计)          │   │
│  └──────────────────────────────────────────────────┘   │
└──────────────────────────────────────────────────────────┘
```

### 3.2 核心类设计

#### 3.2.1 Service 基类

```cpp
// service/base_service.h
class BaseService {
public:
    virtual ~BaseService() = default;
    virtual bool Initialize() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual const char* GetName() const = 0;
};

// ServiceManager - 单例，管理所有服务
class ServiceManager {
public:
    static ServiceManager& GetInstance();

    // 注册服务
    void RegisterService(BaseService* service);

    // 获取服务（类型安全）
    template<typename T>
    T* GetService();

    // 初始化所有服务
    bool InitializeAll();

    // 启动所有服务
    void StartAll();

private:
    std::unordered_map<std::string, BaseService*> services_;
};
```

**具体服务示例：**

```cpp
// service/audio_service.h
class AudioService : public BaseService {
public:
    bool Initialize() override;
    void Start() override;
    void Stop() override;
    const char* GetName() const override { return "AudioService"; }

    // 原有的音频接口
    void EnableWakeWordDetection(bool enable);
    bool IsWakeWordRunning() const;
    // ...
};

// service/display_service.h
class DisplayService : public BaseService {
public:
    bool Initialize() override;
    void Start() override;
    void Stop() override;
    const char* GetName() const override { return "DisplayService"; }

    // 显示接口（封装现有的 Display 类）
    void SetEmotion(const char* emotion);
    void SetChatMessage(const char* role, const char* content);
    void ShowNotification(const char* message, int duration_ms);
    // ...
};

// service/network_service.h
class NetworkService : public BaseService {
public:
    bool Initialize() override;
    void Start() override;
    void Stop() override;
    const char* GetName() const override { return "NetworkService"; }

    // 网络接口（封装 Protocol）
    bool Connect();
    void Disconnect();
    void SendAudio(std::unique_ptr<AudioStreamPacket> packet);
    // ...
};
```

#### 3.2.2 App 基类

```cpp
// app/app.h
class App {
public:
    virtual ~App() = default;

    // App 生命周期
    virtual void Init() = 0;      // 初始化（资源创建）
    virtual void Run() = 0;       // 进入前台，获得控制权
    virtual void Back() = 0;      // 返回键处理
    virtual void Pause() = 0;     // 暂停（失去焦点）
    virtual void Resume() = 0;    // 恢复（重新获得焦点）
    virtual void Close() = 0;     // 关闭（资源释放）

    // App 元数据
    struct Config {
        const char* name;
        const char* icon;          // LVGL 图标符号
        bool show_in_launcher;     // 是否在启动器显示
    };
    virtual const Config& GetConfig() const = 0;

    // 获取 App 的 UI 根对象（LVGL）
    virtual lv_obj_t* GetRootObject() = 0;

protected:
    // 便捷访问服务的辅助方法
    AudioService* audio_service_;
    DisplayService* display_service_;
    NetworkService* network_service_;
    // ...
};
```

#### 3.2.3 AppManager

```cpp
// app/app_manager.h
class AppManager {
public:
    static AppManager& GetInstance();

    // 注册 App
    void RegisterApp(App* app);

    // 启动指定的 App
    void LaunchApp(const char* app_name);

    // 返回到上一个 App
    void GoBack();

    // 返回到主屏幕（Launcher）
    void GoHome();

    // 获取当前 App
    App* GetCurrentApp();

    // 显示启动器（App 列表）
    void ShowLauncher();

private:
    std::unordered_map<std::string, App*> apps_;
    std::vector<App*> app_stack_;           // App 栈（用于返回）
    App* current_app_ = nullptr;
    lv_obj_t* launcher_screen_ = nullptr;
};
```

#### 3.2.4 XiaozhiApp (原有功能封装)

```cpp
// app/xiaozhi_app.h
class XiaozhiApp : public App {
public:
    // 实现 App 接口
    void Init() override;
    void Run() override;
    void Back() override;
    void Pause() override;
    void Resume() override;
    void Close() override;

    const Config& GetConfig() const override {
        static Config config = {
            .name = "Xiaozhi",
            .icon = LV_SYMBOL_KEYBOARD,
            .show_in_launcher = true
        };
        return config;
    }

    lv_obj_t* GetRootObject() override { return root_screen_; }

private:
    // 原有 Application 的功能迁移到这里
    lv_obj_t* root_screen_;

    // UI 组件
    lv_obj_t* status_bar_;
    lv_obj_t* content_area_;
    lv_obj_t* emoji_label_;
    lv_obj_t* chat_message_label_;

    // 状态机（简化版，只管理对话状态）
    enum class ChatState {
        Idle,
        Listening,
        Speaking,
        Thinking
    };
    ChatState chat_state_ = ChatState::Idle;

    // 事件处理（从 Application::Run 迁移）
    void HandleWakeWordDetected();
    void HandleNetworkConnected();
    void HandleStateChanged();

    // UI 更新方法
    void UpdateUIForState(ChatState state);
    void ShowListeningAnimation();
    void ShowSpeakingAnimation();
};
```

### 3.3 事件总线设计

为了解耦 App 间通信，引入事件总线：

```cpp
// event/event_bus.h
class EventBus {
public:
    using EventCallback = std::function<void(const void* data)>;

    static EventBus& GetInstance();

    // 订阅事件
    int Subscribe(const char* event_type, EventCallback callback);

    // 取消订阅
    void Unsubscribe(int subscription_id);

    // 发布事件
    void Publish(const char* event_type, const void* data);

private:
    struct Subscription {
        int id;
        const char* event_type;
        EventCallback callback;
    };
    std::vector<Subscription> subscriptions_;
    int next_id_ = 0;
};

// 定义标准事件类型
namespace Events {
    constexpr const char* WAKE_WORD_DETECTED = "wake_word_detected";
    constexpr const char* NETWORK_CONNECTED = "network_connected";
    constexpr const char* NETWORK_DISCONNECTED = "network_disconnected";
    constexpr const char* CHAT_MESSAGE_RECEIVED = "chat_message_received";
    constexpr const char* APP_LAUNCHED = "app_launched";
    constexpr const char* APP_CLOSED = "app_closed";
}
```

## 四、重构步骤

### 阶段 1：基础设施重构（不破坏现有功能）

**目标：** 建立 Service 层和 App 框架，现有代码继续工作

1. **创建 Service 层**
   - 创建 `service/` 目录
   - 实现 `ServiceManager` 和 `BaseService`
   - 将 `AudioService`、`McpServer` 包装为服务
   - **保持现有接口兼容**

2. **创建 App 框架**
   - 创建 `app/` 目录
   - 实现 `App` 基类和 `AppManager`
   - 实现简单的 `LauncherApp`（显示 App 网格）

3. **重构 Application 类**
   - 将 `Application` 改为使用 `ServiceManager`
   - `Application::Initialize()` 初始化所有服务
   - `Application::Run()` 启动 `LauncherApp`
   - **保持原有事件处理逻辑**

**验证：** 现有功能正常工作，可以进入 App Launcher

### 阶段 2：XiaozhiApp 封装

**目标：** 将现有对话功能封装为独立 App

1. **创建 XiaozhiApp**
   - 继承 `App` 基类
   - 迁移 `Application` 的 UI 代码到 `XiaozhiApp::Init()`
   - 迁移状态机逻辑到 `XiaozhiApp` 内部
   - 实现 `Run()`、`Back()`、`Close()`

2. **迁移事件处理**
   - 将 `Application::Run()` 中的事件处理移到 `XiaozhiApp`
   - 使用 `EventBus` 订阅系统事件
   - `XiaozhiApp` 通过 `ServiceManager` 获取服务

3. **UI 改造**
   - 创建独立的 LVGL Screen 作为 App 根对象
   - 实现页面切换动画（淡入淡出）
   - 支持全屏显示和返回按钮

**验证：** XiaozhiApp 可以正常对话，可以从 Launcher 启动和返回

### 阶段 3：添加新 App

**目标：** 验证 App 架构的扩展性

1. **实现 SettingsApp**
   - 创建设置界面（音量、网络、OTA、语言）
   - 使用 `DisplayService` 和 `NetworkService`
   - 通过 `EventBus` 通知其他 App 设置变更

2. **实现 CalculatorApp**（示例）
   - 简单的计算器 UI
   - 独立的页面布局
   - 不依赖其他服务

**验证：** 多个 App 可以正常切换，互不干扰

### 阶段 4：清理和优化

**目标：** 移除旧代码，优化架构

1. **移除冗余代码**
   - 清理 `Application` 类中的旧逻辑
   - 移除全局单例（除 `ServiceManager`、`AppManager`）
   - 统一使用服务注入

2. **优化资源管理**
   - 实现 App 资源自动释放
   - 优化内存占用
   - 添加内存监控

3. **文档和示例**
   - 编写 App 开发指南
   - 添加示例 App 模板
   - 更新 README

## 五、目录结构（重构后）

```
main/
├── app/                        # App 层
│   ├── app.h                  # App 基类
│   ├── app_manager.h          # App 管理器
│   ├── launcher_app.cc/h      # 启动器 App
│   ├── xiaozhi_app.cc/h       # 小智对话 App
│   ├── settings_app.cc/h      # 设置 App
│   └── calculator_app.cc/h    # 计算器 App（示例）
│
├── service/                    # 服务层
│   ├── base_service.h         # 服务基类
│   ├── service_manager.h      # 服务管理器
│   ├── audio_service.cc/h     # 音频服务（封装现有 AudioService）
│   ├── display_service.cc/h   # 显示服务（封装 Display）
│   ├── network_service.cc/h   # 网络服务（封装 Protocol）
│   ├── nvs_service.cc/h       # NVS 存储服务
│   └── mcp_service.cc/h       # MCP 服务
│
├── event/                      # 事件系统
│   ├── event_bus.h            # 事件总线
│   └── events.h               # 标准事件定义
│
├── boards/                     # 硬件抽象层（保持不变）
│   ├── common/
│   └── echoear/
│
├── audio/                      # 音频组件（保持不变）
├── display/                    # 显示组件（保持不变）
├── protocols/                  # 协议实现（保持不变）
│
├── application.cc/h            # 主入口（简化版）
├── device_state_machine.cc/h   # 保留（可能简化）
└── main.cc                     # 程序入口
```

## 六、关键问题和解决方案

### 6.1 DeviceStateMachine 如何处理？

**问题：** 原有的全局状态机包含设备状态、业务状态、UI 状态

**解决方案：**
- **设备级状态**（如启动、升级、错误）保留在 `Application` 中
- **App 内状态**（如对话、监听）下放到各个 App 内部
- **UI 状态**由 LVGL 屏幕栈管理

### 6.2 Board 单例如何与 Service 共存？

**问题：** `Board::GetInstance()` 是硬件抽象，不应与 Service 耦合

**解决方案：**
- `Board` 保持单例，作为最底层 HAL
- Service 通过 Board 访问硬件
- `Board` 不直接被 App 访问，只通过 Service 间接访问

### 6.3 现有的 Display 类如何改造？

**问题：** `LcdDisplay` 继承 `LvglDisplay`接口较重

**解决方案：**
- 保持 `LcdDisplay` 类不变
- `DisplayService` 持有 `LcdDisplay*` 实例
- 提供简化接口供 App 使用
- App 不直接操作 `LcdDisplay`

### 6.4 事件循环如何处理？

**问题：** 原有 `Application::Run()` 是单一事件循环

**解决方案：**
- 主循环保留在 `Application::Run()` 中
- 负责分发系统级事件（网络、音频、定时器）
- App 通过 `EventBus` 订阅事件
- UI 事件由 LVGL 自己的事件循环处理

### 6.5 内存管理如何优化？

**问题：** 多 App 会增加内存占用

**解决方案：**
- 实现按需加载：只初始化当前 App
- App 暂停时释放非关键资源
- 使用 `LVGL` 的对象池减少碎片
- 监控内存使用，超过阈值时关闭后台 App

## 七、Brookesia 集成方案（可选）

如果未来需要与 Brookesia 深度集成：

### 7.1 适配器模式

```cpp
// 将 XiaoZhi 的 Service 适配为 Brookesia 的 Service
class BrookesiaServiceAdapter : public brookesia::service::Base {
public:
    BrookesiaServiceAdapter(xiaozhi::BaseService* service)
        : xiaozhi_service_(service) {}

    bool Init() override { return xiaozhi_service_->Initialize(); }
    // ...

private:
    xiaozhi::BaseService* xiaozhi_service_;
};
```

### 7.2 使用 Brookesia 的 App 框架

- 将 `XiaozhiApp` 改为继承 `brookesia::systems::base::App`
- 使用 Brookesia 的资源管理系统
- 兼容 Brookesia 的 Squareline Studio 导出的 UI

## 八、时间估算（仅供参考）

| 阶段 | 任务 | 预估时间 |
|------|------|----------|
| 阶段 1 | 基础设施重构 | 3-5 天 |
| 阶段 2 | XiaozhiApp 封装 | 5-7 天 |
| 阶段 3 | 添加新 App | 3-5 天 |
| 阶段 4 | 清理和优化 | 2-3 天 |
| **总计** | | **13-20 天** |

## 九、风险评估

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 破坏现有功能 | 高 | 渐进式重构，每阶段都测试 |
| 内存不足 | 中 | 优化资源管理，按需加载 |
| 架构过度设计 | 中 | 保持简单，先实现 MVP |
| 兼容性问题 | 低 | 保持 Board 层不变 |

## 十、下一步行动

1. **审查此方案**，确认是否符合预期
2. **详细设计** `ServiceManager` 和 `AppManager` 的类图
3. **原型验证**：先实现一个最小化的 Demo（2 个简单 App 的切换）
4. **制定测试计划**：确保每个阶段不破坏现有功能

---

**附录：参考 Brookesia 的关键文件**

- `/home/nanase/github/esp-brookesia/core/brookesia_core/` - 核心框架
- `/home/nanase/github/esp-brookesia/service/` - 服务实现
- `/home/nanase/github/esp-brookesia/apps/` - 示例 App
- `/home/nanase/github/esp-brookesia/products/` - 产品级实现
