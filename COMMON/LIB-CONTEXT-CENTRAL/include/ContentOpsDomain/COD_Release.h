#pragma once
#include "COD_LocalisationMovie.h"
#include "COD_TypeMovie.h"
#include "ContentOpsDomain/COD_Cpl.h"
#include <map>
#include <memory>

class COD_ReleaseRepo;
class COD_Releases
{
public:
    COD_Releases();
    COD_Releases(int id_content, int typeMovie, int localisationMovie);
    ~COD_Releases();

    void UploadCIS(std::string CISPath);
    void UploadCPL(int id_serv_pair_config, std::string uuid, std::string name, std::string cplPath);
    void UploadSyncLoop(std::string SyncLoopPath);
    
    void SetReleaseId(int id_content, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    void SetReleaseInfos(std::string CPLRefPath) {this->_CPLRefPath = CPLRefPath;}
    void SetTypeName(std::string typeName) {this->_typeName = typeName;}
    void SetLocalisationName(std::string localisationName) {this->_localisationName = localisationName;}
    void SetCISPath(std::string cisPath) {this->_cisPath = cisPath;}
    void AddCPL(std::string compositeId, COD_CplRelease* cpl) {this->_cpls[compositeId] = cpl;}
    void DeleteCPL(std::string compositeId) {this->_cpls.erase(compositeId);}

    const int* GetReleaseId() const {return this->_releaseId;}
    std::string GetCPLRefPath() const {return this->_CPLRefPath;}
    std::string GetTypeName() const {return this->_typeName;}
    std::string GetLocalisationName() const {return this->_localisationName;}
    std::string GetCISPath() const {return this->_cisPath;}
    std::map<std::string, COD_CplRelease*> GetCPLs() {return this->_cpls;}
    COD_CplRelease* GetCPL(std::string compositeId) {return this->_cpls[compositeId];}
    std::string GetSyncLoopPath() const { return this->_syncLoopPath; }
    void SetSyncLoopPath(const std::string& path) { this->_syncLoopPath = path; }

private:
    std::shared_ptr<COD_ReleaseRepo> _releaseRepo;
    
    std::map<std::string, COD_CplRelease*> _cpls;
    int _releaseId[3];
    std::string _CPLRefPath;
    std::string _typeName;
    std::string _localisationName;
    std::string _cisPath;
    std::string _syncLoopPath;
};