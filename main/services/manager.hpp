/**
 * 管理App的安装、启动
 */

#pragma once
#include <map>
#include <memory>

#include "app_base.hpp"

class Manager {
public:

    Manager() = default;
    ~Manager();

    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;

    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;

    bool installApp(std::unique_ptr<App> app,int& id);
    bool launchApp(int id);
private:
    std::map<int, std::unique_ptr<App>> app_map_;
    App* current_app_=nullptr;
    int current_app_id_=-1;
    int next_id_ = 0;    // 分配给app的id号
};  