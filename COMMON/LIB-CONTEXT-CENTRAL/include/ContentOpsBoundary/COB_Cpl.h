#pragma once
#include "ContentOpsDomain/COD_Cpl.h"

class COB_Cpl : public COD_Cpl
{
public:
    COB_Cpl();
    COB_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync = "");
    COB_Cpl(int id, const std::string& name, const std::string& uuid, const std::string& pathSync, int id_content, int id_type, int id_localisation);
    ~COB_Cpl();

    void SetIdContent(int id_content) { _id_content = id_content; }
    void SetIdType(int id_type) { _id_type = id_type; }
    void SetIdLocalisation(int id_localisation) { _id_localisation = id_localisation; }
    int GetIdContent() const { return _id_content; }
    int GetIdType() const { return _id_type; }
    int GetIdLocalisation() const { return _id_localisation; }

    // Conversion en XML
    operator std::string() const;
private:
    int _id_content = -1;
    int _id_type = -1;
    int _id_localisation = -1;
}; 