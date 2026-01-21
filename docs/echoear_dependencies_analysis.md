# EchoEar 板子依赖分析

## 一、EchoEar 直接依赖的 common 文件

根据代码分析，EchoEar 板子**直接使用**的 common 文件：

### 必需文件（✅ 保留）

| 文件 | 用途 | 分析 |
|------|------|------|
| `board.h` | Board 基类定义 | ✅ 核心基类，所有板子都需要 |
| `board.cc` | Board 基类实现 | ✅ 核心基类，所有板子都需要 |
| `wifi_board.h` | WiFi 板子基类 | ✅ EchoEar 继承自 WifiBoard |
| `wifi_board.cc` | WiFi 功能实现 | ✅ WiFi 连接、BLUFI、网络配置 |
| `button.h` | 按键驱动 | ✅ EchoEar 使用按键 |
| `button.cc` | 按键实现 | ✅ 按键处理 |
| `backlight.h` | 背光控制 | ✅ EchoEar 使用 LCD 背光 |
| `backlight.cc` | 背光实现 | ✅ PWM 背光控制 |
| `i2c_device.h` | I2C 设备抽象 | ✅ 用于触摸屏和充电 IC |
| `i2c_device.cc` | I2C 设备实现 | ✅ I2C 读写封装 |
| `esp32_camera.h` | 摄像头驱动 | ✅ EchoEar 有摄像头 |
| `esp32_camera.cc` | 摄像头实现 | ✅ 摄像头初始化和控制 |

### 未直接使用的文件（❌ 可删除）

| 文件 | 用途 | EchoEar 是否使用 |
|------|------|-----------------|
| `adc_battery_monitor.h/cc` | ADC 电池监控 | ❌ EchoEar 使用 I2C 充电 IC（BQ25896） |
| `afsk_demod.h/cc` | 音频解调（声控 WiFi） | ❌ EchoEar 不使用声控 WiFi 配置 |
| `axp2101.h/cc` | AXP2101 PMIC | ❌ EchoEar 使用不同的电源管理 IC |
| `dual_network_board.h/cc` | 双网络板（WiFi+4G） | ❌ EchoEar 只有 WiFi |
| `knob.h/cc` | 旋钮编码器 | ❌ EchoEar 没有旋钮 |
| `lamp_controller.h` | 灯控制器 | ❌ EchoEar 没有灯控制功能 |
| `ml307_board.h/cc` | ML307 4G 模块 | ❌ EchoEar 不支持 4G |
| `power_save_timer.h/cc` | 省电定时器 | ❌ EchoEar 没有省电功能 |
| `press_to_talk_mcp_tool.h/cc` | 按键通话 MCP 工具 | ❌ EchoEar 不使用这个 MCP 工具 |
| `sleep_timer.h/cc` | 睡眠定时器 | ❌ EchoEar 没有睡眠功能 |
| `sy6970.h/cc` | SY6970 充电 IC | ❌ EchoEar 使用 BQ25896 |
| `system_reset.h/cc` | 系统复位 | ❌ EchoEar 没有独立的复位控制 |
| `camera.h` | 摄像头抽象（旧版） | ⚠️ 已被 `esp32_camera.h/cc` 替代 |

## 二、间接依赖（WiFi 板基类）

`wifi_board.cc` 可能间接使用了：

| 文件 | WiFiBoard 是否使用 | 分析 |
|------|-------------------|------|
| `blufi.h/cpp` | ✅ 可能 | WiFiBoard 支持 BLUFI 配置 |
| `adc_battery_monitor.h/cc` | ❓ 可选 | 如果需要电池电量显示 |

## 三、建议删除的文件列表

### 高优先级删除（100% 确认不需要）

```bash
# 双网络板（WiFi+4G）
main/boards/common/dual_network_board.h
main/boards/common/dual_network_board.cc

# ML307 4G 模块
main/boards/common/ml307_board.h
main/boards/common/ml307_board.cc

# 其他电源管理 IC
main/boards/common/axp2101.h
main/boards/common/axp2101.cc
main/boards/common/sy6970.h
main/boards/common/sy6970.cc

# 旋钮编码器
main/boards/common/knob.h
main/boards/common/knob.cc

# 灯控制器
main/boards/common/lamp_controller.h

# 音频解调（声控 WiFi）
main/boards/common/afsk_demod.h
main/boards/common/afsk_demod.cc

# 定时器相关
main/boards/common/power_save_timer.h
main/boards/common/power_save_timer.cc
main/boards/common/sleep_timer.h
main/boards/common/sleep_timer.cc

# MCP 工具
main/boards/common/press_to_talk_mcp_tool.h
main/boards/common/press_to_talk_mcp_tool.cc

# 系统复位
main/boards/common/system_reset.h
main/boards/common/system_reset.cc

# 旧版摄像头抽象
main/boards/common/camera.h  # 已被 esp32_camera.h 替代
```

### 中优先级删除（需要确认）

```bash
# ADC 电池监控（如果 EchoEar 不使用）
main/boards/common/adc_battery_monitor.h
main/boards/common/adc_battery_monitor.cc
```

## 四、删除命令

```bash
cd /home/nanase/workspace/esp_proj/xiaozhi-esp32-multiapp/main/boards/common

# 高优先级删除
rm -f dual_network_board.h dual_network_board.cc
rm -f ml307_board.h ml307_board.cc
rm -f axp2101.h axp2101.cc
rm -f sy6970.h sy6970.cc
rm -f knob.h knob.cc
rm -f lamp_controller.h
rm -f afsk_demod.h afsk_demod.cc
rm -f power_save_timer.h power_save_timer.cc
rm -f sleep_timer.h sleep_timer.cc
rm -f press_to_talk_mcp_tool.h press_to_talk_mcp_tool.cc
rm -f system_reset.h system_reset.cc
rm -f camera.h

# 中优先级删除（需要确认）
# rm -f adc_battery_monitor.h adc_battery_monitor.cc
```

## 五、保留的文件列表

```bash
# 核心基类
board.h
board.cc
wifi_board.h
wifi_board.cc

# 硬件驱动
button.h
button.cc
backlight.h
backlight.cc
i2c_device.h
i2c_device.cc
esp32_camera.h
esp32_camera.cc

# 可选保留
blufi.h
blufi.cpp
adc_battery_monitor.h  # 如果需要显示电池电量
adc_battery_monitor.cc
```

## 六、编译验证

删除文件后，需要编译验证：

```bash
cd /home/nanase/workspace/esp_proj/xiaozhi-esp32-multiapp
idf.py set-target esp32s3
idf.py menuconfig  # 选择 EchoEar 板子
idf.py build
```

如果编译失败，可能需要：
1. 检查 `wifi_board.cc` 是否间接使用了某些文件
2. 检查 `main/CMakeLists.txt` 中的 `BOARD_COMMON_SOURCES` 是否需要更新

## 七、Kconfig.projbuild 清理

删除板子后，还需要清理 `main/Kconfig.projbuild` 中的相关配置：

```bash
# 删除其他板子的配置选项
# 只保留：
# - BOARD_TYPE_ECHOEAR
# - DISPLAY_STYLE (Emote 风格)
# - WAKE_WORD_TYPE (AFE 唤醒)
# - USE_AUDIO_PROCESSOR
# - USE_DEVICE_AEC
# - 相关的 WiFi 配置选项
```

## 八、预计减少的代码量

- **删除的 common 文件**：约 14-18 个文件
- **减少的代码行数**：约 3000-4000 行
- **减少的编译时间**：约 10-15%
