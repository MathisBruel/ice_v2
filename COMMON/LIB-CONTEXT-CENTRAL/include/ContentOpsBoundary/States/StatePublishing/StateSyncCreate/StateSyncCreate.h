#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"

struct StateSyncCreate : ContentStateBase {
    template <typename Control>
    void entryGuard(Control&) {}
    template <typename Control>
    void enter(Control&) {}
    template <typename Control>
    void update(Control&) {}
    template <typename Control>
    void exit(Control&) {}
    template <typename Control>
    static int select(const Control&) { return 0; }
};
