#include "machine.hpp"
#include "Domain/Content.h"
#include "Domain/Release.h"
#include "Domain/CIS.h"
#include "Domain/CPL.h"
#include "Domain/Sync.h"
#include "Domain/SyncLoop.h"

struct State_Idle;
struct State_ContentInit;
struct State_ReleaseCreation;
struct State_Publishing;
struct State_UploadCIS;
struct State_SyncCreate;
struct State_IdleSync;
struct State_CPL;
struct State_Sync;
struct State_SyncLoop;
struct State_Cancel;
struct State_InProd;

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
};

// Event 
struct InitContent {};
struct Publish {};
struct CreateCPL {};
struct CreateSync {};
struct CreateSyncLoop {};
struct SyncCreated {};
struct Upload {};
struct CreateRelease {};
struct ReleaseCreated {};
struct Stop {};