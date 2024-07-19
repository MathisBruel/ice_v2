#pragma once
#include <map>
#include <string>
#include <functional>
#include "App/TransitionResponse.h"

struct StateContentInit;
struct StateReleaseCreation;
struct StatePublishing;
struct StateUploadCIS;
struct StateSyncCreate;
struct StateIdleSync;
struct StateCPL;
struct StateSync;
struct StateSyncLoop;
struct StateCancel;
struct StateInProd;

enum StateEvent {
    CONTENT_INIT,
    CREATE_CPL,
    CREATE_SYNC,
    SYNC_CREATED,
    PUSH_CIS,
    PUBLISH,
    CREATE_RELEASE,
    RELEASE_CREATED,
    CANCEL
};
// Event
struct ContentInitEvent {};
struct CreateCPLEvent {};
struct CreateSyncEvent {};
struct SyncCreatedEvent {};
struct PushCISEvent {};
struct PublishEvent {};
struct CreateReleaseEvent {};
struct ReleaseCreatedEvent {};
struct CancelEvent {};