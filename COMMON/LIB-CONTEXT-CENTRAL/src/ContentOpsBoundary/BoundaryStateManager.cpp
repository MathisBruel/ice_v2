#include "ContentOpsBoundary/BoundaryStateManager.h"
#include "ContentOpsBoundary/BoundaryStateMachine.h"
#include "ContentOpsInfra/MySQLContentRepo.h"
#include "commandCentral.h"
#include <stdexcept>

BoundaryStateManager::BoundaryStateManager() {
    InitStateMachines();
}
BoundaryStateManager::~BoundaryStateManager() {}

void BoundaryStateManager::AddStateMachine(int id, BoundaryStateMachine* stateMachine) {
    _stateMachineMap[id] = stateMachine;
}

BoundaryStateMachine* BoundaryStateManager::GetStateMachine(int id) {
    auto it = _stateMachineMap.find(id);
    if (it != _stateMachineMap.end())
        return it->second;
    return nullptr;
}

TransitionResponse BoundaryStateManager::CreateContent(std::string title) {
    if (!_contentRepo) _contentRepo = std::make_shared<COB_ContentRepo>(std::make_shared<MySQLContentRepo>());
    if (!_releaseRepo) _releaseRepo = std::make_shared<COB_ReleaseRepo>(std::make_shared<MySQLReleaseRepo>());
    
    auto* sm = new BoundaryStateMachine(_contentRepo, _releaseRepo, std::make_shared<bool>(true), title);
    sm->start();
    
    // Vérifier que le configurateur d'interaction existe
    if (!sm->getContext().interactionConfigurator) {
        delete sm;
        throw std::runtime_error("Interaction configurator not initialized");
    }
    
    // Vérifier que l'interaction CREATE_CONTENT existe
    auto interactions = sm->getContext().interactionConfigurator->GetInteractions();
    if (interactions.find(CommandCentral::CREATE_CONTENT) == interactions.end()) {
        delete sm;
        throw std::runtime_error("CREATE_CONTENT interaction not found");
    }
    
    // Exécuter l'interaction pour créer le contenu
    TransitionResponse transitionResponse = interactions[CommandCentral::CREATE_CONTENT]->Run("temp_uuid", std::map<std::string, std::string>{{"contentTitle", title}});
    
    // Vérifier que le contenu a été créé
    if (sm->getContext().content) {
        int contentId = *sm->getContext().content->GetContentId();
        _stateMachineMap[contentId] = sm;
        sm->update();
        return transitionResponse;
    } else {
        delete sm;
        throw std::runtime_error("Content non créé");
    }
}

void BoundaryStateManager::InitStateMachines() {
    if (!_contentRepo) _contentRepo = std::make_shared<COB_ContentRepo>(std::make_shared<MySQLContentRepo>());
    if (!_releaseRepo) _releaseRepo = std::make_shared<COB_ReleaseRepo>(std::make_shared<MySQLReleaseRepo>());
    COB_Contents contents = _contentRepo->GetContents();
    std::for_each(contents.begin(), contents.end(), [this](const COB_Content& content) {
        int id = content.GetContentId();
        auto isNewContent = std::make_shared<bool>(false);
        auto* sm = new BoundaryStateMachine(_contentRepo, _releaseRepo, isNewContent);
        sm->getContext().content = std::make_shared<COB_Content>(content);
        sm->start();
        _stateMachineMap[id] = sm;
    });
} 

std::string BoundaryStateManager::GetState(int contentId) {
    auto it = _stateMachineMap.find(contentId);
    if (it != _stateMachineMap.end())
        return it->second->getCurrentStateName();
    return "Unknown";
}