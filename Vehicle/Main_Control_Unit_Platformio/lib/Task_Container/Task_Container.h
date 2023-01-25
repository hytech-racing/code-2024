#pragma once
#include <unordered_map>
#include <functional>

class Task_Container {
private:
    std::unordered_map<uint32_t, std::pair<std::function<void(void)>, uint32_t>> tasks;

public:
    void run();
    void emplace(uint32_t ID, const std::function<void(void)> &fn, uint32_t refresh_ms);
    void remove(uint32_t ID);
};
