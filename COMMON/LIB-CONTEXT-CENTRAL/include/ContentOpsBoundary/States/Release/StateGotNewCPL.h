#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"
#include "ContentOpsApp/TransitionResponse.h"
#include "commandCentral.h"
#include "ContentOpsBoundary/Interactions/COB_CPLInteraction.h"
#include <iostream>

struct StateGotNewCPL : ContentStateBase {
    using ContentStateBase::react;
    TransitionResponse response;
    
    template <typename Control>
    void entryGuard(Control& control) {
        auto cplInteraction = dynamic_cast<COB_CPLInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::CPL_CREATED]
        );
        
        if (cplInteraction) {
            cplInteraction->pfStateGotNewCPL = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
                this->response.cmdUUID = UUID;
                
                try {
                    std::string cplId = Params["cplId"];
                    std::string cplPath = Params["cplPath"];
                    std::string cplData = Params["cplData"];
                    
                    if (control.context()->release) {
                        std::vector<SyncInfo> generatedSyncs = GenerateSyncsFromCPL(cplPath, cplData);
                        control.context()->releaseRepo->Update(control.context()->release.get());
                    }
                    
                    this->response.cmdStatus = "OK";
                    this->response.cmdComment = "CPL processed and syncs generated successfully";
                    this->response.cmdDatasXML = cplPath;
                    
                    return this->response;
                } catch (const std::exception& e) {
                    this->response.cmdStatus = "KO";
                    this->response.cmdComment = "Failed to process CPL: " + std::string(e.what());
                    return this->response;
                }
            };
        }
    }
    
    template <typename Control>
    void react(const NewCPLEvent& event, Control& control) {
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
        
        auto cplInteraction = dynamic_cast<COB_CPLInteraction*>(
            control.context()->interactionConfigurator->GetInteractions()[CommandCentral::CPL_CREATED]
        );
        if (cplInteraction && cplInteraction->pfStateGotNewCPL) {
            std::map<std::string, std::string> params = event.params;
            params["cmdUUID"] = this->response.cmdUUID;
            params["cmdDatasXML"] = this->response.cmdDatasXML;
            cplInteraction->pfStateGotNewCPL(this->response.cmdUUID, params);
            control.changeTo(BoundaryStateMachineFSM::stateId<StateCreateRelease>());
        }
    }

private:
    struct SyncInfo {
        std::string path;
        std::string data;
        std::string type;
    };

    std::vector<SyncInfo> GenerateSyncsFromCPL(const std::string& cplPath, const std::string& cplData) {
        throw std::runtime_error("Not implemented");
    }
}; 