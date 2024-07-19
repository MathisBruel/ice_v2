#pragma once
#include "LocalisationMovie.h"
#include "TypeMovie.h"
#include "CIS.h"
#include "CPL.h"
#include "SyncLoop.h"
#include <map>
class ReleaseRepo;
class Releases
{
public:
    Releases();
    ~Releases();

    void UploadCIS(std::string CISPath);
    void UploadCPL(int id_serv_pair_config, std::string uuid, std::string name, std::string cplPath);
    void UploadSyncLoop(int id_serv_pair_config, std::string SyncLoopPath);
    std::string toXmlString();

    void SetReleaseId(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    void SetReleaseInfos(std::string CPLRefPath) {this->_CPLRefPath = CPLRefPath;}
    void SetCIS(CIS* cis) {this->_cis = cis;}
    void AddCPL(std::string compositeId, CPLRelease* cpl) {this->_cpls[compositeId] = cpl;}
    void AddSyncLoop(std::string compositeId, SyncLoop* syncLoop) {this->_syncLoops[compositeId] = syncLoop;}

    int* GetReleaseId() {return this->_releaseId;}
    std::string GetCPLRefPath() {return this->_CPLRefPath;}
    CIS* GetCIS() {return this->_cis;}
    std::map<std::string, CPLRelease*> GetCPLs() {return this->_cpls;}
    CPLRelease* GetCPL(std::string compositeId) {return this->_cpls[compositeId];}
    std::map<std::string, SyncLoop*> GetSyncLoops() {return this->_syncLoops;}
    SyncLoop* GetSyncLoop(std::string compositeId) {return this->_syncLoops[compositeId];}

private:
    ReleaseRepo* _releaseRepo;
    CIS* _cis;
    std::map<std::string, CPLRelease*> _cpls;
    std::map<std::string, SyncLoop*> _syncLoops;

    int _releaseId[3];
    std::string _CPLRefPath;
};