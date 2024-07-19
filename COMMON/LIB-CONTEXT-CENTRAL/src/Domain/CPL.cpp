#include "Domain/CPL.h"
#include "Domain/CPLRepo.h"
CPLRelease::CPLRelease()
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
CPLRelease::~CPLRelease()
{
    if (this->_sync != nullptr)
    {
        delete this->_sync;
    }
}
void CPLRelease::CreateSync(std::string SyncPath)
{
    this->_sync = new Sync();
    this->_sync->SetSyncId(_CPLId[0], _CPLId[1], _CPLId[2], _CPLId[3]);
    this->_sync->SetSyncInfos(SyncPath);
}
void CPLRelease::SetCPLId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation)
{
    this->_CPLId[0] = id_serv_pair_config;
    this->_CPLId[1] = id_movie;
    this->_CPLId[2] = id_type;
    this->_CPLId[3] = id_localisation;
}
void CPLRelease::SetDatas(std::string CPLUUID, std::string CPLName)
{
    this->_CPLUUID = CPLUUID;
    this->_CPLName = CPLName;
}
void CPLRelease::SetCplInfos(std::string CPLPath)
{
    this->_CPLPath = CPLPath;
}

std::string CPLRelease::toXmlString()
{
    std::string xml = "<cpl";
    xml += " id_serv_pair_config=\"" + std::to_string(this->_CPLId[0]) + "\"";
    xml += " id_movie=\"" + std::to_string(this->_CPLId[1]) + "\"";
    xml += " id_type=\"" + std::to_string(this->_CPLId[2]) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->_CPLId[3]) + "\"";
    xml += " CPL_name=\"" + this->_CPLName + "\"";
    xml += " CPL_uuid=\"" + this->_CPLUUID + "\"";
    xml += " CPL_path=\"" + this->_CPLPath + "\"";
    xml += "/>";
    return xml;
}