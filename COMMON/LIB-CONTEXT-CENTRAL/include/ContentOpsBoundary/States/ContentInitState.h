#pragma once
#include <machine.hpp>
#include "ContentOpsBoundary/ContentContext.h"
#include "ContentOpsInfra/MySQLContentRepo.h"
#include <memory>
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsBoundary/FSMTypes.h"
#include "ContentOpsBoundary/States/ContentStateBase.h"
#include "ContentOpsBoundary/States/StatePublishing/StatePublishing.h"
#include "ContentOpsApp/TransitionResponse.h"
#include "ContentOpsApp/ContentInteraction.h"
#include "commandCentral.h"
#include "ContentOpsBoundary/Interactions/COB_ContentInteraction.h"


struct ContentInitState : ContentStateBase {
    template <typename Control>
    void entryGuard(Control& control) {
        std::cout << "ContentInitState entryGuard" << std::endl;
        if(!*control.context()->isNewContent) {
            control.template changeTo<StatePublishing>();
        }
    }

    template <typename Control>
    void enter(Control& control) {
        std::cout << "ContentInitState enter" << std::endl;
        
        // Configuration de l'interaction pour ContentInitState (logique de l'ancien entryGuard)
        auto contentInteraction = dynamic_cast<COB_ContentInteraction*>(control.context()->interactionConfigurator->GetInteractions()[CommandCentral::CREATE_CONTENT]);
        if (contentInteraction) {
            contentInteraction->pfStateContentInit = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
                TransitionResponse response;
                response.cmdUUID = UUID;
                std::string titre = Params["contentTitle"];
                try {
                    control.context()->content = control.context()->contentRepo->Create(titre);
                    response.cmdStatus = "OK";
                    response.cmdComment = "Content created successfully";
                    response.cmdDatasXML = static_cast<std::string>(*control.context()->content);
                    return response;
                } catch (const std::exception& e) {
                    response.cmdStatus = "KO";
                    response.cmdComment = "Failed Content create: " + std::string(e.what());
                    return response;
                }
            };
            
            // Configuration de la transition vers Publishing
            contentInteraction->pfTransitionToPublishing = [control, this]() {
                std::cout << "Transition to Publishing requested" << std::endl;
                // Ici on pourrait déclencher la transition vers StatePublishing
                // mais pour l'instant on laisse la machine à états gérer cela automatiquement
            };
        }
        
    }

    template <typename Control>
    void update(Control& control) {
        std::cout << "ContentInitState update" << std::endl;
        control.template changeTo<StatePublishing>(); 

    }

    template <typename Control>
    void exit(Control& control) {
        std::cout << "ContentInitState exit" << std::endl;
    }


}; 