#pragma once
#include "Sync.h"
#include <iostream>
class CPLRepo;
class CPLRelease
{
public:
    CPLRelease();
    ~CPLRelease();

    void CreateSync(std::string SyncPath);

    void SetCPLId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation);
    void SetDatas(std::string CPLUUID, std::string CPLName);
    void SetCplInfos(std::string CPLPath);
    void DeleteSync() {delete this->_sync;}
    std::string toXmlString();

    int* GetCPLId() {return this->_CPLId;}
    std::string GetCPLUUID() {return this->_CPLUUID;}
    std::string GetCPLName() {return this->_CPLName;}
    std::string GetCPLPath() {return this->_CPLPath;}
    Sync* GetSync() {return this->_sync;}

private:
    CPLRepo* _cplRepo;
    Sync* _sync;
    
    int _CPLId[4];
    std::string _CPLUUID;
    std::string _CPLName;
    std::string _CPLPath;
};