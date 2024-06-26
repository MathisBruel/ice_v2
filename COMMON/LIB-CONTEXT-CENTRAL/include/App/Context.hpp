#pragma once

#include "Domain/Content.h"
#include "Domain/Release.h"
#include "Domain/CIS.h"
#include "Domain/CPL.h"
#include "Domain/Sync.h"
#include "Domain/SyncLoop.h"

#include "App/State.h"
#include "App/Content_Interaction.h"
#include "App/Release_Interaction.h"
#include "App/Publishing_Interaction.h"
#include "App/CIS_Interaction.h"
#include "App/IdleSync_Interaction.h"
#include "App/CPL_Interaction.h"
#include "App/Sync_Interaction.h"
#include "App/SyncLoop_Interaction.h"
#include "App/InProd_Interaction.h"
#include "App/Idle_Interaction.h"

struct Context
{
    bool CISFinish = false;
    bool SyncFinish = false;

    Content content;
    Releases release;
    CIS cis;
    CPL cpl;
    Sync sync;
    SyncLoop syncLoop;

    Idle_Interaction* idleInteraction;
    Content_Interaction* contentInteraction;
    Release_Interaction* releaseInteraction;
    Publishing_Interaction* publishingInteraction;
    CIS_Interaction* cisInteraction;
    IdleSync_Interaction* idleSyncInteraction;
    CPL_Interaction* cplInteraction;
    Sync_Interaction* syncInteraction;
    SyncLoop_Interaction* syncloopInteraction;
    InProd_Interaction* inProdInteraction;
    
    Context(Idle_Interaction* idleInteraction,
            Content_Interaction* contentInteraction,
            Release_Interaction* releaseInteraction,
            Publishing_Interaction* publishingInteraction,
            CIS_Interaction* cisInteraction,
            IdleSync_Interaction* idleSyncInteraction,
            CPL_Interaction* cplInteraction,
            Sync_Interaction* syncInteraction,
            SyncLoop_Interaction* syncloopInteraction,
            InProd_Interaction* inProdInteraction) 
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