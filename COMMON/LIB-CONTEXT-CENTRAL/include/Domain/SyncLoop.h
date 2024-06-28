#pragma once
#include <iostream>
class SyncLoopRepo;
class SyncLoop
{
public:
    SyncLoop();
    ~SyncLoop();

    void SetSyncLoopId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation);
    void SetSyncLoopInfos(std::string SyncLoopPath);

    int* GetSyncLoopId() {return this->_syncLoopId;}
    std::string GetSyncLoopPath() {return this->_SyncLoopPath;}

private:
    SyncLoopRepo* syncLoopRepo;

    int _syncLoopId[4];
    std::string _SyncLoopPath;
};