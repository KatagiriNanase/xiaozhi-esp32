/** 系统上下文资源
 *
 */
#pragma once
#include "event.hpp"
#include "manager.hpp"

 // audio
#include "audio_codec.h"

class Context {
public:
    Context() = default;

    static Context& requestInstance()
    {
        static Context instance;
        return instance;
    }

    Context(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(const Context&) = delete;
    Context& operator=(Context&) = delete;

    Event& getEvent();
    Manager& getManager();
    AudioCodec* getAudioCodec();

    bool setAudioCodec(AudioCodec* codec);

private:
    Event event_;
    Manager manager_;

    // audio 
    AudioCodec* audio_codec_ = nullptr;
};