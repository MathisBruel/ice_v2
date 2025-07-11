#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"
#include "ContentOpsApp/TransitionResponse.h"
#include "commandCentral.h"
#include "ContentOpsBoundary/Interactions/COB_ReleaseInteraction.h"
#include "ContentOpsBoundary/States/Release/StateWaitCloseRelease.h"
#include <iostream>

struct StateCreateRelease : ContentStateBase {
    using ContentStateBase::react;
    TransitionResponse response;
    
    
    template <typename Control>
    void update(Control& control) {
        if (control.context()->cisFinish && control.context()->syncFinish) {
            if (*control.context()->cisFinish && *control.context()->syncFinish) {
                control.changeTo(BoundaryStateMachineFSM::stateId<StateWaitCloseRelease>());
                return;
            }
        }
    }
    
    template <typename Control>
    void entryGuard(Control& control) {
        
        auto releaseInteraction = dynamic_cast<COB_ReleaseInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::CREATE_RELEASE]
        );
        
        if (releaseInteraction) {
            releaseInteraction->pfStateCreateRelease = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
                this->response.cmdUUID = UUID;
                
                try {
                    int contentId = std::stoi(Params["contentId"]);
                    int typeId = std::stoi(Params["typeId"]);
                    int localisationId = std::stoi(Params["localisationId"]);
                    
                    COB_Release newRelease(contentId, typeId, localisationId);
                    control.context()->releaseRepo->Create(&newRelease);
                    
                    if (!control.context()->release) {
                        control.context()->release = std::make_shared<COB_Release>(newRelease);
                    }
                    
                    this->response.cmdStatus = "OK";
                    this->response.cmdComment = "Release created successfully";
                    this->response.cmdDatasXML = static_cast<std::string>(newRelease);
                    
                    return this->response;
                } catch (const std::exception& e) {
                    this->response.cmdStatus = "KO";
                    this->response.cmdComment = "Failed to create release: " + std::string(e.what());
                    return this->response;
                }
            };
        }
    }

    template <typename Control>
    void react(const CreateReleaseEvent& event, Control& control) {
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
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::CREATE_RELEASE]
        );
        if (releaseInteraction && releaseInteraction->pfStateCreateRelease) {
            std::map<std::string, std::string> params = event.params;
            params["cmdUUID"] = this->response.cmdUUID;
            params["cmdDatasXML"] = this->response.cmdDatasXML;
            releaseInteraction->pfStateCreateRelease(this->response.cmdUUID, params);
        }
        
        checkAndTransitionToWaitClose(control);
    }

    template <typename Control>
    void react(const UploadCISEvent& event, Control& control) {
        control.changeTo(BoundaryStateMachineFSM::stateId<StateUploadCIS>());
    }
    
    template <typename Control>
    void react(const UploadSyncEvent& event, Control& control) {
        control.changeTo(BoundaryStateMachineFSM::stateId<StateUploadSyncLoop>());
    }
    
    template <typename Control>
    void checkAndTransitionToWaitClose(Control& control) {
        if (control.context()->cisFinish && control.context()->syncFinish) {
            if (*control.context()->cisFinish && *control.context()->syncFinish) {
                control.changeTo(BoundaryStateMachineFSM::stateId<StateWaitCloseRelease>());
                return;
            }
        }
    }
    
    template <typename Control>
    static constexpr uint8_t select(const Control& control) {
        return 0;
    }
    
}; 