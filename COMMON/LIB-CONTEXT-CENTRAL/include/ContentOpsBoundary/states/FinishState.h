#pragma once
#include <machine.hpp>

template <typename Owner>
struct FinishState : Owner::State {
    void entryGuard(auto& control) {}
    void enter(auto& control) {}
    void update(auto& control) {}
    void exit(auto& control) {}
}; 