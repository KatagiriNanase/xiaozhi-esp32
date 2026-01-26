/** 系统上下文资源
 *
 */
#pragma once
#include "event.hpp"
#include "manager.hpp"

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
    // 后续用Display代替
    

private:
    Event event_;
    Manager manager_;
};