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
    ContentInit,
    CreateCPL,
    CreateSync,
    SyncCreated,
    PushCIS,
    Publish,
    CreateRelease,
    ReleaseCreated,
    Cancel
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