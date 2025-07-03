#pragma once
#include <machine.hpp>
#include "ContentOpsBoundary/ContentContext.h"
#include "ContentOpsInfra/MySQLContentRepo.h"
#include <memory>
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsBoundary/FSMTypes.h"
#include "ContentOpsBoundary/States/ContentStateBase.h"

struct ContentInitState : ContentStateBase {
    template <typename Control>
    void entryGuard(Control& control) {
        std::cout << "ContentInitState entryGuard" << std::endl;
        if(!*control.context()->isNewContent) {
            //control.template changeTo<CreateCISPathState>();
        }
    }

    template <typename Control>
    void enter(Control& control) {
        std::cout << "ContentInitState enter" << std::endl;
    }

    template <typename Control>
    void update(Control& control) {
        std::cout << "ContentInitState update" << std::endl;
        std::string titre = *control.context()->pendingTitle;
        if (control.context()->contentRepo) {
            control.context()->content = control.context()->contentRepo->Create(titre);
        }
        //control.template changeTo<CreateCISPathState>(); 
    }

    template <typename Control>
    void exit(Control& control) {
        std::cout << "ContentInitState exit" << std::endl;
    }
}; 