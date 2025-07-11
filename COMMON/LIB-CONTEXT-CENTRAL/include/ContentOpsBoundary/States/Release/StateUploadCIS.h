#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"
#include "ContentOpsApp/TransitionResponse.h"
#include "commandCentral.h"
#include "ContentOpsBoundary/Interactions/COB_UploadInteraction.h"
#include "ContentOpsBoundary/States/Release/StateCreateRelease.h"
#include <iostream>

struct StateUploadCIS : ContentStateBase {
    using ContentStateBase::react;
    TransitionResponse response;
    
    
    template <typename Control>
    void entryGuard(Control& control) {
        auto uploadInteraction = dynamic_cast<COB_UploadInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::CIS_CREATED]
        );
        
        if (uploadInteraction) {
            uploadInteraction->pfStateUploadCIS = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
                this->response.cmdUUID = UUID;
                
                if (control.context()->release && Params.find("release_cis_path") != Params.end()) {
                    try {
                        control.context()->release->SetCISPath(Params.at("release_cis_path"));
                        
                        if (control.context()->releaseRepo) {
                            control.context()->releaseRepo->Update(control.context()->release.get());
                        }
                        
                        if (control.context()->cisFinish) {
                            *control.context()->cisFinish = true;
                        }
                        
                        std::cout << "StateUploadCIS: Successfully updated CIS path to: " << Params.at("release_cis_path") << std::endl;
                        
                        this->response.cmdStatus = "OK";
                        this->response.cmdComment = "CIS uploaded and path saved successfully";
                        this->response.cmdDatasXML = Params.at("release_cis_path");
                        
                    } catch (const std::exception& e) {
                        std::cout << "StateUploadCIS: Error updating CIS path: " << e.what() << std::endl;
                        this->response.cmdStatus = "KO";
                        this->response.cmdComment = "Failed to save CIS path: " + std::string(e.what());
                    }

                } else {
                    std::cout << "StateUploadCIS: Error - invalid release context or empty CIS path" << std::endl;
                    this->response.cmdStatus = "KO";
                    this->response.cmdComment = "Invalid release context or empty CIS path";
                }

                return this->response;
            
            };
        }
    }
    
    template <typename Control>
    void react(const UploadCISEvent& event, Control& control) {
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
        
        auto uploadInteraction = dynamic_cast<COB_UploadInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::CIS_CREATED]
        );
        if (uploadInteraction && uploadInteraction->pfStateUploadCIS) {
            std::map<std::string, std::string> params = event.params;
            params["release_cis_path"] = event.releaseCisPath;
            uploadInteraction->pfStateUploadCIS(this->response.cmdUUID, params);
            
            control.changeTo(BoundaryStateMachineFSM::stateId<StateCreateRelease>());
            
            StateCreateRelease tempState;
            tempState.checkAndTransitionToWaitClose(control);
        }
    }
}; 