#pragma once

#include <unordered_map>
#include <functional>
#include <string>

#define INSTANCE_FUNC(fn) [this]() { (fn)(); }

class Task_Container {
private:
    std::unordered_map<std::string, std::pair<std::function<void(void)>, uint32_t>> tasks;

public:
    void run();

    void emplace(const std::string &ID, const std::function<void(void)> &fn, uint32_t refresh_ms);

    void remove(const std::string &ID);
};

extern Task_Container taskContainer;
