#include "Domain/CIS.h"
#include "Domain/CISRepo.h"
CIS::CIS()
{
    _CISId[0] = -1;
    _CISId[1] = -1;
    _CISId[2] = -1;
    _CISPath = "";
}
CIS::~CIS() {}
void CIS::SetCISId(int id_movie, int id_type, int id_localisation)
{
    _CISId[0] = id_movie;
    _CISId[1] = id_type;
    _CISId[2] = id_localisation;
}
void CIS::SetCISInfos(std::string _CISPath)
{
    this->_CISPath = _CISPath;
}