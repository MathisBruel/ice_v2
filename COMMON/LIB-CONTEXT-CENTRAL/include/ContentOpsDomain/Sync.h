#pragma once
#include <iostream>
class SyncRepo;
class Sync
{
public:
    Sync();

    void SetSyncId(int id_serv_pair_config, int id_content, int id_type, int id_localisation);
    void SetSyncInfos(std::string SyncPath);
    std::string toXmlString();

    int* GetSyncID() {return this->_syncId;}
    std::string GetSyncPath() {return this->_syncPath;}
private:
    SyncRepo* _syncRepo;

    int _syncId[4];
    std::string _syncPath;
};