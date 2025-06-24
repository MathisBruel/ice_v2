#include "ContentOpsDomain/Release.h"
#include "ContentOpsDomain/ReleaseRepo.h"
Releases::Releases()
{
    _releaseId[0] = -1;
    _releaseId[1] = -1;
    _releaseId[2] = -1;
    _CPLRefPath = "";
    _cis = nullptr;
}
Releases::Releases(int id_content, int typeMovie, int localisationMovie) {
    _releaseId[0] = id_content;
    _releaseId[1] = typeMovie;
    _releaseId[2] = localisationMovie;
    _CPLRefPath = "";
    _cis = nullptr;

}
Releases::~Releases() {
    if (_cis != nullptr) {
        delete _cis;
    }
}
void Releases::UploadCIS(std::string CISPath)
{
    this->_cis = new COD_CIS();
    this->_cis->SetCISInfos(CISPath);
    this->_cis->SetCISId(_releaseId[0], _releaseId[1], _releaseId[2]);
}
void Releases::UploadCPL(int id_serv_pair_config, std::string uuid, std::string name, std::string cplPath)
{
    std::string compositeId = std::to_string(id_serv_pair_config) + "_" 
                            + std::to_string(_releaseId[0]) + "_" 
                            + std::to_string(_releaseId[1]) + "_" 
                            + std::to_string(_releaseId[2]);
    this->_cpls[compositeId] = new CPLRelease();
    this->_cpls[compositeId]->SetCPLId(id_serv_pair_config, _releaseId[0], _releaseId[1], _releaseId[2]);
    this->_cpls[compositeId]->SetDatas(uuid, name);
    this->_cpls[compositeId]->SetCplInfos(cplPath);
}
void Releases::UploadSyncLoop(int id_serv_pair_config, std::string SyncLoopPath)
{
    std::string compositeId = std::to_string(id_serv_pair_config) + "_" 
                        + std::to_string(_releaseId[0]) + "_" 
                        + std::to_string(_releaseId[1]) + "_" 
                        + std::to_string(_releaseId[2]);
    this->_syncLoops[compositeId] = new SyncLoop();
    this->_syncLoops[compositeId]->SetSyncLoopId(id_serv_pair_config, _releaseId[0], _releaseId[1], _releaseId[2]);
    this->_syncLoops[compositeId]->SetSyncLoopInfos(SyncLoopPath);
}
void Releases::SetReleaseId(int id_content, TypeMovie typeMovie, LocalisationMovie localisationMovie)
{
    _releaseId[0] = id_content;
    _releaseId[1] = typeMovie;
    _releaseId[2] = localisationMovie;
}

std::string Releases::toXmlString(bool printChild)
{
    std::string xml = "<release";
    xml += " id_content=\"" + std::to_string(this->_releaseId[0]) + "\"";
    xml += " id_type=\"" + std::to_string(this->_releaseId[1]) + "\"";
    xml += " id_localisation=\"" + std::to_string(this->_releaseId[2]) + "\"";
    xml += " release_cpl_ref_path=\"" + this->_CPLRefPath + "\"";
    if (printChild) {
        xml += " release_cis_path=\"" + this->_cis->GetCISPath() + "\"";
    }
    xml += "/>";
    return xml;
}