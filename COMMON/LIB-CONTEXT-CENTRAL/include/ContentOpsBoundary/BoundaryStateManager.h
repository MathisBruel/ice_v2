#pragma once
#include "BoundaryEnum.h"
// #include "BoundaryStateMachine.h"
#include "ContentOpsBoundary/COB_Content.h"
#include "ContentOpsBoundary/COB_ContentRepo.h"
#include "ContentOpsBoundary/COB_ReleaseRepo.h"
#include "ContentOpsInfra/MySQLReleaseRepo.h"
#include "ContentOpsInfra/MySQLContentRepo.h"

class BoundaryStateMachine;

class BoundaryStateManager {
public:
    BoundaryStateManager();
    ~BoundaryStateManager();
    void AddStateMachine(int id, BoundaryStateMachine* stateMachine);
    BoundaryStateMachine* GetStateMachine(int id);
    COB_Content* CreateContent(std::string title);

    void InitStateMachines();
private:
    std::map<int, BoundaryStateMachine*> _stateMachineMap;
    std::shared_ptr<COB_ContentRepo> _contentRepo;
    std::shared_ptr<COB_ReleaseRepo> _releaseRepo;
};