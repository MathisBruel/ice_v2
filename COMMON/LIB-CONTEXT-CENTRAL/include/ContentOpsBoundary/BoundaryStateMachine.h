#pragma once
#include <machine.hpp>
#include "ContentOpsBoundary/ContentContext.h"
#include "ContentOpsBoundary/states/ContentInitState.h"
#include "ContentOpsBoundary/states/FinishState.h"
#include "ContentOpsBoundary/states/CreateCISPathState.h"
#include <memory>

#define S(s) struct s

using Config = hfsm2::Config::ContextT<ContentContext*>;
using Machine = hfsm2::MachineT<Config>;

using PeerRootType = Machine::PeerRoot<>;

struct ContentInitStateFsm;
struct CreateCISPathStateFsm;
struct FinishStateFsm;
using BoundaryFSM = Machine::PeerRoot<
    ContentInitStateFsm,
    CreateCISPathStateFsm,
    FinishStateFsm
>;

struct ContentInitStateFsm : ContentInitState<BoundaryFSM> {};
struct CreateCISPathStateFsm : CreateCISPathState<BoundaryFSM> {};
struct FinishStateFsm : FinishState<BoundaryFSM> {};

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
    BoundaryFSM::Instance* _fsm;
};
