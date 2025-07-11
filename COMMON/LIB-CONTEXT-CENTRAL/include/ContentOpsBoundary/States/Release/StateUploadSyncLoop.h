#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"
#include "ContentOpsApp/TransitionResponse.h"
#include "commandCentral.h"
#include "ContentOpsBoundary/Interactions/COB_UploadInteraction.h"
#include "ContentOpsBoundary/COB_SyncLoop.h"
#include "ContentOpsBoundary/FSMTypes.h"
#include "ContentOpsBoundary/States/Release/StateCreateRelease.h"
#include <iostream>

struct StateUploadSyncLoop : ContentStateBase {
    using ContentStateBase::react;
    TransitionResponse response;
    
    
    template <typename Control>
    void entryGuard(Control& control) {
        auto uploadInteraction = dynamic_cast<COB_UploadInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::SYNC_CREATED]
        );
        
        if (uploadInteraction) {
            uploadInteraction->pfStateUploadSyncLoop = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
                this->response.cmdUUID = UUID;
                
                try {
                    std::string syncPath = Params.find("syncPath") != Params.end() ? 
                                           Params.at("syncPath") : "";
                    int servPairConfigId = Params.find("id_serv_pair_config") != Params.end() ? 
                                           std::stoi(Params.at("id_serv_pair_config")) : 1;
                    
                    if (control.context()->syncLoopRepo && !syncPath.empty()) {
                        COB_SyncLoop syncLoop(servPairConfigId, 
                                            control.context()->contentId,
                                            control.context()->typeId, 
                                            control.context()->localisationId, 
                                            syncPath);
                        control.context()->syncLoopRepo->Create(&syncLoop);
                    }
                    
                    if (control.context()->syncCount) {
                        (*control.context()->syncCount)++;
                    }
                    
                    if (control.context()->syncFinish) {
                        *control.context()->syncFinish = true;
                    }
                    
                    this->response.cmdStatus = "OK";
                    this->response.cmdComment = "SyncLoop uploaded successfully";
                    this->response.cmdDatasXML = "<syncLoop path=\"" + syncPath + "\">Upload processed</syncLoop>";
                    
                } catch (const std::exception& e) {
                    this->response.cmdStatus = "KO";
                    this->response.cmdComment = "Failed to process syncloop: " + std::string(e.what());
                }
                return this->response;
            };
        }
    }

    template <typename Control>
    void react(const UploadSyncEvent& event, Control& control) {
        
        auto uploadInteraction = dynamic_cast<COB_UploadInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::SYNC_CREATED]
        );
        if (uploadInteraction && uploadInteraction->pfStateUploadSyncLoop) {
            std::map<std::string, std::string> params = event.params;
            params["syncPath"] = params.find("syncPath") != params.end() ? params.at("syncPath") : "sync_processed";
            uploadInteraction->pfStateUploadSyncLoop(this->response.cmdUUID, params);
            
            control.changeTo(BoundaryStateMachineFSM::stateId<StateCreateRelease>());
            
            StateCreateRelease tempState;
            tempState.checkAndTransitionToWaitClose(control);
        }
    }
}; 