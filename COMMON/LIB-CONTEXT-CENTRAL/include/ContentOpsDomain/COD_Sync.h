#pragma once
#include <iostream>
#include "ContentOpsDomain/COD_SyncRepo.h"
class COD_Sync
{
public:
    COD_Sync();

    void SetSyncId(int id_serv_pair_config, int id_content, int id_type, int id_localisation);
    void SetSyncInfos(std::string SyncPath);

    const int* GetSyncID() const {return this->_syncId;}
    std::string GetSyncPath() const {return this->_syncPath;}
private:
    COD_SyncRepo* _syncRepo;

    int _syncId[4];
    std::string _syncPath;
};