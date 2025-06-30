#pragma once
#include "ContentOpsDomain/COD_Type.h"

class COB_Type : public COD_Type
{
public:
    COB_Type();
    COB_Type(int id, std::string name);
    ~COB_Type();

    operator std::string() const;
}; 