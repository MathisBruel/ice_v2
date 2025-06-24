#pragma once

#include "ContentOpsDomain/COD_CIS.h"
#include "ContentOpsDomain/CPL.h"
#include "ContentOpsDomain/Sync.h"
#include "ContentOpsDomain/SyncLoop.h"
#include "ContentOpsDomain/Content.h"

#include "ContentOpsApp/State.h"
#include "ContentOpsApp/ContentInteraction.h"
#include "ContentOpsApp/PublishingInteraction.h"
#include "ContentOpsApp/ReleaseInteraction.h"
#include "ContentOpsApp/CISInteraction.h"
#include "ContentOpsApp/IdleSyncInteraction.h"
#include "ContentOpsApp/CPLInteraction.h"
#include "ContentOpsApp/SyncInteraction.h"
#include "ContentOpsApp/SyncLoopInteraction.h"
#include "ContentOpsApp/InProdInteraction.h"

#include "ContentOpsInfra/MySQLDBConnection.h"

class Content;
class Releases;

struct Context
{
    bool cisFinish;
    bool syncFinish;

    Content* content;
    Releases* release;

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

    Context(ContentInteraction* contentInteraction,
            PublishingInteraction* publishingInteraction,
            ReleaseInteraction* releaseInteraction,
            CISInteraction* cisInteraction,
            IdleSyncInteraction* idleSyncInteraction,
            CPLInteraction* cplInteraction,
            SyncInteraction* syncInteraction,
            SyncLoopInteraction* syncLoopInteraction,
            InProdInteraction* inProdInteraction,
            MySQLDBConnection* dbConnection) {
        this->contentInteraction = contentInteraction;
        this->publishingInteraction = publishingInteraction;
        this->releaseInteraction = releaseInteraction;
        this->cisInteraction = cisInteraction;
        this->idleSyncInteraction = idleSyncInteraction;
        this->cplInteraction = cplInteraction;
        this->syncInteraction = syncInteraction;
        this->syncLoopInteraction = syncLoopInteraction;
        this->inProdInteraction = inProdInteraction;
        this->dbConnection = dbConnection;
        this->content = nullptr;
        this->release = nullptr;
        this->cisFinish = false;
        this->syncFinish = false;
    }

    ~Context() {
        if (this->content) { delete this->content; }
        this->content = nullptr;
        if (this->release) { delete this->release; }
        this->release = nullptr;

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