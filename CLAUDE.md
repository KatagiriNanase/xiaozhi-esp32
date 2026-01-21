# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

XiaoZhi ESP32 AI Chatbot is a voice-interactive AI assistant firmware for ESP32 microcontrollers. It integrates with large language models (Qwen, DeepSeek) via the MCP (Model Context Protocol) protocol to enable multi-terminal control and natural language interaction.

**Key Features:**
- Offline voice wake-up using ESP-SR
- WebSocket or MQTT+UDP communication protocols
- OPUS audio codec for streaming audio
- Speaker recognition (3D Speaker)
- OLED/LCD display support with emoji rendering
- Battery management and power control
- Multi-language support (Chinese, English, Japanese, Korean, Vietnamese, Thai)
- Device-side MCP for hardware control (GPIO, Servo, LED, Speaker)
- Cloud-side MCP for extending LLM capabilities
- Support for 70+ hardware boards (ESP32, ESP32-S3, ESP32-C3, ESP32-P4)

## Build System

This project uses ESP-IDF 5.4+ with CMake.

### Essential Build Commands

```bash
# Configure the project (select board type, language, etc.)
idf.py menuconfig

# Build the firmware
idf.py build

# Flash to device
idf.py -p /dev/ttyUSB0 flash

# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor

# Build, flash and monitor in one command
idf.py -p /dev/ttyUSB0 flash monitor

# Clean build
idf.py fullclean

# Build for specific target (if not auto-detected)
idf.py set-target esp32s3
```

### Configuration

- **Board selection**: `idf.py menuconfig` → "Xiaozhi Assistant" → "Board Type"
- **Language**: `idf.py menuconfig` → "Xiaozhi Assistant" → "Default Language"
- **Partition table**: Uses custom tables in `partitions/v2/` (16MB, 8MB, 4MB variants)
- **SDK configuration**: Multiple `sdkconfig.defaults.*` files for different chips

## Architecture

The codebase follows a modular, event-driven architecture suitable for real-time audio processing on embedded systems.

### Core Components

**main/** - Application source
- `main.cc` - Entry point, initializes board and application
- `application.cc/h` - Main application logic and state machine
- `device_state_machine.cc/h` - Device state management
- `mcp_server.cc/h` - Device-side MCP protocol implementation
- `ota.cc/h` - Over-the-air update functionality
- `assets.cc/h` - Asset management (fonts, emojis, wake words)

**main/boards/** - Hardware abstraction layer (70+ boards)
- `common/board.h` - Base Board class interface
- `common/` - Shared hardware drivers (power management, WiFi, battery, etc.)
- `<board-name>/config.h` - Board-specific configuration

Each board must implement:
```cpp
class Board {
    virtual AudioCodec* GetAudioCodec() = 0;
    virtual Display* GetDisplay();
    virtual NetworkInterface* GetNetwork() = 0;
    virtual void StartNetwork() = 0;
    // ... see boards/common/board.h for full interface
};
```

**main/audio/** - Audio processing pipeline
```
MIC → [Processors] → Encode Queue → Opus Encoder → Send Queue → Server
Server → Decode Queue → Opus Decoder → Playback Queue → Speaker
```

- `audio_service.cc/h` - Main audio service managing encoding/decoding
- `audio_codec.h` - Audio codec abstraction
- `codecs/` - Hardware-specific codec implementations (ES8311, ES8374, ES8388, etc.)
- `processors/` - Audio processors (AEC, NS, VAD)
- `wake_words/` - Wake word detection models

**main/display/** - Display subsystem
- `display.h` - Display abstraction
- `oled_display.cc` - Monochrome OLED (SSD1306)
- `lcd_display.cc` - Color LCD with LVGL
- `lvgl_display/` - LVGL-based UI with emoji/GIF/JPEG support

**main/protocols/** - Network communication
- `protocol.h` - Base protocol interface
- `websocket_protocol.cc` - WebSocket implementation
- `mqtt_protocol.cc` - MQTT+UDP hybrid protocol

Binary protocol format (version 2):
```cpp
struct BinaryProtocol2 {
    uint16_t version;
    uint16_t type;          // 0: OPUS audio, 1: JSON
    uint32_t reserved;
    uint32_t timestamp;     // For server-side AEC
    uint32_t payload_size;
    uint8_t payload[];
};
```

**main/led/** - LED control
- `single_led.cc` - Single LED control
- `circular_strip.cc` - Circular LED strip (RGB)

### MCP (Model Context Protocol) Integration

The device implements an MCP server that exposes tools/resources to the LLM:

- **Tools**: GPIO control, Servo control, LED control, Speaker control
- **Resources**: Device status, board configuration, camera images
- **JSON-RPC**: Messages sent via WebSocket/MQTT to cloud MCP server

See `main/mcp_server.h` for the server implementation.

## Adding Support for a New Board

1. Create directory `main/boards/<board-name>/`
2. Create `config.h` defining board configuration:
   ```cpp
   #define BOARD_NAME "My Board"
   #define I2C_SDA 10
   #define I2C_SCL 11
   // ... pin definitions
   ```
3. Implement Board class (usually by extending a common base):
   ```cpp
   class MyBoard : public WifiBoard {
       // Implement required virtual methods
   };
   ```
4. Add to `main/Kconfig.projbuild`:
   ```
   config BOARD_TYPE_MY_BOARD
       bool "My Board"
   ```
5. Add board selection logic in `main/CMakeLists.txt`
6. Update `docs/custom-board.md` with user-facing documentation

## Code Style

- **Language**: C++17
- **Style**: Google C++ Style Guide
- **Naming**:
  - Classes: `PascalCase`
  - Functions/Methods: `PascalCase()`
  - Member variables: `trailing_underscore_`
  - Constants: `kPascalCase`
- **File naming**: `snake_case.cc` and `snake_case.h`
- **Memory management**: Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- **Concurrency**: FreeRTOS tasks, mutexes, condition variables

## Asset Generation

Custom assets (wake words, fonts, emojis, backgrounds) are embedded binary files:

```bash
# Generate default assets
python3 scripts/build_default_assets.py

# The script generates assets.bin containing:
# - Wake word models (esp-sr)
# - Fonts (TTF → binary)
# - Emoji images (PNG → LVGL C arrays)
# - UI assets
```

Asset files are loaded from SPIFFS at runtime. The project uses a v2 partition table that separates assets from firmware.

## Common Pitfalls

1. **Memory constraints**: ESP32 has limited RAM. Monitor stack usage and avoid large allocations on the stack. Use `heap_caps_malloc()` for specific memory types when needed.

2. **Audio timing**: The audio pipeline is timing-sensitive. Long-running operations in audio tasks will cause buffer underruns/overruns. Keep audio task code short and non-blocking.

3. **FreeRTOS stack sizes**: Default stack sizes may be insufficient for complex operations. Increase in `sdkconfig` or via `xTaskCreate()` parameters.

4. **GPIO initialization order**: Some hardware components require specific initialization sequences (e.g., I2C before display, audio codec before speaker).

5. **OTA limitations**: V2 firmware cannot OTA upgrade from V1 due to partition table changes. Manual flashing is required for V1→V2 migration.

## Testing

The project uses pytest for testing managed components. Test files are located in `managed_components/<component>/test_apps/`.

```bash
# Run tests for a specific component
pytest managed_components/espressif__button/test_apps/pytest_button.py
```

## Key Dependencies

- **ESP-IDF 5.4+**: Core SDK
- **ESP-SR**: Speech recognition and wake word detection
- **LVGL**: Graphics library for LCD displays
- **esp_audio_codec**: Audio codec abstraction
- **esp_audio_effects**: Audio processing (AEC, NS)
- **cJSON**: JSON parsing
- **mbedTLS**: SSL/TLS and base64 encoding

## Configuration Files

- `sdkconfig.defaults` - Base configuration
- `sdkconfig.defaults.esp32*` - Chip-specific overrides
- `main/Kconfig.projbuild` - Project configuration options
- `dependencies.lock` - Component versions (managed by ESP Component Manager)
- `main/idf_component.yml` - Component dependencies

## Serial Monitoring

The firmware outputs detailed logs via USB serial. Use `idf.py monitor` or external tools:

- Default baud rate: 115200
- Log levels configurable via `idf.py menuconfig`
- Useful tags: `AUDIO`, `NETWORK`, `DISPLAY`, `MCP`, `BOARD`
