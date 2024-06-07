#include "CIS.h"

CIS::CIS()
{
    ids[0] = -1;
    ids[1] = -1;
    ids[2] = -1;
    ids[3] = -1;
    pathCIS = "";
}
CIS::~CIS() {}
void CIS::setId(int id_movie, int id_type, int id_localisation)
{
    ids[0] = id_movie;
    ids[1] = id_type;
    ids[2] = id_localisation;
}
void CIS::setCISInfos(std::string pathCIS)
{
    this->pathCIS = pathCIS;
}