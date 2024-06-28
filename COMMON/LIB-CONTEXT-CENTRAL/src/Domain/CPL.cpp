#include "Domain/CPL.h"
#include "Domain/CPLRepo.h"
CPL::CPL()
{
    this->_CPLId[0] = -1;
    this->_CPLId[1] = -1;
    this->_CPLId[2] = -1;
    this->_CPLId[3] = -1;
    this->_CPLName = "";
    this->_CPLUUID = "";
    this->_CPLPath = "";
    this->_sync = nullptr;
}
CPL::~CPL()
{
    if (this->_sync != nullptr)
    {
        delete this->_sync;
    }
}
void CPL::CreateSync(std::string SyncPath)
{
    this->_sync = new Sync();
    this->_sync->SetSyncId(_CPLId[0], _CPLId[1], _CPLId[2], _CPLId[3]);
    this->_sync->SetSyncInfos(SyncPath);
}
void CPL::SetCPLId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation)
{
    this->_CPLId[0] = id_serv_pair_config;
    this->_CPLId[1] = id_movie;
    this->_CPLId[2] = id_type;
    this->_CPLId[3] = id_localisation;
}
void CPL::SetDatas(std::string CPLUUID, std::string CPLName)
{
    this->_CPLUUID = CPLUUID;
    this->_CPLName = CPLName;
}
void CPL::SetCplInfos(std::string CPLPath)
{
    this->_CPLPath = CPLPath;
}