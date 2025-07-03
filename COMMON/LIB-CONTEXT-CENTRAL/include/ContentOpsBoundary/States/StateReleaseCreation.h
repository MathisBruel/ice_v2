#pragma once
#include "ContentStateBase.h"

struct StateReleaseCreation : ContentStateBase {
    template <typename Control>
    void entryGuard(Control&) {}
    template <typename Control>
    void enter(Control&) {}
    template <typename Control>
    void update(Control&) {}
    template <typename Control>
    void exit(Control&) {}
};
