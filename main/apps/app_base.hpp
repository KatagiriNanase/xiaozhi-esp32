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
    virtual void onCreate() = 0;
    virtual void onResume() = 0;
    virtual void onPause() = 0;
    virtual void onDestroy() {}

    // App metadata
    virtual int getAppId() const { return id_; };
    virtual std::string getAppName() const = 0;
    virtual const lv_img_dsc_t* getAppIcon() const = 0;  // Font Awesome icon
    virtual lv_obj_t* getObj() = 0;

protected:
    int id_;
    Context* _system_context = nullptr;

};