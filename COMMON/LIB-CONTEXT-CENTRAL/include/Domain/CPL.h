#include "CPLRepo.h"
#include "Sync.h"
#include <iostream>

class CPL
{
public:
    CPL();
    ~CPL();

    void CreateSync(std::string pathSync);

    void setId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation);
    void setDatas(std::string uuid, std::string name);
    void setCplInfos(std::string pathCpl);

    const int* getId() {return this->ids;}
    std::string getUuid() {return this->uuid;}
    std::string getName() {return this->name;}
    std::string getCplPath() {return this->pathCpl;}
    Sync* getSync() {return this->sync;}

private:
    CPLRepo* cplRepo;
    Sync* sync;
    
    int ids[4];
    std::string uuid;
    std::string name;
    std::string pathCpl;
};