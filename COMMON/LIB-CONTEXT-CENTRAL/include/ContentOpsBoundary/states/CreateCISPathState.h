#pragma once
#include <machine.hpp>
#include "ContentOpsBoundary/BoundaryManager.h"
#include "ContentOpsBoundary/states/FinishState.h"

template <typename Owner>
struct CreateCISPathState : Owner::State {
    void entryGuard(auto& control) {
        if(!*control.context()->isNewContent) {
            COB_Releases releases = control.context()->releaseRepo->GetReleases(*control.context()->content->GetContentIdPtr());
            if(!releases[0].GetCISPath().empty()) {
                
            }
        }
    
    }
    void enter(auto& control) {
        std::cout << "CreateCISPathState enter" << std::endl;
    }
    void update(auto& control) {
        std::cout << "CreateCISPathState update" << std::endl;        
    }
    void exit(auto& control) {}
}; 