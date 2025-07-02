#include "ContentOpsBoundary/BoundaryStateMachine.h"
#include "ContentOpsDomain/COD_Content.h"

BoundaryStateMachine::BoundaryStateMachine(std::shared_ptr<COB_ContentRepo> repo, std::shared_ptr<COB_ReleaseRepo> releaseRepo, std::shared_ptr<bool> isNewContent)
    : _context{}, _fsm(nullptr)
{
    _context.contentRepo = repo;
    _context.releaseRepo = releaseRepo;
    _context.isNewContent = isNewContent;
    _fsm = new BoundaryFSM::Instance(&_context);
}

BoundaryStateMachine::BoundaryStateMachine(std::shared_ptr<COB_ContentRepo> repo, std::shared_ptr<COB_ReleaseRepo> releaseRepo, std::shared_ptr<bool> isNewContent, const std::string& title)
    : _context{}, _fsm(nullptr)
{
    _context.contentRepo = repo;
    _context.releaseRepo = releaseRepo;
    _context.isNewContent = isNewContent;
    _context.pendingTitle = std::make_shared<std::string>(title);
    _fsm = new BoundaryFSM::Instance(&_context);
}

BoundaryStateMachine::~BoundaryStateMachine() {
    delete _fsm;
}

void BoundaryStateMachine::start() {
}

void BoundaryStateMachine::update() {
    _fsm->update();
}

std::string BoundaryStateMachine::getCurrentStateName() const {
    if (_fsm->isActive<ContentInitStateFsm>()) return "ContentInitState";
    return "Unknown";
}

ContentContext& BoundaryStateMachine::getContext() {
    return *(_fsm->context());
} 