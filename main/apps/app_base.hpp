#pragma once

#include <string>
#include <lvgl.h>

class Context;
class Manager;

class App {
public:
    friend class Manager;
    App() = default;
    virtual ~App() = default;

    App(const App&) = delete;
    App(App&&) = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&) = delete;

    // Lifecycle methods
    virtual void OnCreate() = 0;
    virtual void OnResume() = 0;
    virtual void OnPause() = 0;
    virtual void OnDestroy() {}

    // App metadata
    virtual int GetAppId() const { return id_; };
    virtual std::string GetAppName() const = 0;
    virtual const lv_img_dsc_t* GetAppIcon() const = 0;  // Font Awesome icon

protected:
    int id_;
    Context* _system_context = nullptr;

};