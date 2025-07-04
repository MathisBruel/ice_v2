#include "ContentOpsBoundary/BoundaryStateMachine.h"
#include "ContentOpsDomain/COD_Content.h"

BoundaryStateMachine::BoundaryStateMachine(std::shared_ptr<COB_ContentRepo> repo, std::shared_ptr<COB_ReleaseRepo> releaseRepo, std::shared_ptr<bool> isNewContent)
    : _context{}, _fsm(nullptr)
{
    _context.contentRepo = repo;
    _context.releaseRepo = releaseRepo;
    _context.isNewContent = isNewContent;
    _context.interactionConfigurator = std::make_shared<COB_InteractionConfigurator>();
    _fsm = new BoundaryStateMachineFSM::Instance(&_context);
}

BoundaryStateMachine::BoundaryStateMachine(std::shared_ptr<COB_ContentRepo> repo, std::shared_ptr<COB_ReleaseRepo> releaseRepo, std::shared_ptr<bool> isNewContent, const std::string& title)
    : _context{}, _fsm(nullptr)
{
    _context.contentRepo = repo;
    _context.releaseRepo = releaseRepo;
    _context.isNewContent = isNewContent;
    _context.pendingTitle = std::make_shared<std::string>(title);
    _context.interactionConfigurator = std::make_shared<COB_InteractionConfigurator>();
    _fsm = new BoundaryStateMachineFSM::Instance(&_context);
}

BoundaryStateMachine::BoundaryStateMachine(std::shared_ptr<COB_Context> cobContext)
    : _cobContext(cobContext), _fsm(nullptr)
{
    _context.interactionConfigurator = std::make_shared<COB_InteractionConfigurator>();
    _fsm = new BoundaryStateMachineFSM::Instance(_cobContext.get());
}

BoundaryStateMachine::~BoundaryStateMachine() {
    delete _fsm;
}

void BoundaryStateMachine::start() {
}

void BoundaryStateMachine::update() {
}

std::string BoundaryStateMachine::getCurrentStateName() const {
    if (_fsm->isActive<ContentInitState>()) return "ContentInitState";
    if (_fsm->isActive<StatePublishing>()) return "StatePublishing";
    if (_fsm->isActive<StateUploadCIS>()) return "StateUploadCIS";
    if (_fsm->isActive<StateSyncCreate>()) return "StateSyncCreate";
    if (_fsm->isActive<StateIdleSync>()) return "StateIdleSync";
    if (_fsm->isActive<StateCPL>()) return "StateCPL";
    if (_fsm->isActive<StateSync>()) return "StateSync";
    if (_fsm->isActive<StateSyncLoop>()) return "StateSyncLoop";
    if (_fsm->isActive<StateReleaseCreation>()) return "StateReleaseCreation";
    if (_fsm->isActive<StateCancel>()) return "StateCancel";
    if (_fsm->isActive<StateInProd>()) return "StateInProd";
    return "Unknown";
}

COB_Context& BoundaryStateMachine::getContext() {
    return *(_fsm->context());
} 