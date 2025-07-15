#include "ContentOpsDomain/COD_Release.h"
#include "ContentOpsDomain/COD_ReleaseRepo.h"

COD_Releases::COD_Releases()
{
    _releaseId[0] = -1;
    _releaseId[1] = -1;
    _releaseId[2] = -1;
    _CPLRefPath = "";
    _typeName = "";
    _localisationName = "";
    _cisPath = "";
}

COD_Releases::COD_Releases(int id_content, int typeMovie, int localisationMovie) {
    _releaseId[0] = id_content;
    _releaseId[1] = typeMovie;
    _releaseId[2] = localisationMovie;
    _CPLRefPath = "";
    _typeName = "";
    _localisationName = "";
    _cisPath = "";
}

COD_Releases::~COD_Releases() {
}

void COD_Releases::UploadCIS(std::string CISPath)
{
    this->_cisPath = CISPath;
}

void COD_Releases::UploadCPL(int id_serv_pair_config, std::string uuid, std::string name, std::string cplPath)
{
    std::string compositeId = std::to_string(id_serv_pair_config) + "_" 
                            + std::to_string(_releaseId[0]) + "_" 
                            + std::to_string(_releaseId[1]) + "_" 
                            + std::to_string(_releaseId[2]);
    this->_cpls[compositeId] = new COD_CplRelease();
    this->_cpls[compositeId]->SetCPLId(id_serv_pair_config, _releaseId[0], _releaseId[1], _releaseId[2]);
    this->_cpls[compositeId]->SetDatas(uuid, name);
    this->_cpls[compositeId]->SetCplInfos(cplPath);
}

void COD_Releases::UploadSyncLoop(std::string SyncLoopPath)
{
    this->_syncLoopPath = SyncLoopPath;
}

void COD_Releases::SetReleaseId(int id_content, TypeMovie typeMovie, LocalisationMovie localisationMovie)
{
    _releaseId[0] = id_content;
    _releaseId[1] = typeMovie;
    _releaseId[2] = localisationMovie;
}
