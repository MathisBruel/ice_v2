#pragma once
#include "ContentOpsBoundary/FSMTypes.h"
#include "ContentOpsBoundary/States/ContentInitState.h"
#include "ContentOpsBoundary/States/StateInProd.h"
#include "ContentOpsBoundary/States/StateReleaseCreation.h"
#include "ContentOpsBoundary/States/StateCancel.h"
#include "ContentOpsBoundary/States/StatePublishing/StatePublishing.h"
#include "ContentOpsBoundary/States/StatePublishing/StateUploadCIS.h"
#include "ContentOpsBoundary/States/StatePublishing/StateSyncCreate/StateSyncCreate.h"
#include "ContentOpsBoundary/States/StatePublishing/StateSyncCreate/StateIdleSync.h"
#include "ContentOpsBoundary/States/StatePublishing/StateSyncCreate/StateCPL.h"
#include "ContentOpsBoundary/States/StatePublishing/StateSyncCreate/StateSync.h"
#include "ContentOpsBoundary/States/StatePublishing/StateSyncCreate/StateSyncLoop.h"
#include <memory>

#define S(s) struct s

using PeerRootType = BoundaryFSM::PeerRoot<>;

using BoundaryStateMachineFSM = BoundaryFSM::PeerRoot<
    ContentInitState,
    BoundaryFSM::Orthogonal<StatePublishing,
        StateUploadCIS,
        BoundaryFSM::Composite<StateSyncCreate,
        StateIdleSync,
        StateCPL,
        StateSync,
        StateSyncLoop
        >
    >,
    StateReleaseCreation,
    StateCancel,
    StateInProd
>;

#undef S

class BoundaryStateMachine {
public:
    BoundaryStateMachine(std::shared_ptr<COB_ContentRepo> repo, std::shared_ptr<COB_ReleaseRepo> releaseRepo, std::shared_ptr<bool> isNewContent, const std::string& title);
    BoundaryStateMachine(std::shared_ptr<COB_ContentRepo> repo, std::shared_ptr<COB_ReleaseRepo> releaseRepo, std::shared_ptr<bool> isNewContent);
    ~BoundaryStateMachine();
    void start();
    void update();
    std::string getCurrentStateName() const;
    ContentContext& getContext();
private:
    ContentContext _context;
    BoundaryStateMachineFSM::Instance* _fsm;
};
