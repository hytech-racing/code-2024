#pragma once

#include <vector>
#include <functional>

template<typename T>
struct MC_Container {
    T FL{}, FR{}, RL{}, RR{};

    std::vector<std::reference_wrapper<T>> iter() {
        return {std::ref(FL), std::ref(FR), std::ref(RL), std::ref(RR)};
    }
};