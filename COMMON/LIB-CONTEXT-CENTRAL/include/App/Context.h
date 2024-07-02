#pragma once

#include "Domain/Content.h"
#include "Domain/Release.h"
#include "Domain/CIS.h"
#include "Domain/CPL.h"
#include "Domain/Sync.h"
#include "Domain/SyncLoop.h"

#include "App/State.h"
#include "App/ContentInteraction.h"
#include "App/ReleaseInteraction.h"
#include "App/PublishingInteraction.h"
#include "App/CISInteraction.h"
#include "App/IdleSyncInteraction.h"
#include "App/CPLInteraction.h"
#include "App/SyncInteraction.h"
#include "App/SyncLoopInteraction.h"
#include "App/InProdInteraction.h"
#include "App/IdleInteraction.h"

struct Context
{
    bool cisFinish = false;
    bool syncFinish = false;

    Content* content = nullptr;
    Releases* release = nullptr;
    CIS* cis = nullptr;
    CPL* cpl = nullptr;
    Sync* sync = nullptr;
    SyncLoop* syncLoop = nullptr;

    IdleInteraction* idleInteraction;
    ContentInteraction* contentInteraction;
    ReleaseInteraction* releaseInteraction;
    PublishingInteraction* publishingInteraction;
    CISInteraction* cisInteraction;
    IdleSyncInteraction* idleSyncInteraction;
    CPLInteraction* cplInteraction;
    SyncInteraction* syncInteraction;
    SyncLoopInteraction* syncloopInteraction;
    InProdInteraction* inProdInteraction;
    
    Context(IdleInteraction* idleInteraction,
            ContentInteraction* contentInteraction,
            ReleaseInteraction* releaseInteraction,
            PublishingInteraction* publishingInteraction,
            CISInteraction* cisInteraction,
            IdleSyncInteraction* idleSyncInteraction,
            CPLInteraction* cplInteraction,
            SyncInteraction* syncInteraction,
            SyncLoopInteraction* syncloopInteraction,
            InProdInteraction* inProdInteraction) 
    {
        this->idleInteraction = idleInteraction;
        this->contentInteraction = contentInteraction;
        this->releaseInteraction = releaseInteraction;
        this->publishingInteraction = publishingInteraction;
        this->cisInteraction = cisInteraction;
        this->idleSyncInteraction = idleSyncInteraction;
        this->cplInteraction = cplInteraction;
        this->syncInteraction = syncInteraction;
        this->syncloopInteraction = syncloopInteraction;
        this->inProdInteraction = inProdInteraction;
    }
    ~Context() {
        this->idleInteraction = nullptr;
        this->contentInteraction = nullptr;
        this->releaseInteraction = nullptr;
        this->publishingInteraction = nullptr;
        this->cisInteraction = nullptr;
        this->idleSyncInteraction = nullptr;
        this->cplInteraction = nullptr;
        this->syncInteraction = nullptr;
        this->syncloopInteraction = nullptr;
        this->inProdInteraction = nullptr;
    }
};