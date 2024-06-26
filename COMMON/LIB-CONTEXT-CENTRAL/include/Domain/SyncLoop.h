#pragma once
#include <iostream>
class SyncLoopRepo;
class SyncLoop
{
public:
    SyncLoop();
    ~SyncLoop();

    void setId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation);
    void setSyncLoopInfos(std::string SyncLoopPath);

    int* getId() {return this->ids;}
    std::string getSyncLoopPath() {return this->SyncLoopPath;}

private:
    SyncLoopRepo* syncLoopRepo;

    int ids[4];
    std::string SyncLoopPath;
};