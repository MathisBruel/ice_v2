#pragma once
#include "COD_LocalisationMovie.h"
#include "COD_TypeMovie.h"
#include "COD_CIS.h"
#include "COD_CPL.h"
#include "COD_SyncLoop.h"
#include <map>
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
    std::string toXmlString(bool printChild);

    void SetReleaseId(int id_content, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    void SetReleaseInfos(std::string CPLRefPath) {this->_CPLRefPath = CPLRefPath;}
    void SetCIS(COD_CIS* cis) {this->_cis = cis;}
    void AddCPL(std::string compositeId, COD_CPLRelease* cpl) {this->_cpls[compositeId] = cpl;}
    void AddSyncLoop(std::string compositeId, COD_SyncLoop* syncLoop) {this->_syncLoops[compositeId] = syncLoop;}
    void DeleteCPL(std::string compositeId) {this->_cpls.erase(compositeId);}
    void DeleteSyncLoop(std::string compositeId) {this->_syncLoops.erase(compositeId);}

    int* GetReleaseId() {return this->_releaseId;}
    std::string GetCPLRefPath() {return this->_CPLRefPath;}
    COD_CIS* GetCIS() {return this->_cis;}
    std::map<std::string, COD_CPLRelease*> GetCPLs() {return this->_cpls;}
    COD_CPLRelease* GetCPL(std::string compositeId) {return this->_cpls[compositeId];}
    std::map<std::string, COD_SyncLoop*> GetSyncLoops() {return this->_syncLoops;}
    COD_SyncLoop* GetSyncLoop(std::string compositeId) {return this->_syncLoops[compositeId];}

private:
    COD_ReleaseRepo* _releaseRepo;
    COD_CIS* _cis;
    std::map<std::string, COD_CPLRelease*> _cpls;
    std::map<std::string, COD_SyncLoop*> _syncLoops;

    int _releaseId[3];
    std::string _CPLRefPath;
};