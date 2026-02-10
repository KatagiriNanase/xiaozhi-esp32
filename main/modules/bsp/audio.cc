/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "bsp/esp-bsp.h"
#include "services/nvs_service.hpp"
#include "audio/codecs/box_audio_codec.h"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "Audio"
#include "esp_lib_utils.h"
#include "audio.hpp"
#include "services/context.hpp"

constexpr int AUDIO_INPUT_SAMPLE_RATE = 24000;
constexpr int AUDIO_OUTPUT_SAMPLE_RATE = 24000;
constexpr int SOUND_VOLUME_MAX = 100;
constexpr int SOUND_VOLUME_MIN = 0;
constexpr int SOUND_VOLUME_DEFAULT = 30;

bool audio_init()
{
    ESP_UTILS_LOG_TRACE_GUARD();

    /* Init audio resource */
    static BoxAudioCodec audio_codec(
        bsp_i2c_get_handle(),
        AUDIO_INPUT_SAMPLE_RATE,
        AUDIO_OUTPUT_SAMPLE_RATE,
        BSP_I2S_MCLK,
        BSP_I2S_SCLK,
        BSP_I2S_LCLK,
        BSP_I2S_DOUT,
        BSP_I2S_DSIN,
        BSP_POWER_AMP_IO,
        ES8311_CODEC_DEFAULT_ADDR,
        ES7210_CODEC_DEFAULT_ADDR,
        true);
    Context::requestInstance().setAudioCodec(&audio_codec);

    /* Update media sound volume when NVS volume is updated */
    auto& nvs_service = NVSservice::requestInstance();
    nvs_service.connectEventSignal([&](const NVSservice::Event& event) {
        if ((event.operation != NVSservice::Operation::UpdateNVS) || (event.key != NVSservice::SETTINGS_VOLUME)) {
            return;
        }

        ESP_UTILS_LOG_TRACE_GUARD();

        NVSservice::Value value;
        ESP_UTILS_CHECK_FALSE_EXIT(
            nvs_service.getLocalParam(NVSservice::SETTINGS_VOLUME, value), "Get NVS volume failed"
        );

        auto volume = std::clamp(std::get<int>(value), SOUND_VOLUME_MIN, SOUND_VOLUME_MAX);
        ESP_UTILS_LOGI("Set media sound volume to %d", volume);
        audio_codec.SetOutputVolume(volume);
        });

    /* Initialize media sound volume */
    NVSservice::Value volume = SOUND_VOLUME_DEFAULT;
    if (!nvs_service.getLocalParam(NVSservice::SETTINGS_VOLUME, volume)) {
        ESP_UTILS_LOGW("Volume not found in NVS, set to default value(%d)", std::get<int>(volume));
    }
    nvs_service.setLocalParam(NVSservice::SETTINGS_VOLUME, volume);

    return true;
}
