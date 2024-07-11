#pragma once

#include "Domain/Content.h"
#include "Domain/Release.h"
#include "Domain/CIS.h"
#include "Domain/CPL.h"
#include "Domain/Sync.h"
#include "Domain/SyncLoop.h"

#include "App/State.h"
#include "App/ContentInteraction.h"
#include "App/PublishingInteraction.h"
#include "App/ReleaseInteraction.h"
#include "App/CISInteraction.h"
#include "App/IdleSyncInteraction.h"
#include "App/CPLInteraction.h"
#include "App/SyncInteraction.h"
#include "App/SyncLoopInteraction.h"
#include "App/InProdInteraction.h"

#include "Infra/MySQLDBConnection.h"
struct Context
{
    bool cisFinish = false;
    bool syncFinish = false;

    Content* content;
    Releases* release;
    CIS* cis;
    CPL* cpl;
    Sync* sync;
    SyncLoop* syncLoop;

    ContentInteraction* contentInteraction;
    PublishingInteraction* publishingInteraction;
    ReleaseInteraction* releaseInteraction;
    CISInteraction* cisInteraction;
    IdleSyncInteraction* idleSyncInteraction;
    CPLInteraction* cplInteraction;
    SyncInteraction* syncInteraction;
    SyncLoopInteraction* syncLoopInteraction;
    InProdInteraction* inProdInteraction;
    MySQLDBConnection* dbConnection;

    Context(ContentInteraction* pfcontentInteraction, PublishingInteraction* pfPublishingInteraction, 
            ReleaseInteraction* pfReleaseInteraction, CISInteraction* pfCISInteraction,
            IdleSyncInteraction* pfIdleSyncInteraction, CPLInteraction* pfCPLInteraction,
            SyncInteraction* pfSyncInteraction, SyncLoopInteraction* pfSyncLoopInteraction,
            InProdInteraction* pfInProdInteraction, MySQLDBConnection* dbConnection) {
        this->content = nullptr;
        this->release = nullptr;
        this->cis = nullptr;
        this->cpl = nullptr;
        this->sync = nullptr;
        this->syncLoop = nullptr;
        this->contentInteraction = pfcontentInteraction;
        this->publishingInteraction = pfPublishingInteraction;
        this->releaseInteraction = pfReleaseInteraction;
        this->cisInteraction = pfCISInteraction;
        this->idleSyncInteraction = pfIdleSyncInteraction;
        this->cplInteraction = pfCPLInteraction;
        this->syncInteraction = pfSyncInteraction;
        this->syncLoopInteraction = pfSyncLoopInteraction;
        this->inProdInteraction = pfInProdInteraction;
        this->dbConnection = dbConnection;
    }

    ~Context() {
        if (this->content) { delete this->content; }
        this->content = nullptr;
        if (this->release) { delete this->release; }
        this->release = nullptr;
        if (this->cis) { delete this->cis; }
        this->cis = nullptr;
        if (this->cpl) { delete this->cpl; }
        this->cpl = nullptr;
        if (this->sync) { delete this->sync; }
        this->sync = nullptr;
        if (this->syncLoop) { delete this->syncLoop; }
        this->syncLoop = nullptr;

        if (this->contentInteraction) { delete this->contentInteraction; }
        this->contentInteraction = nullptr;
        if (this->publishingInteraction) { delete this->publishingInteraction; }
        this->publishingInteraction = nullptr;
        if (this->releaseInteraction) { delete this->releaseInteraction; }
        this->releaseInteraction = nullptr;
        if (this->cisInteraction) { delete this->cisInteraction; }
        this->cisInteraction = nullptr;
        if (this->idleSyncInteraction) { delete this->idleSyncInteraction; }
        this->idleSyncInteraction = nullptr;
        if (this->cplInteraction) { delete this->cplInteraction; }
        this->cplInteraction = nullptr;
        if (this->syncInteraction) { delete this->syncInteraction; }
        this->syncInteraction = nullptr;
        if (this->syncLoopInteraction) { delete this->syncLoopInteraction; }
        this->syncLoopInteraction = nullptr;
        if (this->inProdInteraction) { delete this->inProdInteraction; }
        this->inProdInteraction = nullptr;
        this->dbConnection = nullptr;
    }
};