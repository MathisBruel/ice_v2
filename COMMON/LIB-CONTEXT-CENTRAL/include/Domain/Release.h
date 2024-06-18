#pragma once
#include "ReleaseRepo.h"
#include "LocalisationMovie.h"
#include "TypeMovie.h"
#include "CIS.h"
#include "CPL.h"
#include "SyncLoop.h"

class Release
{
public:
    Release();
    ~Release();

    void UploadCIS(std::string cisPath);
    void UploadCPL(int id_serv_pair_config, std::string uuid, std::string name, std::string cplPath);
    void UploadSyncLoop(int id_serv_pair_config, std::string syncLoopPath);

    void setId(int id_movie, TypeMovie typeMovie, LocalisationMovie localisationMovie);
    void setReleaseInfos(std::string cisPath);
    void setCIS(CIS* cis);
    void setCPL(CPL* cpl);
    void setSyncLoop(SyncLoop* syncLoop);

    const int* getId() {return this->ids;}
    std::string getCISPath() {return this->cisPath;}
    CIS* getCIS() {return this->cis;}
    CPL* getCPL() {return this->cpl;}
    SyncLoop* getSyncLoop() {return this->syncLoop;}

private:
    ReleaseRepo* releaseRepo;
    CIS* cis;
    CPL* cpl;
    SyncLoop* syncLoop;

    int ids[3];
    std::string cisPath;
};