#include "Domain/SyncLoop.h"
#include "Domain/SyncLoopRepo.h"
SyncLoop::SyncLoop()
{
    ids[0] = -1;
    ids[1] = -1;
    ids[2] = -1;
    ids[3] = -1;
    SyncLoopPath = "";
}
SyncLoop::~SyncLoop() {}
void SyncLoop::setId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation)
{
    ids[0] = id_serv_pair_config;
    ids[1] = id_movie;
    ids[2] = id_type;
    ids[3] = id_localisation;
}
void SyncLoop::setSyncLoopInfos(std::string SyncLoopPath)
{
    this->SyncLoopPath = SyncLoopPath;
}