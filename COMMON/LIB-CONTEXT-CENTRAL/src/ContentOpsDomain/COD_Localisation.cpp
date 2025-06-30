#include "ContentOpsDomain/COD_Localisation.h"

COD_Localisation::COD_Localisation()
{
    _id = -1;
    _name = "";
}

COD_Localisation::COD_Localisation(int id, std::string name)
{
    _id = id;
    _name = name;
}

COD_Localisation::~COD_Localisation()
{
} 