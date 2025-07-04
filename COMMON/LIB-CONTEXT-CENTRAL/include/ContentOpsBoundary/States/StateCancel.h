#pragma once
#include "ContentStateBase.h"
#include <iostream>

struct StateCancel : ContentStateBase {
    template <typename Control>
    void entryGuard(Control&) {}
    template <typename Control>
    void enter(Control&)  { std::cout << "Cancel\n"; }
    template <typename Control>
    void update(Control&) {}
    template <typename Control>
    void exit(Control&) {}
};
