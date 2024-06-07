#include "machine.hpp"

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