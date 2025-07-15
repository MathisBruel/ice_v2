#pragma once
#include "BoundaryEnum.h"
#include "ContentOpsBoundary/COB_Release.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsBoundary/COB_Configurator.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"
#include "ContentOpsApp/TransitionResponse.h"
#include <map>

class BoundaryStateMachine;

class BoundaryStateManager {
public:
    BoundaryStateManager();
    ~BoundaryStateManager();
    
    void AddStateMachine(const std::string& releaseKey, BoundaryStateMachine* stateMachine);
    BoundaryStateMachine* GetStateMachine(const std::string& releaseKey);
    
    std::string MakeReleaseKey(int contentId, int typeId, int localisationId) const;
    
    TransitionResponse CreateRelease(int contentId, int typeId, int localisationId, std::string releaseCplRefPath);
    TransitionResponse StartReleaseStateMachine(int contentId, int typeId, int localisationId);
    
    std::string GetReleaseState(int contentId, int typeId, int localisationId);
    std::string GetReleaseState(const std::string& releaseKey);
    
    std::string GetState(int contentId);
    
    void InitReleaseStateMachines();
    
    void ProcessUploadCIS(int contentId, int typeId, int localisationId, std::string releaseCisPath);
    void ProcessUploadSync(int contentId, int typeId, int localisationId, int servPairConfigId, std::string syncPath);
    void ProcessNewCPL(int contentId, int typeId, int localisationId);
    void ProcessCloseRelease(int contentId, int typeId, int localisationId);

private:
    std::map<std::string, BoundaryStateMachine*> _releaseStateMachineMap;
    
    std::shared_ptr<COB_ReleaseRepo> _releaseRepo;
    std::shared_ptr<COB_Configurator> _configurator;
    
    void CheckAndTransitionToWaitClose(int contentId, int typeId, int localisationId);
    void loadRelease(int contentId, int typeId, int localisationId);
};