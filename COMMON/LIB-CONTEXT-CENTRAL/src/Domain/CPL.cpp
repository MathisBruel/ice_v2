#include "CPL.h"
CPL::CPL()
{
    this->ids[0] = -1;
    this->ids[1] = -1;
    this->ids[2] = -1;
    this->ids[3] = -1;
    this->name = "";
    this->uuid = "";
    this->pathCpl = "";
    this->sync = nullptr;
}
CPL::~CPL()
{
    if (this->sync != nullptr)
    {
        delete this->sync;
    }
}
void CPL::CreateSync(std::string pathSync)
{
    this->sync = new Sync();
    this->sync->setId(id[0], id[1], id[2], id[3]);
    this->sync->setSyncInfos(pathSync);
}
void CPL::setId(int id_serv_pair_config, int id_movie, int id_type, int id_localisation)
{
    this->ids[0] = id_serv_pair_config;
    this->ids[1] = id_movie;
    this->ids[2] = id_type;
    this->ids[3] = id_localisation;
}
void CPL::setDatas(std::string uuid, std::string name)
{
    this->uuid = uuid;
    this->name = name;
}
void CPL::setCplInfos(std::string pathCpl)
{
    this->pathCpl = pathCpl;
}