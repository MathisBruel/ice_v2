#pragma once
#include <iostream>
#include "machine.hpp"
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

struct Context
{
    bool CISFinish = false;
    bool SyncFinish = false;
    Content content;
    Release release;
    CIS cis;
    CPL cpl;
    Sync sync;
    SyncLoop syncLoop;

    Content_Interaction contentInteraction;
    Release_Interaction releaseInteraction;
    Publishing_Interaction publishingInteraction;
    CIS_Interaction cisInteraction;
    IdleSync_Interaction idleSyncInteraction;
    CPL_Interaction cplInteraction;
    Sync_Interaction syncInteraction;
    SyncLoop_Interaction syncloopInteraction;
};