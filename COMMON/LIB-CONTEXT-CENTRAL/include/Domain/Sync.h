#pragma once
#include <iostream>
class SyncRepo;
class Sync
{
public:
    Sync();
    ~Sync();

    void SetSyncId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation);
    void SetSyncInfos(std::string SyncPath);

    int* GetSyncID() {return this->_syncId;}
    std::string GetSyncPath() {return this->_syncPath;}
private:
    SyncRepo* _syncRepo;

    int _syncId[4];
    std::string _syncPath;
};