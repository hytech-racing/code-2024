#pragma once

#include <vector>
#include <functional>

template<typename T>
struct MC_Container {
    T FL{}, FR{}, RL{}, RR{};

private:
    std::vector<std::reference_wrapper<T>> vec{std::ref(FL), std::ref(FR), std::ref(RL), std::ref(RR)};

public:
    std::vector<std::reference_wrapper<T>>& iter() {
        return vec;
    }

    auto begin() {
        return vec.begin();
    }

    auto end() {
        return vec.end();
    }
};