#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"

struct StateSync : ContentStateBase {
    template <typename Control>
    void entryGuard(Control&) {}
    template <typename Control>
    void enter(Control&) {}
    template <typename Control>
    void update(Control&) {}
    template <typename Control>
    void exit(Control&) {}
};
