#include "ContentOpsDomain/COD_Cpl.h"
#include "ContentOpsDomain/COD_CplRepo.h"

COD_Cpl::COD_Cpl()
    : _id(-1), _name(""), _uuid(""), _type_cpl(-1), _sha1_sync(""), _path_cpl(""), _pathSync(""), _id_serv_pair_config(-1), _id_content(-1), _id_type(-1), _id_localisation(-1)
{
}

COD_Cpl::COD_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync)
    : _id(id), _name(name), _uuid(uuid), _type_cpl(-1), _sha1_sync(""), _path_cpl(""), _pathSync(pathSync), _id_serv_pair_config(-1), _id_content(-1), _id_type(-1), _id_localisation(-1)
{
}

COD_Cpl::~COD_Cpl()
{
} 