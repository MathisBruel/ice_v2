#pragma once
#include "ContentOpsBoundary/FSMTypes.h"
#include "ContentOpsBoundary/States/ContentStateBase.h"

#include "ContentOpsBoundary/States/Release/StateCreateRelease.h"
#include "ContentOpsBoundary/States/Release/StateArchiveRelease.h"
#include "ContentOpsBoundary/States/Release/StateUploadCIS.h"
#include "ContentOpsBoundary/States/Release/StateUploadSyncLoop.h"
#include "ContentOpsBoundary/States/Release/StateWaitCloseRelease.h"
#include "ContentOpsBoundary/States/Release/StateGotNewCPL.h"

#include "ContentOpsBoundary/COB_Context.h"
#include <memory>

enum class ReleaseStateEvent {
    CREATE_RELEASE,
    UPLOAD_CIS,
    UPLOAD_SYNC,
    ARCHIVE_COMPLETE,
    CLOSE_RELEASE,
    NEW_CPL
};

class BoundaryStateMachine {
public:
    BoundaryStateMachine(int contentId, int typeId, int localisationId, std::shared_ptr<COB_ReleaseRepo> releaseRepo);
    BoundaryStateMachine(std::shared_ptr<COB_Release> release, std::shared_ptr<COB_ReleaseRepo> releaseRepo);
    BoundaryStateMachine(std::shared_ptr<COB_Context> releaseContext);
    
    ~BoundaryStateMachine();
    
    void start();
    void update();
    void Transition(ReleaseStateEvent eventTrigger, const std::map<std::string, std::string>& params = {});
    
    std::string getCurrentStateName() const;
    COB_Context& getContext();
    
    int getContentId() const {
        if (_releaseContext) {
            const int* releaseId = _releaseContext->release ? _releaseContext->release->GetReleaseId() : nullptr;
            return releaseId ? releaseId[0] : -1;
        }
        const int* releaseId = _context.release ? _context.release->GetReleaseId() : nullptr;
        return releaseId ? releaseId[0] : -1;
    }
    int getTypeId() const { 
        if (_releaseContext) {
            const int* releaseId = _releaseContext->release ? _releaseContext->release->GetReleaseId() : nullptr;
            return releaseId ? releaseId[1] : -1;
        }
        const int* releaseId = _context.release ? _context.release->GetReleaseId() : nullptr;
        return releaseId ? releaseId[1] : -1;
    }
    int getLocalisationId() const { 
        if (_releaseContext) {
            const int* releaseId = _releaseContext->release ? _releaseContext->release->GetReleaseId() : nullptr;
            return releaseId ? releaseId[2] : -1;
        }
        const int* releaseId = _context.release ? _context.release->GetReleaseId() : nullptr;
        return releaseId ? releaseId[2] : -1;
    }
    std::string getReleaseKey() const { 
        return std::to_string(getContentId()) + "_" + 
               std::to_string(getTypeId()) + "_" + 
               std::to_string(getLocalisationId());
    }
    
private:
    std::shared_ptr<COB_Context> _releaseContext;
    COB_Context _context;
    BoundaryStateMachineFSM::Instance* _fsm;
};
