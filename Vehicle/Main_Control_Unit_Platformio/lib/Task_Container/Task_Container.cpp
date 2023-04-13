#include "Task_Container.h"
#include <Arduino.h>

void Task_Container::run() {
    static uint32_t prev_time = millis();
    auto curr_time = millis();
    for (const auto &i: tasks) {
        auto val = i.second;
        if (curr_time - prev_time > val.second) {
            val.first();
        }
    }
    prev_time = curr_time;
}

void Task_Container::emplace(const std::string &ID, const std::function<void(void)> &fn, uint32_t refresh_ms) {
    tasks.emplace(ID, std::make_pair(fn, refresh_ms));
}

void Task_Container::remove(const std::string &ID) {
    auto it = tasks.find(ID);
    if (it != tasks.cend()) {
        tasks.erase(it);
    }
}

Task_Container taskContainer{};
