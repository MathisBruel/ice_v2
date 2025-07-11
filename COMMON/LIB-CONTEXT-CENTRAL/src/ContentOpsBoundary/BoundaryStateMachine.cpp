#include "ContentOpsBoundary/BoundaryStateMachine.h"
#include "ContentOpsBoundary/States/ContentStateBase.h"

BoundaryStateMachine::BoundaryStateMachine(int contentId, int typeId, int localisationId, std::shared_ptr<COB_ReleaseRepo> releaseRepo)
    : _context(contentId, typeId, localisationId), _fsm(nullptr)
{
    _context.releaseRepo = releaseRepo;
    _context.interactionConfigurator = std::make_shared<COB_InteractionConfigurator>();
    
    // Initialiser les flags pour les branches parallèles
    _context.cisFinish = std::make_shared<bool>(false);
    _context.syncFinish = std::make_shared<bool>(false);
    _context.syncCount = std::make_shared<int>(0);
    
    // Charger ou créer la release
    try {
        COB_Release release = releaseRepo->GetRelease(contentId, typeId, localisationId);
        _context.release = std::make_shared<COB_Release>(release);
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors du chargement de la release: " << e.what() << std::endl;
        // Créer une nouvelle release si elle n'existe pas
        _context.release = std::make_shared<COB_Release>(contentId, typeId, localisationId);
    }
    
    _fsm = new BoundaryStateMachineFSM::Instance(&_context);
}

BoundaryStateMachine::BoundaryStateMachine(std::shared_ptr<COB_Release> release, std::shared_ptr<COB_ReleaseRepo> releaseRepo)
    : _context{}, _fsm(nullptr)
{
    _context.release = release;
    _context.releaseRepo = releaseRepo;
    _context.interactionConfigurator = std::make_shared<COB_InteractionConfigurator>();
    
    // Initialiser les flags pour les branches parallèles
    _context.cisFinish = std::make_shared<bool>(false);
    _context.syncFinish = std::make_shared<bool>(false);
    _context.syncCount = std::make_shared<int>(0);
    
    _fsm = new BoundaryStateMachineFSM::Instance(&_context);
}

BoundaryStateMachine::BoundaryStateMachine(std::shared_ptr<COB_Context> releaseContext)
    : _releaseContext(releaseContext), _fsm(nullptr)
{
    if (!_releaseContext->interactionConfigurator) {
        _releaseContext->interactionConfigurator = std::make_shared<COB_InteractionConfigurator>();
    }
    
    // Initialiser les flags pour les branches parallèles s'ils ne sont pas déjà initialisés
    if (!_releaseContext->cisFinish) {
        _releaseContext->cisFinish = std::make_shared<bool>(false);
    }
    if (!_releaseContext->syncFinish) {
        _releaseContext->syncFinish = std::make_shared<bool>(false);
    }
    if (!_releaseContext->syncCount) {
        _releaseContext->syncCount = std::make_shared<int>(0);
    }
    
    _fsm = new BoundaryStateMachineFSM::Instance(_releaseContext.get());
}

BoundaryStateMachine::~BoundaryStateMachine() {
    delete _fsm;
}

void BoundaryStateMachine::start() {
    if (!_fsm) {
        return;
    }
}

void BoundaryStateMachine::update() {
    if (!_fsm) {
        return;
    }
    _fsm->update();
}

void BoundaryStateMachine::Transition(ReleaseStateEvent eventTrigger, const std::map<std::string, std::string>& params) {
    if (!_fsm) {
        return;
    }
    
    switch (eventTrigger) {
        case ReleaseStateEvent::CREATE_RELEASE:
            _fsm->react(CreateReleaseEvent{params});
            break;
        case ReleaseStateEvent::UPLOAD_CIS: {
            std::string releaseCisPath = params.find("release_cis_path") != params.end() ? 
                                       params.at("release_cis_path") : "";
            _fsm->react(UploadCISEvent{releaseCisPath, params});
            break;
        }
        case ReleaseStateEvent::UPLOAD_SYNC:
            _fsm->react(UploadSyncEvent{params});
            break;
        case ReleaseStateEvent::ARCHIVE_COMPLETE:
            _fsm->react(ArchiveCompleteEvent{params});
            break;
        case ReleaseStateEvent::CLOSE_RELEASE:
            _fsm->react(CloseReleaseEvent{params});
            break;
        case ReleaseStateEvent::NEW_CPL:
            _fsm->react(NewCPLEvent{params});
            break;
    }
}

std::string BoundaryStateMachine::getCurrentStateName() const {
    if (!_fsm) return "StateInvalid";
    
    bool cisUploaded = false;
    bool syncLoopUploaded = false;
    
    
    
    // On regarde dans le contexte si un CIS ou un SyncLoop a été uploadé
    if (_releaseContext) {
        if (_releaseContext->cisFinish) {
            cisUploaded = *_releaseContext->cisFinish;
        }
        if (_releaseContext->syncFinish) {
            syncLoopUploaded = *_releaseContext->syncFinish;
        }
    } else if (_context.cisFinish) {
        cisUploaded = *_context.cisFinish;
    } else if (_context.syncFinish) {
        syncLoopUploaded = *_context.syncFinish;
    }
    
    std::string result = "CIS uploadée: ";
    result += cisUploaded ? "oui" : "non";
    result += ", SyncLoop uploadée: ";
    result += syncLoopUploaded ? "oui" : "non";
    return result;
}

COB_Context& BoundaryStateMachine::getContext() {
    if (_releaseContext) {
        return *_releaseContext;
    }
    return _context;
} 