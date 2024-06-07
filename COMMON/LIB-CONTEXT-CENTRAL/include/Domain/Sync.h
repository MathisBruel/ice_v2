#include "SyncRepo.h"
#include <iostream>

class Sync
{
public:
    Sync();
    ~Sync();

    void setId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation);
    void setSyncInfos(std::string pathSync);

    const int* getID() {return this->ids;}
    std::string getSyncPath() {return this->pathSync;}
private:
    SyncRepo* syncRepo;

    int ids[4];
    std::string pathSync;
};