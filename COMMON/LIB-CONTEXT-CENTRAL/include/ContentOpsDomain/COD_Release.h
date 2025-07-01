#pragma once
#include "COD_LocalisationMovie.h"
#include "COD_TypeMovie.h"
#include "ContentOpsDomain/COD_Cpl.h"
#include "COD_SyncLoop.h"
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
    void UploadSyncLoop(int id_serv_pair_config, std::string SyncLoopPath);
    
    void SetReleaseId(int id_content, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    void SetReleaseInfos(std::string CPLRefPath) {this->_CPLRefPath = CPLRefPath;}
    void SetTypeName(std::string typeName) {this->_typeName = typeName;}
    void SetLocalisationName(std::string localisationName) {this->_localisationName = localisationName;}
    void SetCISPath(std::string cisPath) {this->_cisPath = cisPath;}
    void AddCPL(std::string compositeId, COD_CplRelease* cpl) {this->_cpls[compositeId] = cpl;}
    void AddSyncLoop(std::string compositeId, COD_SyncLoop* syncLoop) {this->_syncLoops[compositeId] = syncLoop;}
    void DeleteCPL(std::string compositeId) {this->_cpls.erase(compositeId);}
    void DeleteSyncLoop(std::string compositeId) {this->_syncLoops.erase(compositeId);}

    const int* GetReleaseId() const {return this->_releaseId;}
    std::string GetCPLRefPath() const {return this->_CPLRefPath;}
    std::string GetTypeName() const {return this->_typeName;}
    std::string GetLocalisationName() const {return this->_localisationName;}
    std::string GetCISPath() const {return this->_cisPath;}
    std::map<std::string, COD_CplRelease*> GetCPLs() {return this->_cpls;}
    COD_CplRelease* GetCPL(std::string compositeId) {return this->_cpls[compositeId];}
    std::map<std::string, COD_SyncLoop*> GetSyncLoops() {return this->_syncLoops;}
    COD_SyncLoop* GetSyncLoop(std::string compositeId) {return this->_syncLoops[compositeId];}

private:
    std::shared_ptr<COD_ReleaseRepo> _releaseRepo;
    
    std::map<std::string, COD_CplRelease*> _cpls;
    std::map<std::string, COD_SyncLoop*> _syncLoops;

    int _releaseId[3];
    std::string _CPLRefPath;
    std::string _typeName;
    std::string _localisationName;
    std::string _cisPath;
};