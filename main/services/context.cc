#include "context.hpp"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "Context"
#include "esp_lib_utils.h"

Event& Context::getEvent()
{
    return event_;
}

Manager& Context::getManager()
{
    return manager_;
}

AudioCodec* Context::getAudioCodec()
{
    ESP_UTILS_CHECK_NULL_RETURN(audio_codec_, nullptr, "audio_codec is null!");
    return audio_codec_;
}

bool Context::setAudioCodec(AudioCodec* codec)
{
    ESP_UTILS_CHECK_NULL_RETURN(codec, false, "audio codec is null");
    audio_codec_ = codec;
    return true;
}
