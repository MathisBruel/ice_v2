#pragma once
#include <functional>
#include <map>
#include <string>
struct StateIdle;
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
struct TransitionResponse {
    std::string cmdUUID;
    std::string cmdComment;
    std::string cmdStatus;
    std::string cmdDatasXML;
};