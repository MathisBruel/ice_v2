#include "ContentOpsDomain/COD_CIS.h"
#include "ContentOpsDomain/CISRepo.h"
COD_CIS::COD_CIS()
{
    _CISId[0] = -1;
    _CISId[1] = -1;
    _CISId[2] = -1;
    _CISPath = "";
}
COD_CIS::~COD_CIS() {}
void COD_CIS::SetCISId(int id_content, int id_type, int id_localisation)
{
    _CISId[0] = id_content;
    _CISId[1] = id_type;
    _CISId[2] = id_localisation;
}
void COD_CIS::SetCISInfos(std::string _CISPath)
{
    this->_CISPath = _CISPath;
}