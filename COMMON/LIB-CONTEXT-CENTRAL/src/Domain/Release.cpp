#include "Domain/Release.h"
#include "Domain/ReleaseRepo.h"
Releases::Releases()
{
    _releaseId[0] = -1;
    _releaseId[1] = -1;
    _releaseId[2] = -1;
    _CISPath = "";
    _cis = nullptr;
    _cpl = nullptr;
    _syncLoop = nullptr;
}
Releases::~Releases() {
    if (_cis != nullptr) {
        delete _cis;
    }
    if (_cpl != nullptr) {
        delete _cpl;
    }
    if (_syncLoop != nullptr) {
        delete _syncLoop;
    }
}
void Releases::UploadCIS(std::string CISPath)
{
    this->_cis = new CIS();
    this->_cis->SetCISInfos(CISPath);
    this->_cis->SetCISId(_releaseId[0], _releaseId[1], _releaseId[2]);
}
void Releases::UploadCPL(int id_serv_pair_config, std::string uuid, std::string name, std::string cplPath)
{
    this->_cpl = new CPL();
    this->_cpl->SetCPLId(id_serv_pair_config, _releaseId[0], _releaseId[1], _releaseId[2]);
    this->_cpl->SetDatas(uuid, name);
    this->_cpl->SetCplInfos(cplPath);
}
void Releases::UploadSyncLoop(int id_serv_pair_config, std::string SyncLoopPath)
{
    this->_syncLoop = new SyncLoop();
    this->_syncLoop->SetSyncLoopId(id_serv_pair_config, _releaseId[0], _releaseId[1], _releaseId[2]);
    this->_syncLoop->SetSyncLoopInfos(SyncLoopPath);
}
void Releases::SetReleaseId(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie)
{
    _releaseId[0] = id_movie;
    _releaseId[1] = typeMovie;
    _releaseId[2] = localisationMovie;
}