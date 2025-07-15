#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"
#include "ContentOpsApp/TransitionResponse.h"
#include "commandCentral.h"
#include "ContentOpsBoundary/Interactions/COB_ReleaseInteraction.h"
#include "ContentOpsBoundary/FSMTypes.h"
#include "ContentOpsBoundary/BoundaryStateMachine.h"
#include <iostream>

struct StateWaitCloseRelease : ContentStateBase {
    using ContentStateBase::react;
    TransitionResponse response;
    
    template <typename Control>
    void entryGuard(Control& control) {
        auto releaseInteraction = dynamic_cast<COB_ReleaseInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::RELEASE_CREATED]
        );
        
        if (releaseInteraction) {
            releaseInteraction->pfStateWaitCloseRelease = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
                this->response.cmdUUID = UUID;
                
                try {
                    if (control.context()->release) {
                        control.context()->releaseRepo->Update(control.context()->release.get());
                    }
                    
                    this->response.cmdStatus = "OK";
                    this->response.cmdComment = "Release closed successfully";
                    this->response.cmdDatasXML = static_cast<std::string>(*control.context()->release);
                    
                    return this->response;
                } catch (const std::exception& e) {
                    this->response.cmdStatus = "KO";
                    this->response.cmdComment = "Failed to close release: " + std::string(e.what());
                    return this->response;
                }
            };
        }
    }



    template <typename Control>
    void react(const CloseReleaseEvent& event, Control& control) {
        if (!control.context()->release) {
            if (event.params.find("id_content") != event.params.end() &&
                event.params.find("id_type") != event.params.end() &&
                event.params.find("id_localisation") != event.params.end()) {
                
                int contentId = std::stoi(event.params.at("id_content"));
                int typeId = std::stoi(event.params.at("id_type"));
                int localisationId = std::stoi(event.params.at("id_localisation"));
                
                control.context()->release = std::make_shared<COB_Release>(contentId, typeId, localisationId);
            }
        }
        
        auto releaseInteraction = dynamic_cast<COB_ReleaseInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::RELEASE_CREATED]
        );
        if (releaseInteraction && releaseInteraction->pfStateWaitCloseRelease) {
            std::map<std::string, std::string> params = event.params;
            params["cmdUUID"] = this->response.cmdUUID;
            params["cmdDatasXML"] = this->response.cmdDatasXML;
            releaseInteraction->pfStateWaitCloseRelease(this->response.cmdUUID, params);
        }
    }
    
    
}; 