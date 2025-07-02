#pragma once
#include "ContentOpsDomain/COD_Cpl.h"

class COB_Cpl : public COD_Cpl
{
public:
    COB_Cpl();
    COB_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync = "", int type_cpl = -1, const std::string& sha1_sync = "", const std::string& path_cpl = "", long long id_serv_pair_config = -1, long long id_content = -1, long long id_type = -1, long long id_localisation = -1);
    ~COB_Cpl();

    void SetIdContent(int id_content) { _id_content = id_content; }
    void SetIdType(int id_type) { _id_type = id_type; }
    void SetIdLocalisation(int id_localisation) { _id_localisation = id_localisation; }
    int GetIdContent() const { return _id_content; }
    int GetIdType() const { return _id_type; }
    int GetIdLocalisation() const { return _id_localisation; }

    operator std::string() const;
private:
    int _id_content = -1;
    int _id_type = -1;
    int _id_localisation = -1;
}; 