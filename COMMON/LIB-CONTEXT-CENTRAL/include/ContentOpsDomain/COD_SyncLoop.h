#pragma once
#include <iostream>
#include "ContentOpsDomain/COD_SyncLoopRepo.h"
class COD_SyncLoop
{
public:
    COD_SyncLoop();
    ~COD_SyncLoop();

    void SetSyncLoopId(int id_serv_pair_config, int id_content, int id_type, int id_localisation);
    void SetSyncLoopInfos(std::string SyncLoopPath);
    std::string toXmlString();

    int* GetSyncLoopId() {return this->_syncLoopId;}
    std::string GetSyncLoopPath() {return this->_SyncLoopPath;}

private:
    COD_SyncLoopRepo* _syncLoopRepo;

    int _syncLoopId[4];
    std::string _SyncLoopPath;
};