#include "ContentOpsBoundary/BoundaryStateManager.h"
#include "ContentOpsBoundary/BoundaryStateMachine.h"
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"
#include "commandCentral.h"
#include <stdexcept>
#include <vector>

BoundaryStateManager::BoundaryStateManager() {
    InitReleaseStateMachines();
}

BoundaryStateManager::~BoundaryStateManager() {
    for (auto& pair : _releaseStateMachineMap) {
        delete pair.second;
    }
    _releaseStateMachineMap.clear();
}


void BoundaryStateManager::AddStateMachine(const std::string& releaseKey, BoundaryStateMachine* stateMachine) {
    _releaseStateMachineMap[releaseKey] = stateMachine;
}

BoundaryStateMachine* BoundaryStateManager::GetStateMachine(const std::string& releaseKey) {
    auto it = _releaseStateMachineMap.find(releaseKey);
    if (it != _releaseStateMachineMap.end())
        return it->second;
    return nullptr;
}

std::string BoundaryStateManager::MakeReleaseKey(int contentId, int typeId, int localisationId) const {
    return std::to_string(contentId) + "_" + std::to_string(typeId) + "_" + std::to_string(localisationId);
}


TransitionResponse BoundaryStateManager::CreateRelease(int contentId, int typeId, int localisationId, std::string releaseCplRefPath) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    BoundaryStateMachine* sm = nullptr;
    
    try {
        COB_Release newRelease(contentId, typeId, localisationId);
        newRelease.SetReleaseInfos(releaseCplRefPath);
        _releaseRepo->Create(&newRelease);
        
        auto context = std::make_shared<COB_Context>(contentId, typeId, localisationId);
        context->releaseRepo = _releaseRepo;
        context->interactionConfigurator = std::make_shared<COB_InteractionConfigurator>();
        context->release = std::make_shared<COB_Release>(newRelease);
        context->isNewContent = std::make_shared<bool>(true);
        context->syncFinish = std::make_shared<bool>(context->releaseRepo->IsSyncLoopUploaded(contentId, typeId, localisationId));
        
        sm = new BoundaryStateMachine(context);
        sm->start();
        
        _releaseStateMachineMap[releaseKey] = sm;
        
        std::map<std::string, std::string> createParams;
        createParams["id_content"] = std::to_string(contentId);
        createParams["id_type"] = std::to_string(typeId);
        createParams["id_localisation"] = std::to_string(localisationId);
        sm->Transition(ReleaseStateEvent::CREATE_RELEASE, createParams);
        sm->update();
        
        TransitionResponse response;
        response.cmdUUID = "release_" + releaseKey;
        response.cmdStatus = "OK";
        response.cmdComment = "Release created successfully";
        response.cmdDatasXML = "<release id=\"" + releaseKey + "\" contentId=\"" + std::to_string(contentId) + 
                              "\" typeId=\"" + std::to_string(typeId) + "\" localisationId=\"" + std::to_string(localisationId) + 
                              "\" state=\"" + sm->getCurrentStateName() + "\" />";
        
        return response;
        
    } catch (const std::exception& e) {
        if (sm && _releaseStateMachineMap.find(releaseKey) != _releaseStateMachineMap.end()) {
            _releaseStateMachineMap.erase(releaseKey);
            delete sm;
        } else if (sm) {
            delete sm;
        }
        throw std::runtime_error("Failed to create release: " + std::string(e.what()));
    }
}

// Fonction utilitaire pour charger une release existante et initialiser la state machine
void BoundaryStateManager::loadRelease(int contentId, int typeId, int localisationId) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    COB_Release release = _releaseRepo->GetRelease(contentId, typeId, localisationId);
    auto context = std::make_shared<COB_Context>(contentId, typeId, localisationId);
    context->releaseRepo = _releaseRepo;
    context->interactionConfigurator = std::make_shared<COB_InteractionConfigurator>();
    context->release = std::make_shared<COB_Release>(release);
    context->isNewContent = std::make_shared<bool>(false);
    context->cisFinish = std::make_shared<bool>(context->releaseRepo->IsCISUploaded(contentId, typeId, localisationId));
    context->syncFinish = std::make_shared<bool>(context->releaseRepo->IsSyncLoopUploaded(contentId, typeId, localisationId));
    BoundaryStateMachine* sm = new BoundaryStateMachine(context);
    sm->start();
    _releaseStateMachineMap[releaseKey] = sm;
}

TransitionResponse BoundaryStateManager::StartReleaseStateMachine(int contentId, int typeId, int localisationId) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    
    BoundaryStateMachine* sm = GetStateMachine(releaseKey);
    if (!sm) {
        if (_releaseRepo->IsReleaseCreated(contentId, typeId, localisationId)) {
            loadRelease(contentId, typeId, localisationId);
            sm = GetStateMachine(releaseKey);
        } else {
            // Si la release n'existe pas, la créer en base
            return CreateRelease(contentId, typeId, localisationId, "");
        }
    } else {
        sm->start();
    }
    sm->update();
    return TransitionResponse{"", "StateMachine démarrée pour release " + releaseKey, "OK", ""};
}


std::string BoundaryStateManager::GetReleaseState(int contentId, int typeId, int localisationId) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    return GetReleaseState(releaseKey);
}

std::string BoundaryStateManager::GetReleaseState(const std::string& releaseKey) {
    auto it = _releaseStateMachineMap.find(releaseKey);
    if (it != _releaseStateMachineMap.end())
        return it->second->getCurrentStateName();
    return "Unknown";
}

std::string BoundaryStateManager::GetState(int contentId) {
    std::vector<std::string> states;
    bool hasActiveReleases = false;
    bool cisUploaded = false;
    bool syncLoopUploaded = false;

    for (const auto& pair : _releaseStateMachineMap) {
        BoundaryStateMachine* sm = pair.second;
        if (sm && sm->getContentId() == contentId) {
            hasActiveReleases = true;
            std::string state = sm->getCurrentStateName();
            states.push_back(state);

            COB_Context& ctx = sm->getContext();
            if (ctx.cisFinish && *ctx.cisFinish) {
                cisUploaded = true;
            }
            if (ctx.syncFinish && *ctx.syncFinish) {
                syncLoopUploaded = true;
            }
        }
    }

    if (!hasActiveReleases) {
        return "NoActiveReleases";
    }

    for (const std::string& state : states) {
        if (state == "StateInProd") {
            return "InProd";
        }
    }

    for (const std::string& state : states) {
        if (state != "StateCancel" && state != "Unknown") {
            std::string result = state;
            result += " [";
            result += "CISUpload=";
            result += (cisUploaded ? "oui" : "non");
            result += ", SyncLoopUpload=";
            result += (syncLoopUploaded ? "oui" : "non");
            result += "]";
            return result;
        }
    }

    return "Inactive";
}

void BoundaryStateManager::InitReleaseStateMachines() {
    if (!_configurator) {
        _configurator = std::make_shared<COB_Configurator>();
    }
    if (!_releaseRepo) {
        _releaseRepo = _configurator->GetReleaseRepo();
    }
}

void BoundaryStateManager::ProcessUploadCIS(int contentId, int typeId, int localisationId, std::string releaseCisPath) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    
    auto it = _releaseStateMachineMap.find(releaseKey);
    if (it != _releaseStateMachineMap.end()) {
        BoundaryStateMachine* stateMachine = it->second;
        
        std::map<std::string, std::string> params;
        params["release_cis_path"] = releaseCisPath;
        params["id_content"] = std::to_string(contentId);
        params["id_type"] = std::to_string(typeId);
        params["id_localisation"] = std::to_string(localisationId);
        
        stateMachine->Transition(ReleaseStateEvent::UPLOAD_CIS, params);
        CheckAndTransitionToWaitClose(contentId, typeId, localisationId);
    }
}

void BoundaryStateManager::ProcessUploadSync(int contentId, int typeId, int localisationId, int servPairConfigId, std::string syncPath) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    
    auto it = _releaseStateMachineMap.find(releaseKey);
    if (it != _releaseStateMachineMap.end()) {
        BoundaryStateMachine* stateMachine = it->second;
        
        std::map<std::string, std::string> params;
        params["id_content"] = std::to_string(contentId);
        params["id_type"] = std::to_string(typeId);
        params["id_localisation"] = std::to_string(localisationId);
        params["id_serv_pair_config"] = std::to_string(servPairConfigId);
        params["syncPath"] = syncPath;
        stateMachine->Transition(ReleaseStateEvent::UPLOAD_SYNC, params);
        CheckAndTransitionToWaitClose(contentId, typeId, localisationId);
    }
}

void BoundaryStateManager::ProcessNewCPL(int contentId, int typeId, int localisationId) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    BoundaryStateMachine* sm = GetStateMachine(releaseKey);
    
    if (sm) {
        std::map<std::string, std::string> params;
        params["id_content"] = std::to_string(contentId);
        params["id_type"] = std::to_string(typeId);
        params["id_localisation"] = std::to_string(localisationId);
        sm->Transition(ReleaseStateEvent::NEW_CPL, params);
    }
}

void BoundaryStateManager::ProcessCloseRelease(int contentId, int typeId, int localisationId) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    BoundaryStateMachine* sm = GetStateMachine(releaseKey);
    
    if (sm) {
        std::map<std::string, std::string> params;
        params["id_content"] = std::to_string(contentId);
        params["id_type"] = std::to_string(typeId);
        params["id_localisation"] = std::to_string(localisationId);
        sm->Transition(ReleaseStateEvent::CLOSE_RELEASE, params);
    }
}

void BoundaryStateManager::CheckAndTransitionToWaitClose(int contentId, int typeId, int localisationId) {
    std::string releaseKey = MakeReleaseKey(contentId, typeId, localisationId);
    
    auto it = _releaseStateMachineMap.find(releaseKey);
    if (it != _releaseStateMachineMap.end()) {
        BoundaryStateMachine* stateMachine = it->second;
        COB_Context& context = stateMachine->getContext();
        
        bool cisComplete = context.cisFinish && *context.cisFinish;
        bool syncComplete = context.syncFinish && *context.syncFinish;
        
        if (cisComplete && syncComplete) {
            std::map<std::string, std::string> params;
            params["id_content"] = std::to_string(contentId);
            params["id_type"] = std::to_string(typeId);
            params["id_localisation"] = std::to_string(localisationId);
            stateMachine->Transition(ReleaseStateEvent::ARCHIVE_COMPLETE, params);
        }
    }
}