#pragma once
#include "ContentOpsDomain/COD_Localisation.h"

class COB_Localisation : public COD_Localisation
{
public:
    COB_Localisation();
    COB_Localisation(int id, std::string name);
    ~COB_Localisation();

    operator std::string() const;
}; 