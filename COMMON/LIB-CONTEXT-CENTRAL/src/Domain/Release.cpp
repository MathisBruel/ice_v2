#include "Release.h"

Release::Release()
{
    ids[0] = -1;
    ids[1] = -1;
    ids[2] = -1;
    cisPath = "";
    cis = nullptr;
    cpl = nullptr;
    syncLoop = nullptr;
}
Release::~Release() {
    if (cis != nullptr) {
        delete cis;
    }
    if (cpl != nullptr) {
        delete cpl;
    }
    if (syncLoop != nullptr) {
        delete syncLoop;
    }
}
void Release::UploadCIS(std::string cisPath)
{
    this->cis = new CIS();
    this->cis->setCISInfos(cisPath);
    this->cis->setId(id[0], ids[1], ids[2]);
}
void Release::UploadCPL(int id_serv_pair_config, std::string uuid, std::string name, std::string cplPath)
{
    this->cpl = new CPL();
    this->cpl->setId(id_serv_pair_config, ids[0], ids[1], ids[2]);
    this->cpl->setDatas(uuid, name);
    this->cpl->setCplInfos(cplPath);
}
void Release::UploadSyncLoop(int id_serv_pair_config, std::string syncLoopPath)
{
    this->syncLoop = new SyncLoop();
    this->syncLoop->setId(id_serv_pair_config, ids[0], ids[1], ids[2]);
    this->syncLoop->setSyncLoopInfos(syncLoopPath);
}
void Release::setId(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie)
{
    ids[0] = id_movie;
    ids[1] = typeMovie;
    ids[2] = localisationMovie;
}
void Release::setReleaseInfos(std::string cisPath) {this->cisPath = cisPath;}
void Release::setCIS(CIS* cis) {this->cis = cis;}
void Release::setCPL(CPL* cpl) {this->cpl = cpl;}
void Release::setSyncLoop(SyncLoop* syncLoop) {this->syncLoop = syncLoop;}

