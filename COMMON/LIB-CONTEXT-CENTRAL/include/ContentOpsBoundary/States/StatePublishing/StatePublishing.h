#pragma once
#include "ContentOpsBoundary/States/ContentStateBase.h"
#include "ContentOpsBoundary/States/StateReleaseCreation.h"
#include "ContentOpsBoundary/States/StateCancel.h"
#include "ContentOpsApp/TransitionResponse.h"
#include "ContentOpsApp/PublishingInteraction.h"
#include "commandCentral.h"
#include "ContentOpsBoundary/Interactions/COB_PublishingInteraction.h"
#include <iostream>

// Forward declaration pour éviter la dépendance circulaire
class BoundaryManager;

// Fonctions de gestion des suppressions (implémentées dans le .cpp)
void handleDeleteRelease(const std::map<std::string, std::string>& Params);
void handleDeleteCPL(const std::map<std::string, std::string>& Params);
void handleDeleteSync(const std::map<std::string, std::string>& Params);
void handleDeleteSyncLoop(const std::map<std::string, std::string>& Params);

struct StatePublishing : ContentStateBase {
    template <typename Control>
    void entryGuard(Control& control) {
        std::cout << "StatePublishing entryGuard" << std::endl;
    }

    template <typename Control>
    void enter(Control& control) {
        std::cout << "StatePublishing enter" << std::endl;
        
        // Configuration de l'interaction pour StatePublishing
        auto publishingInteraction = dynamic_cast<COB_PublishingInteraction*>(control.context()->interactionConfigurator->GetInteractions()[CommandCentral::CREATE_RELEASE]);
        if (publishingInteraction) {
            publishingInteraction->pfStatePublishing = [control, this](std::string UUID, std::map<std::string, std::string> Params) {
                TransitionResponse response;
                response.cmdUUID = UUID;
                
                try {
                    // Logique de suppression selon le type d'élément
                    if (Params["typeOfElement"] == "RELEASE") {
                        handleDeleteRelease(Params);
                        response.cmdComment = "Release deleted";
                    }
                    else if (Params["typeOfElement"] == "CPL") {
                        response.cmdComment = "Cpl deleted";
                        handleDeleteSync(Params);
                        handleDeleteCPL(Params);
                    } 
                    else if (Params["typeOfElement"] == "SYNC") {
                        response.cmdComment = "Sync deleted";
                        handleDeleteSync(Params);
                    } 
                    else if (Params["typeOfElement"] == "SYNCLOOP") {
                        response.cmdComment = "SyncLoop deleted";
                        handleDeleteSyncLoop(Params);
                    }
                    
                    response.cmdStatus = "OK";
                    return response;
                } catch (const std::exception& e) {
                    response.cmdStatus = "KO";
                    response.cmdComment = "Failed Publishing operation: " + std::string(e.what());
                    return response;
                }
            };
            
        }
    }

    template <typename Control>
    void update(Control& control) {
        std::cout << "StatePublishing update" << std::endl;
        std::cout << "Etats d'avancement: CIS " << control.context()->cisFinish << " Sync " << control.context()->syncFinish << "\n";
    }

    template <typename Control>
    void exit(Control& control) {
        std::cout << "StatePublishing exit" << std::endl;
    }


};
