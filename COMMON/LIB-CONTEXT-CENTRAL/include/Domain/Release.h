#pragma once
#include "LocalisationMovie.h"
#include "TypeMovie.h"
#include "CIS.h"
#include "CPL.h"
#include "SyncLoop.h"
class ReleaseRepo;
class Releases
{
public:
    Releases();
    ~Releases();

    void UploadCIS(std::string CISPath);
    void UploadCPL(int id_serv_pair_config, std::string uuid, std::string name, std::string cplPath);
    void UploadSyncLoop(int id_serv_pair_config, std::string SyncLoopPath);

    void SetReleaseId(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    void SetReleaseInfos(std::string CISPath) {this->_CISPath = CISPath;}
    void SetCIS(CIS* cis) {this->_cis = cis;}
    void SetCPL(CPL* cpl) {this->_cpl = cpl;}
    void SetSyncLoop(SyncLoop* syncLoop) {this->_syncLoop = syncLoop;}

    int* GetReleaseId() {return this->_releaseId;}
    std::string GetCISPath() {return this->_CISPath;}
    CIS* GetCIS() {return this->_cis;}
    CPL* GetCPL() {return this->_cpl;}
    SyncLoop* GetSyncLoop() {return this->_syncLoop;}

private:
    ReleaseRepo* _releaseRepo;
    CIS* _cis;
    CPL* _cpl;
    SyncLoop* _syncLoop;

    int _releaseId[3];
    std::string _CISPath;
};