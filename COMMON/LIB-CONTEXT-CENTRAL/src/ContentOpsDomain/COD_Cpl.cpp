#include "ContentOpsDomain/COD_Cpl.h"
#include "ContentOpsDomain/COD_CplRepo.h"

COD_Cpl::COD_Cpl()
    : _id(-1), _name(""), _uuid(""), _pathSync("")
{
}

COD_Cpl::COD_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync)
    : _id(id), _name(name), _uuid(uuid), _pathSync(pathSync)
{
}

COD_Cpl::~COD_Cpl()
{
} 