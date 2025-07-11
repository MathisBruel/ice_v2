#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"
#include "ContentOpsApp/TransitionResponse.h"
#include "commandCentral.h"
#include "ContentOpsBoundary/Interactions/COB_ArchiveInteraction.h"
#include <iostream>

struct StateArchiveRelease : ContentStateBase {
    using ContentStateBase::react;
    TransitionResponse response;
    
    template <typename Control>
    void enter(Control& control) {
    }
    
    template <typename Control>
    void entryGuard(Control& control) {
    }
    
    template <typename Control>
    void react(const ArchiveCompleteEvent& event, Control& control) {
        throw std::runtime_error("Not implemented");
    }
    
}; 